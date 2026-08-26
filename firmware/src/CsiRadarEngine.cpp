/**
 * @file CsiRadarEngine.cpp
 * @brief AuraForge 50X - Wi-Fi CSI Sensing & RuView Live Stream Engine Implementation
 * @author Ravi Kachhwaha
 */

#include "CsiRadarEngine.h"
#include <esp_wifi.h>

CsiRadarEngine g_csiRadarEngine;

static void IRAM_ATTR _wifi_csi_cb(void *ctx, wifi_csi_info_t *info) {
    if (info && info->buf) {
        g_csiRadarEngine.processCsiPacket(info);
    }
}

CsiRadarEngine::CsiRadarEngine() {}

void CsiRadarEngine::begin() {
    wifi_csi_config_t csi_config;
    memset(&csi_config, 0, sizeof(csi_config));
    csi_config.lltf_en = true;
    csi_config.htltf_en = true;
    csi_config.stbc_htltf2_en = true;
    csi_config.channel_filter_en = true;

    esp_wifi_set_csi_config(&csi_config);
    esp_wifi_set_csi_rx_cb(_wifi_csi_cb, NULL);
}

void CsiRadarEngine::setEnabled(bool enable) {
    telemetry.enabled = enable;
    esp_wifi_set_csi(enable);
}

void CsiRadarEngine::setUdpStream(bool enable, const char* pcIp, uint16_t port) {
    telemetry.udpStreaming = enable;
    if (pcIp && strlen(pcIp) > 0) {
        strncpy(telemetry.pcIp, pcIp, sizeof(telemetry.pcIp));
    }
    telemetry.pcPort = port;
}

void CsiRadarEngine::processCsiPacket(const wifi_csi_info_t *info) {
    if (!telemetry.enabled || !info) return;

    telemetry.packetCount++;
    telemetry.rssi = info->rx_ctrl.rssi;

    int len = info->len;
    int8_t *data = (int8_t*)info->buf;

    float subcarrierSum = 0.0f;
    for (int i = 0; i < CSI_SUBCARRIERS && (i * 2 + 1) < len; i++) {
        int8_t real = data[i * 2];
        int8_t imag = data[i * 2 + 1];
        float mag = sqrtf((float)(real * real + imag * imag));
        uint8_t val = (uint8_t)min(255.0f, mag * 4.0f);
        
        telemetry.motionIndex += fabsf((float)val - (float)telemetry.subcarriers[i]);
        telemetry.subcarriers[i] = val;
        subcarrierSum += val;
    }
    telemetry.motionIndex = (telemetry.motionIndex * 0.1f);

    // Stream raw packet to RuView / PC over UDP if enabled
    if (telemetry.udpStreaming && (millis() - lastUdpSend >= 20)) { // 50 Hz UDP Packet Stream to PC
        lastUdpSend = millis();
        udpClient.beginPacket(telemetry.pcIp, telemetry.pcPort);
        // Header format for RuView / ESP-CSI Tool
        udpClient.printf("CSI,%u,%d,%.2f,", telemetry.packetCount, telemetry.rssi, telemetry.motionIndex);
        for (int i = 0; i < CSI_SUBCARRIERS; i++) {
            udpClient.printf("%u%c", telemetry.subcarriers[i], (i == CSI_SUBCARRIERS - 1) ? '\n' : ',');
        }
        udpClient.endPacket();
    }
}

String CsiRadarEngine::getCsiJson() {
    StaticJsonDocument<1024> doc;
    doc["type"] = "csi_radar";
    doc["enabled"] = telemetry.enabled;
    doc["udpStreaming"] = telemetry.udpStreaming;
    doc["pcIp"] = telemetry.pcIp;
    doc["pcPort"] = telemetry.pcPort;
    doc["packets"] = telemetry.packetCount;
    doc["rssi"] = telemetry.rssi;
    doc["motion"] = telemetry.motionIndex;

    JsonArray subs = doc.createNestedArray("subcarriers");
    for (int i = 0; i < CSI_SUBCARRIERS; i++) {
        subs.add(telemetry.subcarriers[i]);
    }

    String out;
    serializeJson(doc, out);
    return out;
}
