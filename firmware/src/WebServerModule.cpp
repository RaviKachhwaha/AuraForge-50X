/**
 * @file WebServerModule.cpp
 * @brief AuraForge 50X - Async Web Server, WebSocket Telemetry, Dual-Bank OTA & REST API Engine
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
#include <esp_ota_ops.h>

#ifndef AURAFORGE_VERSION
#define AURAFORGE_VERSION "1.0.0"
#endif

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
        StaticJsonDocument<1024> doc;
        DeserializationError err = deserializeJson(doc, (char*)data);
        if (!err) {
            if (doc.containsKey("action")) {
                const char* act = doc["action"];
                if (strcmp(act, "reboot") == 0) {
                    delay(200);
                    ESP.restart();
                } else if (strcmp(act, "reset") == 0) {
                    g_configManager.resetToDefaults();
                    g_dspEngine.updateFromConfig(g_configManager.getConfig());
                } else if (strcmp(act, "savePreset") == 0) {
                    uint8_t slot = doc["slot"] | 0;
                    const char* name = doc["name"] | "Custom Preset";
                    g_configManager.saveUserPreset(slot, name, g_configManager.getConfig().eqGains);
                } else if (strcmp(act, "loadPreset") == 0) {
                    uint8_t slot = doc["slot"] | 0;
                    if (g_configManager.loadUserPreset(slot, g_configManager.getConfig().eqGains)) {
                        g_dspEngine.updateFromConfig(g_configManager.getConfig());
                    }
                } else if (strcmp(act, "testCsiUdp") == 0) {
                    bool ok = g_csiRadarEngine.sendTestUdpPacket();
                    char resp[96];
                    snprintf(resp, sizeof(resp), "{\"type\":\"csi_test_result\",\"success\":%s}", ok ? "true" : "false");
                    if (client) client->text(resp);
                }
            } else {
                if (g_configManager.updateFromJson(doc)) {
                    const SystemConfig& cfg = g_configManager.getConfig();
                    g_dspEngine.updateFromConfig(cfg);
                    g_csiRadarEngine.setEnabled(cfg.csiStreamingEnabled || cfg.csiPresenceAutomation);
                    g_csiRadarEngine.setUdpStream(cfg.csiStreamingEnabled, cfg.csiPcIp, cfg.csiPcPort, cfg.csiStreamRateHz);
                }
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

    // GET /api/v1/presets
    server.on("/api/v1/presets", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "application/json", g_configManager.getUserPresetsJson());
    });

    // POST /api/v1/presets/save
    AsyncCallbackJsonWebHandler *savePresetHandler = new AsyncCallbackJsonWebHandler("/api/v1/presets/save", [](AsyncWebServerRequest *request, JsonVariant &json) {
        JsonObject obj = json.as<JsonObject>();
        uint8_t slot = obj["slot"] | 0;
        const char* name = obj["name"] | "Custom Preset";
        bool ok = g_configManager.saveUserPreset(slot, name, g_configManager.getConfig().eqGains);
        request->send(ok ? 200 : 400, "application/json", ok ? "{\"status\":\"ok\"}" : "{\"status\":\"error\"}");
    });
    server.addHandler(savePresetHandler);

    // POST /api/v1/presets/load
    AsyncCallbackJsonWebHandler *loadPresetHandler = new AsyncCallbackJsonWebHandler("/api/v1/presets/load", [](AsyncWebServerRequest *request, JsonVariant &json) {
        JsonObject obj = json.as<JsonObject>();
        uint8_t slot = obj["slot"] | 0;
        bool ok = g_configManager.loadUserPreset(slot, g_configManager.getConfig().eqGains);
        if (ok) {
            g_dspEngine.updateFromConfig(g_configManager.getConfig());
        }
        request->send(ok ? 200 : 400, "application/json", ok ? "{\"status\":\"ok\"}" : "{\"status\":\"error\"}");
    });
    server.addHandler(loadPresetHandler);

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

    // GET /api/v1/thermal
    server.on("/api/v1/thermal", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "application/json", g_hardwareMonitor.getTelemetryJson());
    });

    // GET /api/v1/ota/status (Dual-Bank Flash Info)
    server.on("/api/v1/ota/status", HTTP_GET, [](AsyncWebServerRequest *request) {
        StaticJsonDocument<512> doc;
        const esp_partition_t* running = esp_ota_get_running_partition();
        const esp_partition_t* next = esp_ota_get_next_update_partition(NULL);

        doc["runningPartition"] = running ? running->label : "ota_0";
        doc["nextPartition"] = next ? next->label : "ota_1";
        doc["runningAddress"] = running ? running->address : 0x10000;
        doc["partitionSize"] = running ? running->size : 0x1D0000;
        doc["freeSketchSpace"] = ESP.getFreeSketchSpace();
        doc["sketchSize"] = ESP.getSketchSize();
        doc["firmwareVersion"] = AURAFORGE_VERSION;
        doc["chipModel"] = ESP.getChipModel();

        String res;
        serializeJson(doc, res);
        request->send(200, "application/json", res);
    });

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

    // Dual-Bank Asynchronous Web OTA Firmware Flashing Handler
    server.on("/update", HTTP_POST, [](AsyncWebServerRequest *request) {
        bool failure = Update.hasError();
        AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", failure ? "FAIL" : "OK");
        response->addHeader("Connection", "close");
        request->send(response);
        if (!failure) {
            Serial.println("[OTA Flasher] Swap bank complete. Rebooting into new firmware...");
            delay(500);
            ESP.restart();
        }
    }, [this](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
        if (!index) {
            const esp_partition_t* next = esp_ota_get_next_update_partition(NULL);
            Serial.printf("[OTA Flasher] Flashing image '%s' to target partition '%s'...\n", 
                          filename.c_str(), next ? next->label : "ota_1");
            if (!Update.begin(UPDATE_SIZE_UNKNOWN, U_FLASH)) {
                Update.printError(Serial);
            }
        }
        if (!Update.hasError()) {
            if (Update.write(data, len) != len) {
                Update.printError(Serial);
            } else {
                // Broadcast live flashing progress to connected WebSocket clients
                if (request->contentLength() > 0) {
                    uint8_t progress = (uint8_t)(((index + len) * 100) / request->contentLength());
                    char progressMsg[64];
                    snprintf(progressMsg, sizeof(progressMsg), "{\"type\":\"ota_progress\",\"progress\":%u}", progress);
                    this->ws.textAll(progressMsg);
                }
            }
        }
        if (final) {
            if (Update.end(true)) {
                Serial.printf("[OTA Flasher] Dual-Bank Update Success: %u bytes verified.\n", index + len);
                this->ws.textAll("{\"type\":\"ota_complete\",\"success\":true}");
            } else {
                Update.printError(Serial);
                this->ws.textAll("{\"type\":\"ota_complete\",\"success\":false}");
            }
        }
    });
}

void WebServerModule::broadcastTelemetry() {
    if (ws.count() == 0) return;

    // 1. Hardware & Closed-Loop Thermal Governor Telemetry Packet
    StaticJsonDocument<768> telemDoc;
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
    dataObj["cpuFreqMHz"] = telem.cpuFreqMHz;

    // Thermal Governor Telemetry
    dataObj["temperatureC"] = telem.temperatureC;
    dataObj["temperatureF"] = telem.temperatureF;
    dataObj["governorState"] = (int)telem.governorState;
    dataObj["thermalAttenuation_dB"] = telem.thermalAttenuation_dB;
    dataObj["thermalGainMultiplier"] = telem.thermalGainMultiplier;
    dataObj["drcGainReduction"] = g_dspEngine.getDrcGainReduction();

    String telemStr;
    serializeJson(telemDoc, telemStr);
    ws.textAll(telemStr);

    // 2. 128-Point Radix-2 FFT Spectrum Telemetry Packet (16 Bands)
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

    // 3. Wi-Fi CSI Spatial Presence State Machine Telemetry Packet
    if (g_csiRadarEngine.isEnabled()) {
        ws.textAll(g_csiRadarEngine.getCsiJson());
    }
}

void WebServerModule::update() {
    ws.cleanupClients();

    unsigned long now = millis();
    if (now - lastWsBroadcast >= 60) { // High frame-rate telemetry broadcast (~16 FPS)
        lastWsBroadcast = now;
        broadcastTelemetry();
    }
}
