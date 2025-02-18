#include <./headers/cam_server_config.h>

AsyncWebServer server(80);

void handleJPEG(AsyncWebServerRequest *request) {

    request->send_P(200, "multipart/x-mixed-replace; boundary=frame", "", 0);
    while (request->client()->connected()) {
        camera_fb_t *fb = esp_camera_fb_get();
        if (!fb) {
            Serial.println("Camera capture failed");
            break;
        }

        AsyncWebServerResponse *response = request->beginChunkedResponse("image/jpeg", [fb](uint8_t *buffer, size_t maxLen, size_t index) -> size_t {
            if (index == 0) {
                memcpy(buffer, fb->buf, fb->len);
                return fb->len;
            }
            return 0;
        });

        request->send(response);
        esp_camera_fb_return(fb);
        if (!request->client()->connected()) break;
    }
}

void startCameraServer() {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", 
        "<html><body><h1>ESP32-CAM Stream</h1><img src='/stream' width='100%'></body></html>");
    });

    server.on("/stream", HTTP_GET, handleJPEG);

    server.begin();
}