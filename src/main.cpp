#include <./headers/arduino_config.h>
#include <./headers/wifi_config.h>
#include <./headers/websocket_config.h>
#include <./headers/servo_config.h>
#include <./headers/camera_config.h>
#include <./headers/cam_server_config.h>
#define CAMERA_MODEL_AI_THINKER

const int servoPin = 13;

void setup() {

   esp_log_level_set("*", ESP_LOG_VERBOSE);
  Serial.begin(115200);
  //servo.attach(servoPin);

  WiFi.softAP(ssid, password);
  Serial.print(WiFi.softAPIP());

  initCamera();
  startCameraServer();

  websocket.begin();
  websocket.onEvent(onWebSocketEvent);

 
}

void loop() {
 websocket.loop();
}


