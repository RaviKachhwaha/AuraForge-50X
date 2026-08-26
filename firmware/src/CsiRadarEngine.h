/**
 * @file CsiRadarEngine.h
 * @brief AuraForge 50X - Wi-Fi CSI Sensing & RuView Live Stream Engine
 * @author Ravi Kachhwaha
 */

#ifndef CSI_RADAR_ENGINE_H
#define CSI_RADAR_ENGINE_H

#include <Arduino.h>
#include <esp_wifi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

#define CSI_SUBCARRIERS 64
#define DEFAULT_CSI_UDP_PORT 5000

struct CsiTelemetryData {
    bool enabled = false;
    bool udpStreaming = false;
    uint32_t packetCount = 0;
    float motionIndex = 0.0f;
    int8_t rssi = -50;
    uint8_t subcarriers[CSI_SUBCARRIERS] = {0};
    char pcIp[32] = "255.255.255.255";
    uint16_t pcPort = DEFAULT_CSI_UDP_PORT;
};

class CsiRadarEngine {
public:
    CsiRadarEngine();
    void begin();
    void setEnabled(bool enable);
    void setUdpStream(bool enable, const char* pcIp = "255.255.255.255", uint16_t port = DEFAULT_CSI_UDP_PORT);
    bool isEnabled() const { return telemetry.enabled; }
    bool isUdpStreaming() const { return telemetry.udpStreaming; }

    void processCsiPacket(const wifi_csi_info_t *info);
    String getCsiJson();
    const CsiTelemetryData& getTelemetry() const { return telemetry; }

private:
    CsiTelemetryData telemetry;
    WiFiUDP udpClient;
    unsigned long lastUdpSend = 0;
};

extern CsiRadarEngine g_csiRadarEngine;

#endif // CSI_RADAR_ENGINE_H
