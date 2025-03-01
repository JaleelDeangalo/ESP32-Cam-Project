#include <./headers/cam_server_config.h>
#include <esp_task_wdt.h>
#include <ESPAsyncWebServer.h>

#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_ARDUHAL_ESP_LOG)
#include "esp32-hal-log.h"

// Include the HTML gzipped files
extern const uint8_t index_ov3660_html_gz[];
extern const size_t index_ov3660_html_gz_len;
extern const uint8_t index_ov5640_html_gz[];
extern const size_t index_ov5640_html_gz_len;
extern const uint8_t index_ov2640_html_gz[];
extern const size_t index_ov2640_html_gz_len;
// Enable LED FLASH setting
#define CONFIG_LED_ILLUMINATOR_ENABLED 1

// LED FLASH setup
#if CONFIG_LED_ILLUMINATOR_ENABLED

#define LED_LEDC_GPIO            22  //configure LED pin
#define CONFIG_LED_MAX_INTENSITY 255

int led_duty = 0;
bool isStreaming = true;

void enable_led(bool enable) {
    if (enable) {
        ledcWrite(0, CONFIG_LED_MAX_INTENSITY);
    } else {
        ledcWrite(0, 0);
    }
}
#endif

#endif

#define PART_BOUNDARY "123456789000000000000987654321"
static const char *_STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char *_STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char *_STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

AsyncWebServer server(80);

typedef struct {
    size_t size;   //number of values used for filtering
    size_t index;  //current value index
    size_t count;  //value count
    int sum;
    int *values;  //array to be filled with values
} ra_filter_t;

static ra_filter_t ra_filter;

static ra_filter_t *ra_filter_init(ra_filter_t *filter, size_t sample_size) {
    memset(filter, 0, sizeof(ra_filter_t));
  
    filter->values = (int *)malloc(sample_size * sizeof(int));
    if (!filter->values) {
        return NULL;
    }
    memset(filter->values, 0, sample_size * sizeof(int));
  
    filter->size = sample_size;
    return filter;
}

static void handleBMP(AsyncWebServerRequest *request) {
    camera_fb_t * fb = esp_camera_fb_get();
    if (!fb) {
        log_e("Camera capture failed");
        request->send(500, "text/plain", "Camera capture failed");
        return;
    }

    AsyncWebServerResponse *response = request->beginResponse_P(200, "image/bmp", fb->buf, fb->len);
    response->addHeader("Content-Disposition", "inline; filename=capture.bmp");
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
    esp_camera_fb_return(fb);
}

static void stream_handler(AsyncWebServerRequest *request) {
    Serial.println("Stream Handler Called");
    if (!isStreaming) {
        Serial.println("Streaming is disabled");
        request->send(404, "Stream is set to false", "Not found");
        return;
    }

    

    AsyncWebServerResponse *response = request->beginChunkedResponse(_STREAM_CONTENT_TYPE, [](uint8_t *buffer, size_t maxLen, size_t index) -> size_t {
        Serial.println("Inside chunked response callback");
        if (!isStreaming) {
            Serial.println("Streaming is disabled inside chunked response");
            return 0;
        }

        camera_fb_t * fb = esp_camera_fb_get();
        if (!fb) {
            log_e("Camera capture failed");
            return 0;
        }

        size_t hlen = snprintf((char *)buffer, maxLen, _STREAM_PART, fb->len);
        size_t len = hlen + fb->len + strlen(_STREAM_BOUNDARY);

        if (len > maxLen) {
            esp_camera_fb_return(fb);
            Serial.println("Frame size exceeds buffer size");
            return 0;
        }

        memcpy(buffer + hlen, fb->buf, fb->len);
        memcpy(buffer + hlen + fb->len, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));

        esp_camera_fb_return(fb);
        Serial.printf("Stream frame sent, length: %u\n", len);

        // Add a delay to control the frame rate
        delay(100); // Increase the delay to control the frame rate

        //esp_task_wdt_reset(); // Reset the watchdog timer

      
        return len;
    }); 

    response->addHeader("Cache-Control", "no-store, no-cache, must-revalidate, pre-check=0, post-check=0, max-age=0");
    response->addHeader("Pragma", "no-cache");
    response->addHeader("Connection", "close");
    request->send(response);
    Serial.println("Stream response sent");
}

static void handleRoot(AsyncWebServerRequest *request) {
    Serial.println("Root handler called");
    request->send(200, "text/html", "<html><body><h1>ESP32 Camera Stream</h1><img src=\"/stream\" /></body></html>");
}

void startCameraServer() {
    server.on("/", HTTP_GET, handleRoot);
    server.on("/bmp", HTTP_GET, handleBMP);
    server.on("/stream", HTTP_GET, stream_handler);
    server.begin();
    Serial.println("Camera server started");
}