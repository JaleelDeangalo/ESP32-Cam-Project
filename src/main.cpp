#include <./headers/arduino_config.h>
#include <./headers/wifi_config.h>
#include <./headers/websocket_config.h>
#include <./headers/servo_config.h>
#include <./headers/camera_config.h>
#include <./headers/cam_server_config.h>
#include <esp_task_wdt.h>
#define CAMERA_MODEL_AI_THINKER

const int servoPin = 13;
const int redLED = 25;
const int greenLED = 26;
const int blueLED = 27;

void setup() {
  esp_log_level_set("*", ESP_LOG_VERBOSE);
  Serial.begin(115200);
  Serial.println("Setup started");

  servo.attach(servoPin);
  pinMode(redLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(greenLED, OUTPUT);

  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("WiFi IPAddress: ");
  Serial.println(WiFi.localIP());

  // Increase the task watchdog timeout
  //esp_task_wdt_init(120, false); // 120 seconds timeout, disable panic

  Serial.println("Initializing camera...");
  initCamera();

  Serial.println("Starting camera server...");
  startCameraServer();

  Serial.println("Starting websocket...");
  websocket.begin();
  websocket.onEvent(onWebSocketEvent);

  Serial.println("Setup completed");
}

void loop() {
  websocket.loop();
  //esp_task_wdt_reset(); // Reset the watchdog timer
}


