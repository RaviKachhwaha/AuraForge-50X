/**
 * @file WifiManagerModule.cpp
 * @brief AuraForge 50X - Dual AP/STA Wi-Fi Manager, Captive Portal & mDNS Responder Implementation
 * @author Ravi Kachhwaha
 */

#include "WifiManagerModule.h"

WifiManagerModule g_wifiManager;

const byte DNS_PORT = 53;

WifiManagerModule::WifiManagerModule() {}

void WifiManagerModule::begin(const SystemConfig& config) {
    WiFi.persistent(false);
    WiFi.setAutoReconnect(true);

    if (config.wifiApEnabled || strlen(config.wifiStaSsid) == 0) {
        startAccessPoint(config.wifiApSsid, config.wifiApPass);
    } else {
        WiFi.mode(WIFI_STA);
    }

    if (strlen(config.wifiStaSsid) > 0) {
        connectStation(config.wifiStaSsid, config.wifiStaPass, config.useStaticIp, config.staticIp, config.staticGateway, config.staticSubnet);
    }

    setupMdns(config.mdnsHost);
}

void WifiManagerModule::startAccessPoint(const char* apSsid, const char* apPass) {
    if (WiFi.getMode() == WIFI_STA) {
        WiFi.mode(WIFI_AP_STA);
    } else if (WiFi.getMode() != WIFI_AP_STA) {
        WiFi.mode(WIFI_AP);
    }

    IPAddress apIP(192, 168, 4, 1);
    IPAddress netMsk(255, 255, 255, 0);
    WiFi.softAPConfig(apIP, apIP, netMsk);

    bool res = WiFi.softAP(apSsid, apPass);
    if (res) {
        wifiActiveAP = true;
        Serial.printf("[Wi-Fi AP] Started Access Point '%s' at IP %s\n", apSsid, WiFi.softAPIP().toString().c_str());
        
        // Start Captive Portal DNS server (redirects all domain queries to 192.168.4.1)
        dnsServer.start(DNS_PORT, "*", apIP);
        captivePortalActive = true;
    } else {
        Serial.println("[Wi-Fi AP] Failed to start Access Point!");
    }
}

bool WifiManagerModule::connectStation(const char* ssid, const char* pass, bool staticIp, const char* ip, const char* gw, const char* sub) {
    if (strlen(ssid) == 0) return false;

    if (wifiActiveAP) {
        WiFi.mode(WIFI_AP_STA);
    } else {
        WiFi.mode(WIFI_STA);
    }

    if (staticIp && strlen(ip) > 0) {
        IPAddress ipAddr, gwAddr, subAddr;
        if (ipAddr.fromString(ip) && gwAddr.fromString(gw) && subAddr.fromString(sub)) {
            WiFi.config(ipAddr, gwAddr, subAddr);
            Serial.printf("[Wi-Fi STA] Configuring Static IP: %s\n", ip);
        }
    }

    Serial.printf("[Wi-Fi STA] Connecting to '%s'...\n", ssid);
    WiFi.begin(ssid, pass);

    uint8_t timeout = 0;
    while (WiFi.status() != WL_CONNECTED && timeout < 20) {
        delay(250);
        Serial.print(".");
        timeout++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        wifiConnectedSTA = true;
        Serial.printf("\n[Wi-Fi STA] Connected! Assigned IP: %s\n", WiFi.localIP().toString().c_str());
        return true;
    } else {
        wifiConnectedSTA = false;
        Serial.println("\n[Wi-Fi STA] Connection timed out or password incorrect.");
        return false;
    }
}

void WifiManagerModule::setupMdns(const char* hostname) {
    if (MDNS.begin(hostname)) {
        MDNS.addService("http", "tcp", 80);
        Serial.printf("[mDNS] Hostname registered: http://%s.local\n", hostname);
    } else {
        Serial.println("[mDNS] Error setting up mDNS responder!");
    }
}

void WifiManagerModule::update() {
    if (captivePortalActive) {
        dnsServer.processNextRequest();
    }

    // Background STA Reconnect Manager
    if (!wifiConnectedSTA && WiFi.getMode() != WIFI_OFF) {
        unsigned long now = millis();
        if (now - lastReconnectAttempt > 30000) {
            lastReconnectAttempt = now;
            if (WiFi.status() == WL_CONNECTED) {
                wifiConnectedSTA = true;
            }
        }
    }
}

String WifiManagerModule::scanNetworksJson() {
    int n = WiFi.scanNetworks();
    StaticJsonDocument<1536> doc;
    JsonArray netArr = doc.createNestedArray("networks");

    for (int i = 0; i < n && i < 15; ++i) {
        JsonObject obj = netArr.createNestedObject();
        obj["ssid"] = WiFi.SSID(i);
        obj["rssi"] = WiFi.RSSI(i);
        obj["secure"] = (WiFi.encryptionType(i) != WIFI_AUTH_OPEN);
        obj["channel"] = WiFi.channel(i);
    }

    WiFi.scanDelete();

    String jsonOutput;
    serializeJson(doc, jsonOutput);
    return jsonOutput;
}

String WifiManagerModule::getLocalIp() const {
    if (wifiConnectedSTA) {
        return WiFi.localIP().toString();
    }
    return "0.0.0.0";
}

String WifiManagerModule::getApIp() const {
    if (wifiActiveAP) {
        return WiFi.softAPIP().toString();
    }
    return "0.0.0.0";
}

int32_t WifiManagerModule::getRssi() const {
    if (wifiConnectedSTA) {
        return WiFi.RSSI();
    }
    return -100;
}
