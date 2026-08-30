/**
 * @file CsiRadarEngine.cpp
 * @brief AuraForge 50X - Wi-Fi CSI Spatial Presence State Machine & RuView Live
 * Stream Engine Implementation
 * @author Ravi Kachhwaha
 */

#include "CsiRadarEngine.h"
#include "ConfigManager.h"
#include "DspEngine.h"
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

  const SystemConfig &cfg = g_configManager.getConfig();
  setUdpStream(cfg.csiStreamingEnabled, cfg.csiPcIp, cfg.csiPcPort,
               cfg.csiStreamRateHz);
  Serial.println("[CSI Radar Engine] Wi-Fi 802.11n 64-Subcarrier Sensing & "
                 "RuView UDP Engine Initialized.");
}

void CsiRadarEngine::setEnabled(bool enable) {
  telemetry.enabled = enable;
  esp_wifi_set_csi(enable);
}

void CsiRadarEngine::setUdpStream(bool enable, const char *pcIp, uint16_t port,
                                  uint8_t rateHz) {
  telemetry.udpStreaming = enable;
  if (pcIp && strlen(pcIp) > 0) {
    strncpy(telemetry.pcIp, pcIp, sizeof(telemetry.pcIp) - 1);
    telemetry.pcIp[sizeof(telemetry.pcIp) - 1] = '\0';
  }
  telemetry.pcPort = (port > 0) ? port : DEFAULT_CSI_UDP_PORT;
  telemetry.streamRateHz = (rateHz > 0) ? rateHz : 50;
  Serial.printf("[CSI UDP] Stream %s -> Target PC IP: %s:%u @ %u Hz\n",
                enable ? "ENABLED" : "DISABLED", telemetry.pcIp,
                telemetry.pcPort, telemetry.streamRateHz);
}

bool CsiRadarEngine::sendTestUdpPacket() {
  udpClient.beginPacket(telemetry.pcIp, telemetry.pcPort);
  udpClient.printf("CSI_TEST,%u,%d,%.2f,%.2f,", telemetry.packetCount,
                   telemetry.rssi, telemetry.motionIndex,
                   telemetry.subcarrierVariance);
  for (int i = 0; i < CSI_SUBCARRIERS; i++) {
    udpClient.printf("%u%c", telemetry.subcarriers[i],
                     (i == CSI_SUBCARRIERS - 1) ? '\n' : ',');
  }
  bool ok = (udpClient.endPacket() == 1);
  Serial.printf("[CSI UDP] Test packet transmitted to %s:%u (%s)\n",
                telemetry.pcIp, telemetry.pcPort, ok ? "SUCCESS" : "FAILED");
  return ok;
}

void CsiRadarEngine::processCsiPacket(const wifi_csi_info_t *info) {
  if (!telemetry.enabled || !info)
    return;

  telemetry.packetCount++;
  telemetry.rssi = info->rx_ctrl.rssi;

  int len = info->len;
  int8_t *data = (int8_t *)info->buf;

  float subcarrierSum = 0.0f;
  float prevSubSum = 0.0f;
  float rawMotion = 0.0f;

  for (int i = 0; i < CSI_SUBCARRIERS && (i * 2 + 1) < len; i++) {
    int8_t real = data[i * 2];
    int8_t imag = data[i * 2 + 1];
    float mag = sqrtf((float)(real * real + imag * imag));
    uint8_t val = (uint8_t)min(255.0f, mag * 4.0f);

    rawMotion += fabsf((float)val - (float)telemetry.subcarriers[i]);
    prevSubSum += telemetry.subcarriers[i];
    telemetry.subcarriers[i] = val;
    subcarrierSum += val;
  }

  // 1. Calculate dynamic spatial perturbation / motion index
  float instantaneousMotion = rawMotion / (float)CSI_SUBCARRIERS;
  telemetry.motionIndex =
      telemetry.motionIndex * 0.75f + instantaneousMotion * 0.25f;

  // 2. Calculate OFDM Subcarrier Variance (Spatial Perturbation Dispersion)
  float meanMag = subcarrierSum / (float)CSI_SUBCARRIERS;
  float varianceSum = 0.0f;
  for (int i = 0; i < CSI_SUBCARRIERS; i++) {
    float diff = (float)telemetry.subcarriers[i] - meanMag;
    varianceSum += (diff * diff);
  }
  telemetry.subcarrierVariance = varianceSum / (float)CSI_SUBCARRIERS;

  // Stream raw packet to RuView / PC over UDP if enabled
  unsigned long intervalMs =
      1000 / (telemetry.streamRateHz > 0 ? telemetry.streamRateHz : 50);
  if (telemetry.udpStreaming && (millis() - lastUdpSend >= intervalMs)) {
    lastUdpSend = millis();
    udpClient.beginPacket(telemetry.pcIp, telemetry.pcPort);
    // Header format for RuView / ESP-CSI Tool / Python Visualizer
    udpClient.printf("CSI,%u,%d,%.2f,%.2f,", telemetry.packetCount,
                     telemetry.rssi, telemetry.motionIndex,
                     telemetry.subcarrierVariance);
    for (int i = 0; i < CSI_SUBCARRIERS; i++) {
      udpClient.printf("%u%c", telemetry.subcarriers[i],
                       (i == CSI_SUBCARRIERS - 1) ? '\n' : ',');
    }
    udpClient.endPacket();
  }
}

void CsiRadarEngine::updatePresenceStateMachine() {
  const SystemConfig &cfg = g_configManager.getConfig();
  float threshold = cfg.csiSensitivity;        // Default 5.0
  uint16_t timeoutSec = cfg.csiTimeoutSeconds; // Default 120s
  unsigned long now = millis();

  bool motionDetected = (telemetry.motionIndex > threshold);

  if (motionDetected) {
    lastMotionTime = now;
  }

  switch (telemetry.presenceState) {
  case PRESENCE_VACANT:
    if (motionDetected) {
      telemetry.presenceState = PRESENCE_DETECTED;
      activeStartTime = now;
      Serial.println("[CSI State Machine] Presence DETECTED in room!");
      // Touchless Room Automation: Auto-Unmute on entry
      if (cfg.csiPresenceAutomation && cfg.csiAutoPlay &&
          g_configManager.getConfig().isMuted) {
        g_configManager.getConfig().isMuted = false;
        g_dspEngine.updateFromConfig(g_configManager.getConfig());
        Serial.println(
            "[CSI Automation] Triggered Auto-Unmute on occupant entry.");
      }
    }
    telemetry.activeDurationSeconds = 0;
    telemetry.cooldownRemainingSeconds = 0;
    break;

  case PRESENCE_DETECTED:
    if (now - activeStartTime >=
        1500) { // Sustained motion > 1.5s transitions to Active
      telemetry.presenceState = PRESENCE_ACTIVE;
      Serial.println("[CSI State Machine] Presence state ACTIVE.");
    }
    if (!motionDetected && (now - lastMotionTime > 3000)) {
      telemetry.presenceState = PRESENCE_COOLDOWN;
    }
    break;

  case PRESENCE_ACTIVE:
    telemetry.activeDurationSeconds = (now - activeStartTime) / 1000;
    if (!motionDetected && (now - lastMotionTime > 3000)) {
      telemetry.presenceState = PRESENCE_COOLDOWN;
      Serial.println(
          "[CSI State Machine] Motion ceased. Entering COOLDOWN mode.");
    }
    break;

  case PRESENCE_COOLDOWN:
    if (motionDetected) {
      telemetry.presenceState = PRESENCE_ACTIVE;
      Serial.println(
          "[CSI State Machine] Motion resumed. Returning to ACTIVE.");
    } else {
      unsigned long elapsedSinceMotion = (now - lastMotionTime) / 1000;
      if (elapsedSinceMotion >= timeoutSec) {
        telemetry.presenceState = PRESENCE_VACANT;
        telemetry.cooldownRemainingSeconds = 0;
        Serial.println("[CSI State Machine] Room confirmed VACANT.");
        // Touchless Room Automation: Auto-Mute on exit
        if (cfg.csiPresenceAutomation && cfg.csiAutoMute &&
            !g_configManager.getConfig().isMuted) {
          g_configManager.getConfig().isMuted = true;
          g_dspEngine.updateFromConfig(g_configManager.getConfig());
          Serial.println(
              "[CSI Automation] Triggered Auto-Mute on room vacancy.");
        }
      } else {
        telemetry.cooldownRemainingSeconds =
            (uint16_t)(timeoutSec - elapsedSinceMotion);
      }
    }
    break;
  }
}

void CsiRadarEngine::update() {
  unsigned long now = millis();
  if (now - lastStateTick >= 200) { // 5 Hz State Machine evaluation loop
    lastStateTick = now;
    updatePresenceStateMachine();
  }
}

String CsiRadarEngine::getCsiJson() {
  StaticJsonDocument<1280> doc;
  doc["type"] = "csi_radar";
  doc["enabled"] = telemetry.enabled;
  doc["udpStreaming"] = telemetry.udpStreaming;
  doc["pcIp"] = telemetry.pcIp;
  doc["pcPort"] = telemetry.pcPort;
  doc["streamRateHz"] = telemetry.streamRateHz;
  doc["packets"] = telemetry.packetCount;
  doc["rssi"] = telemetry.rssi;
  doc["motion"] = telemetry.motionIndex;
  doc["variance"] = telemetry.subcarrierVariance;

  // Presence State Machine
  doc["presenceState"] = (int)telemetry.presenceState;
  doc["activeDuration"] = telemetry.activeDurationSeconds;
  doc["cooldownRemaining"] = telemetry.cooldownRemainingSeconds;

  JsonArray subs = doc.createNestedArray("subcarriers");
  for (int i = 0; i < CSI_SUBCARRIERS; i++) {
    subs.add(telemetry.subcarriers[i]);
  }

  String out;
  serializeJson(doc, out);
  return out;
}
