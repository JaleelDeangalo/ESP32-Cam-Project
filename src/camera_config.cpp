#include <./headers/camera_config.h>
#define CAMERA_MODEL_AI_THINKER

void initCamera() {
    Serial.println("Configuring camera...");
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG; 

    // Check if PSRAM is available and configure the frame buffer location accordingly
    if (psramFound()) {
        config.fb_location = CAMERA_FB_IN_PSRAM;
        config.frame_size = FRAMESIZE_QVGA; // Increase frame size to 320x240
        config.jpeg_quality = 10; // Increase quality to reduce compression artifacts
        config.fb_count = 2; // Set to 2 to allocate two frame buffers
        Serial.println("PSRAM found. Frame buffers will be allocated in PSRAM.");
    } else {
        config.fb_location = CAMERA_FB_IN_DRAM;
        config.frame_size = FRAMESIZE_QVGA; // Increase frame size to 320x240
        config.jpeg_quality = 10; // Increase quality to reduce compression artifacts
        config.fb_count = 1; // Set to 1 to allocate one frame buffer
        Serial.println("PSRAM not found. Frame buffers will be allocated in DRAM.");
    }

    Serial.println("Initializing camera with config...");
    if (esp_camera_init(&config) != ESP_OK) {
        Serial.println("Camera init failed!");
        return;
    } else {
        Serial.println("Camera initialized successfully.");
    }
}