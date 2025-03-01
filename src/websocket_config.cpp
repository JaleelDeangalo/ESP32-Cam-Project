#include <./headers/websocket_config.h>
#include <./headers/servo_config.h>

WebSocketsServer websocket = WebSocketsServer(81);
Servo servo;

void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {

  switch (type) {
    case WStype_DISCONNECTED:
    Serial.printf("[%u] Disconnected!\n", num);
    break;

    case WStype_CONNECTED: {
      IPAddress ip = websocket.remoteIP(num);
      Serial.printf("[%u] Connetion from: ", num);
      Serial.println(ip.toString());
      websocket.sendTXT(num, "Connected!");
    }
    break;

    case WStype_TEXT: {
      String message = String((char*) payload);
      Serial.println(message);
      int angle = message.toInt();
      if(angle >= 0 && angle <= 180) {
        servo.write(angle);
      }
    }
    break;

    case WStype_BIN: {
      Serial.printf("Data received: \n", num, length);
    }
    break;

    case WStype_ERROR: {
      websocket.sendTXT(num, "Error Has Occured");
    }
    break;
    
  
  }
}

void sendFrame(uint8_t num) {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  websocket.sendBIN(num, fb->buf, fb->len);
  esp_camera_fb_return(fb);
}

void handleVideoStream() {
  for (int i = 0; i < websocket.connectedClients(); i++) {
    sendFrame(i);
  }
}