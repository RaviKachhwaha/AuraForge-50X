/**
 * @file ConfigManager.h
 * @brief AuraForge 50X - Non-Volatile Storage (NVS) Configuration Manager
 * @author Ravi Kachhwaha
 */

#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <Arduino.h>
#include <Preferences.h>
#include <ArduinoJson.h>

#define NUM_EQ_BANDS 10

struct SystemConfig {
    // Audio / DSP Settings
    uint8_t volume = 80;
    bool isMuted = false;
    float eqGains[NUM_EQ_BANDS] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    float preAmpGain = 0.0f;           // -12dB to +12dB
    float balance = 0.0f;              // -1.0 (Left) to +1.0 (Right)
    bool monoMode = false;             // Downmix to mono
    bool phaseInvertL = false;
    bool phaseInvertR = false;
    float dynamicBassGain = 3.0f;      // 0.0 to 12.0 dB
    float dynamicBassFreq = 80.0f;     // 40Hz to 150Hz
    bool dynamicBassEnabled = true;
    float stereoWidth = 1.0f;          // 0.0 (Mono) to 2.0 (Ultra-wide)
    float highPassCutoff = 20.0f;      // 20Hz - 100Hz
    float subLowPassCutoff = 150.0f;   // 60Hz - 250Hz
    bool loudnessCompensation = false;

    // Network & Wi-Fi Settings
    char wifiStaSsid[32] = "";
    char wifiStaPass[64] = "";
    char wifiApSsid[32] = "AuraForge-50X-AP";
    char wifiApPass[64] = "auraforge123";
    bool wifiApEnabled = true;
    bool useStaticIp = false;
    char staticIp[16] = "192.168.1.200";
    char staticGateway[16] = "192.168.1.1";
    char staticSubnet[16] = "255.255.255.0";
    char mdnsHost[32] = "auraforge50x";

    // Bluetooth & System Settings
    char btDeviceName[32] = "AuraForge-50X-Audio";
    bool nightMode = false;
    uint16_t autoStandbyMinutes = 15;
    char adminPassword[32] = "admin";
    uint32_t bootCount = 0;

    // Advanced Radar, Sensing & Themes
    bool csiStreamingEnabled = false;
    uint8_t activeTheme = 0;           // 0: Cyber Cyan, 1: Solar Flare, 2: Matrix Neon, 3: Vaporwave, 4: Deep Space
    uint8_t powerProfile = 0;          // 0: High Perf (240MHz), 1: Balanced (160MHz), 2: Eco (80MHz)
};

class ConfigManager {
public:
    ConfigManager();
    bool begin();
    void load();
    void save();
    void resetToDefaults();

    SystemConfig& getConfig() { return config; }
    
    // JSON Serialization for Web UI & REST API
    String toJsonString();
    bool updateFromJson(const JsonDocument& doc);

private:
    Preferences preferences;
    SystemConfig config;
};

extern ConfigManager g_configManager;

#endif // CONFIG_MANAGER_H
