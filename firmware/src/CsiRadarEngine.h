/**
 * @file CsiRadarEngine.h
 * @brief AuraForge 50X - Wi-Fi CSI Spatial Presence State Machine & RuView Live
 * Stream Engine
 * @author Ravi Kachhwaha
 */

#ifndef CSI_RADAR_ENGINE_H
#define CSI_RADAR_ENGINE_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFiUdp.h>
#include <esp_wifi.h>

#define CSI_SUBCARRIERS 64
#define DEFAULT_CSI_UDP_PORT 5000

enum PresenceState {
  PRESENCE_VACANT = 0, // Room is empty, no motion
  PRESENCE_DETECTED,   // Initial movement detected
  PRESENCE_ACTIVE,     // Continuous occupant activity
  PRESENCE_COOLDOWN    // Motion ceased, countdown before vacant
};

struct CsiTelemetryData {
  bool enabled = false;
  bool udpStreaming = false;
  uint32_t packetCount = 0;
  float motionIndex = 0.0f;
  float subcarrierVariance = 0.0f;
  int8_t rssi = -50;
  uint8_t subcarriers[CSI_SUBCARRIERS] = {0};
  char pcIp[32] = "192.168.4.2";
  uint16_t pcPort = DEFAULT_CSI_UDP_PORT;
  uint8_t streamRateHz = 50;

  // Spatial Presence State Machine Telemetry
  PresenceState presenceState = PRESENCE_VACANT;
  uint32_t activeDurationSeconds = 0;
  uint16_t cooldownRemainingSeconds = 0;
  bool automationTriggered = false;
};

class CsiRadarEngine {
public:
  CsiRadarEngine();
  void begin();
  void update();
  void setEnabled(bool enable);
  void setUdpStream(bool enable, const char *pcIp = "192.168.4.2",
                    uint16_t port = DEFAULT_CSI_UDP_PORT, uint8_t rateHz = 50);
  bool isEnabled() const { return telemetry.enabled; }
  bool isUdpStreaming() const { return telemetry.udpStreaming; }
  bool sendTestUdpPacket();

  void processCsiPacket(const wifi_csi_info_t *info);
  String getCsiJson();
  const CsiTelemetryData &getTelemetry() const { return telemetry; }
  PresenceState getPresenceState() const { return telemetry.presenceState; }

private:
  CsiTelemetryData telemetry;
  WiFiUDP udpClient;
  unsigned long lastUdpSend = 0;
  unsigned long lastMotionTime = 0;
  unsigned long lastStateTick = 0;
  unsigned long activeStartTime = 0;

  void updatePresenceStateMachine();
};

extern CsiRadarEngine g_csiRadarEngine;

#endif // CSI_RADAR_ENGINE_H
