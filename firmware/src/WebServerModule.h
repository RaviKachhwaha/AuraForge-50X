/**
 * @file WebServerModule.h
 * @brief AuraForge 50X - Async Web Server, WebSocket Telemetry & REST API Engine
 * @author Ravi Kachhwaha
 */

#ifndef WEB_SERVER_MODULE_H
#define WEB_SERVER_MODULE_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>
#include <AsyncTCP.h>
#include <ArduinoJson.h>

class WebServerModule {
public:
    WebServerModule();
    void begin();
    void update();

private:
    AsyncWebServer server;
    AsyncWebSocket ws;

    void setupRoutes();
    void setupWebSockets();
    void handleWebSocketMessage(void *arg, uint8_t *data, size_t len, AsyncWebSocketClient *client);
    void broadcastTelemetry();

    unsigned long lastWsBroadcast = 0;
};

extern WebServerModule g_webServerModule;

#endif // WEB_SERVER_MODULE_H
