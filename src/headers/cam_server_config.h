#if !defined(CAM_SERVER_CONFIG_H)
#define CAM_SERVER_CONFIG_H

#include <ESPAsyncWebServer.h>
#include <./headers/cam_object.h>
#include <WiFi.h>

extern AsyncWebServer server;

void startCameraServer();



#endif