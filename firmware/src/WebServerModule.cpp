/**
 * @file WebServerModule.cpp
 * @brief AuraForge 50X - Async Web Server, WebSocket Telemetry & REST API Engine Implementation
 * @author Ravi Kachhwaha
 */

#include "WebServerModule.h"
#include "WebUiAssets.h"
#include "ConfigManager.h"
#include "DspEngine.h"
#include "HardwareMonitor.h"
#include "WifiManagerModule.h"
#include "CsiRadarEngine.h"
#include <Update.h>

WebServerModule g_webServerModule;

WebServerModule::WebServerModule() : server(80), ws("/ws") {}

void WebServerModule::begin() {
    setupWebSockets();
    setupRoutes();
    server.begin();
    Serial.println("[Web Server] Async HTTP Server & WebSocket telemetry listening on port 80.");
}

void WebServerModule::setupWebSockets() {
    ws.onEvent([this](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
        if (type == WS_EVT_CONNECT) {
            Serial.printf("[WebSocket] Client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        } else if (type == WS_EVT_DISCONNECT) {
            Serial.printf("[WebSocket] Client #%u disconnected\n", client->id());
        } else if (type == WS_EVT_DATA) {
            handleWebSocketMessage(arg, data, len, client);
        }
    });
    server.addHandler(&ws);
}

void WebServerModule::handleWebSocketMessage(void *arg, uint8_t *data, size_t len, AsyncWebSocketClient *client) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        data[len] = 0;
        StaticJsonDocument<512> doc;
        DeserializationError err = deserializeJson(doc, (char*)data);
        if (!err) {
            if (g_configManager.updateFromJson(doc)) {
                g_dspEngine.updateFromConfig(g_configManager.getConfig());
            }
        }
    }
}

void WebServerModule::setupRoutes() {
    // Serve Web UI Single Page App
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/html", INDEX_HTML);
    });

    // GET /api/v1/status
    server.on("/api/v1/status", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "application/json", g_configManager.toJsonString());
    });

    // POST /api/v1/status
    AsyncCallbackJsonWebHandler *statusPostHandler = new AsyncCallbackJsonWebHandler("/api/v1/status", [](AsyncWebServerRequest *request, JsonVariant &json) {
        JsonObject jsonObj = json.as<JsonObject>();
        StaticJsonDocument<1024> doc;
        doc.set(jsonObj);
        if (g_configManager.updateFromJson(doc)) {
            g_dspEngine.updateFromConfig(g_configManager.getConfig());
            request->send(200, "application/json", "{\"status\":\"ok\"}");
        } else {
            request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Invalid body\"}");
        }
    });
    server.addHandler(statusPostHandler);

    // GET /api/v1/eq
    server.on("/api/v1/eq", HTTP_GET, [](AsyncWebServerRequest *request) {
        StaticJsonDocument<256> doc;
        JsonArray arr = doc.createNestedArray("eqGains");
        for (int i = 0; i < NUM_EQ_BANDS; i++) {
            arr.add(g_configManager.getConfig().eqGains[i]);
        }
        String res;
        serializeJson(doc, res);
        request->send(200, "application/json", res);
    });

    // POST /api/v1/eq
    AsyncCallbackJsonWebHandler *eqPostHandler = new AsyncCallbackJsonWebHandler("/api/v1/eq", [](AsyncWebServerRequest *request, JsonVariant &json) {
        JsonObject jsonObj = json.as<JsonObject>();
        StaticJsonDocument<512> doc;
        doc.set(jsonObj);
        if (g_configManager.updateFromJson(doc)) {
            g_dspEngine.updateFromConfig(g_configManager.getConfig());
            request->send(200, "application/json", "{\"status\":\"ok\"}");
        } else {
            request->send(400, "application/json", "{\"status\":\"error\"}");
        }
    });
    server.addHandler(eqPostHandler);

    // GET /api/v1/wifi/scan
    server.on("/api/v1/wifi/scan", HTTP_GET, [](AsyncWebServerRequest *request) {
        String jsonScan = g_wifiManager.scanNetworksJson();
        request->send(200, "application/json", jsonScan);
    });

    // POST /api/v1/audio/generator
    AsyncCallbackJsonWebHandler *genHandler = new AsyncCallbackJsonWebHandler("/api/v1/audio/generator", [](AsyncWebServerRequest *request, JsonVariant &json) {
        JsonObject obj = json.as<JsonObject>();
        int mode = obj["mode"] | 0;
        float freq = obj["freq"] | 1000.0f;
        g_dspEngine.setSignalGenerator((SignalGenWaveform)mode, freq);
        request->send(200, "application/json", "{\"status\":\"ok\"}");
    });
    server.addHandler(genHandler);

    // POST /api/v1/system/reboot
    server.on("/api/v1/system/reboot", HTTP_POST, [](AsyncWebServerRequest *request) {
        request->send(200, "application/json", "{\"status\":\"rebooting\"}");
        delay(500);
        ESP.restart();
    });

    // POST /api/v1/system/reset
    server.on("/api/v1/system/reset", HTTP_POST, [](AsyncWebServerRequest *request) {
        g_configManager.resetToDefaults();
        g_dspEngine.updateFromConfig(g_configManager.getConfig());
        request->send(200, "application/json", "{\"status\":\"reset_complete\"}");
    });

    // Over-The-Air Firmware Update Handler
    server.on("/update", HTTP_POST, [](AsyncWebServerRequest *request) {
        bool failure = Update.hasError();
        AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", failure ? "FAIL" : "OK");
        response->addHeader("Connection", "close");
        request->send(response);
        if (!failure) {
            delay(500);
            ESP.restart();
        }
    }, [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
        if (!index) {
            Serial.printf("[OTA Update] Firmware update started: %s\n", filename.c_str());
            if (!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & ~0xFFF)) {
                Update.printError(Serial);
            }
        }
        if (!Update.hasError()) {
            if (Update.write(data, len) != len) {
                Update.printError(Serial);
            }
        }
        if (final) {
            if (Update.end(true)) {
                Serial.printf("[OTA Update] Firmware update success: %u bytes\n", index + len);
            } else {
                Update.printError(Serial);
            }
        }
    });
}

void WebServerModule::broadcastTelemetry() {
    if (ws.count() == 0) return;

    // 1. Hardware Telemetry Packet
    StaticJsonDocument<512> telemDoc;
    telemDoc["type"] = "telemetry";
    JsonObject dataObj = telemDoc.createNestedObject("data");
    const HardwareTelemetry& telem = g_hardwareMonitor.getTelemetry();
    dataObj["ampEnabled"] = telem.ampEnabled;
    dataObj["ampFaultDetected"] = telem.ampFaultDetected;
    dataObj["faultCount"] = telem.faultCount;
    dataObj["batteryVoltage"] = telem.estimatedBatteryVoltage;
    dataObj["batteryPercent"] = telem.estimatedBatteryPercent;
    dataObj["freeHeap"] = telem.freeHeapBytes;
    dataObj["uptimeSeconds"] = telem.uptimeSeconds;

    String telemStr;
    serializeJson(telemDoc, telemStr);
    ws.textAll(telemStr);

    // 2. Spectrum Visualizer Telemetry Packet
    StaticJsonDocument<512> specDoc;
    specDoc["type"] = "spectrum";
    JsonArray specArr = specDoc.createNestedArray("data");
    uint8_t mags[FFT_BANDS];
    g_dspEngine.getSpectrumMagnitudes(mags, FFT_BANDS);
    for (int i = 0; i < FFT_BANDS; i++) {
        specArr.add(mags[i]);
    }

    String specStr;
    serializeJson(specDoc, specStr);
    ws.textAll(specStr);

    // 3. Wi-Fi CSI Radar Subcarrier Sensing Packet
    if (g_csiRadarEngine.isEnabled()) {
        ws.textAll(g_csiRadarEngine.getCsiJson());
    }
}

void WebServerModule::update() {
    ws.cleanupClients();

    unsigned long now = millis();
    if (now - lastWsBroadcast >= 80) { // Broadcast telemetry at ~12 FPS
        lastWsBroadcast = now;
        broadcastTelemetry();
    }
}
