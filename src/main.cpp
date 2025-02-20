#include <./headers/arduino_config.h>
#include <./headers/wifi_config.h>
#include <./headers/websocket_config.h>
#include <./headers/servo_config.h>
#include <./headers/camera_config.h>
#include <./headers/cam_server_config.h>
#define CAMERA_MODEL_AI_THINKER

const int servoPin = 13;
const int redLED = 25;
const int greenLED = 26;
const int blueLED = 27;

void setup() {

  esp_log_level_set("*", ESP_LOG_VERBOSE);
  Serial.begin(115200);
  servo.attach(servoPin);
  pinMode(redLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  ledcAttachPin(10, 0);
  ledcSetup(0, 1000, 8);

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


