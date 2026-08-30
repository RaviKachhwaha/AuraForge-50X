/**
 * @file WifiManagerModule.h
 * @brief AuraForge 50X - Dual AP/STA Wi-Fi Manager, Captive Portal & mDNS
 * Responder
 * @author Ravi Kachhwaha
 */

#ifndef WIFI_MANAGER_MODULE_H
#define WIFI_MANAGER_MODULE_H

#include "ConfigManager.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <WiFi.h>

class WifiManagerModule {
public:
  WifiManagerModule();
  void begin(const SystemConfig &config);
  void update();

  // Wi-Fi Controls
  bool connectStation(const char *ssid, const char *pass, bool staticIp = false,
                      const char *ip = "", const char *gw = "",
                      const char *sub = "");
  void startAccessPoint(const char *apSsid, const char *apPass);

  // Wi-Fi Site Survey Scanner
  String scanNetworksJson();

  // Status Queries
  bool isConnectedSTA() const { return wifiConnectedSTA; }
  bool isAPActive() const { return wifiActiveAP; }
  String getLocalIp() const;
  String getApIp() const;
  int32_t getRssi() const;

private:
  DNSServer dnsServer;
  bool wifiConnectedSTA = false;
  bool wifiActiveAP = false;
  bool captivePortalActive = false;
  unsigned long lastReconnectAttempt = 0;

  void setupMdns(const char *hostname);
};

extern WifiManagerModule g_wifiManager;

#endif // WIFI_MANAGER_MODULE_H
