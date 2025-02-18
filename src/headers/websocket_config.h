#if !defined(WEBSOCKET_CONFIG_H)
#define WEBSOCKET_CONFIG_H

#include <WebSocketsServer.h>

extern WebSocketsServer websocket;

void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);


#endif