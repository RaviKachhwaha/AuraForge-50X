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
#define NUM_USER_PRESETS 4

struct UserPreset {
    char name[24] = "Custom Slot";
    float eqGains[NUM_EQ_BANDS] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    bool isSet = false;
};

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

    // Dynamic Range Compressor (DRC) Settings
    float drcThreshold = -18.0f;       // -40dB to 0dB
    float drcRatio = 4.0f;             // 1.0 to 20.0

    // Closed-Loop Thermal & Digital Governor
    bool thermalGovernorEnabled = true;
    float thermalThrottlingTemp = 75.0f; // °C trigger threshold
    float thermalCriticalTemp = 85.0f;   // °C critical shutdown/attenuation

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

    // Wi-Fi CSI Motion Sensing & Spatial Presence State Machine
    bool csiStreamingEnabled = false;
    char csiPcIp[32] = "192.168.4.2";  // Target PC IP for RuView UDP stream
    uint16_t csiPcPort = 5000;         // Target UDP Port for RuView
    uint8_t csiStreamRateHz = 50;      // UDP Stream Rate (10, 25, 50, 100 Hz)
    bool csiPresenceAutomation = false;
    float csiSensitivity = 5.0f;       // Variance sensitivity threshold
    uint16_t csiTimeoutSeconds = 120;  // Auto-vacant cooldown seconds
    bool csiAutoMute = false;          // Auto-mute when vacant
    bool csiAutoPlay = false;          // Auto-unmute on occupancy

    // Themes & Hardware Power
    uint8_t activeTheme = 0;           // 0..7 Cyberpunk Themes
    uint8_t powerProfile = 0;          // 0: High Perf (240MHz), 1: Balanced (160MHz), 2: Eco (80MHz)

    // User Custom Preset Slots
    UserPreset userPresets[NUM_USER_PRESETS];
};

class ConfigManager {
public:
    ConfigManager();
    bool begin();
    void load();
    void save();
    void resetToDefaults();

    SystemConfig& getConfig() { return config; }
    const SystemConfig& getConfig() const { return config; }
    
    // NVS User Preset Management
    bool saveUserPreset(uint8_t slot, const char* name, const float* gains);
    bool loadUserPreset(uint8_t slot, float* gains);
    String getUserPresetsJson();

    // JSON Serialization for Web UI & REST API
    String toJsonString();
    bool updateFromJson(const JsonDocument& doc);

private:
    Preferences preferences;
    SystemConfig config;
};

extern ConfigManager g_configManager;

#endif // CONFIG_MANAGER_H
