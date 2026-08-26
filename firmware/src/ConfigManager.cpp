/**
 * @file ConfigManager.cpp
 * @brief AuraForge 50X - Non-Volatile Storage (NVS) Configuration Manager Implementation
 * @author Ravi Kachhwaha
 */

#include "ConfigManager.h"

ConfigManager g_configManager;

ConfigManager::ConfigManager() {}

bool ConfigManager::begin() {
    if (!preferences.begin("auraforge", false)) {
        Serial.println("[NVS] Error initializing Preferences!");
        return false;
    }
    if (!preferences.isKey("init")) {
        // First boot: write factory defaults to populate NVS cleanly
        save();
        preferences.putBool("init", true);
    }
    load();
    config.bootCount++;
    preferences.putUInt("bootCount", config.bootCount);
    Serial.printf("[NVS] Configuration Loaded. Boot count: %u\n", config.bootCount);
    return true;
}

void ConfigManager::load() {
    config.volume = preferences.getUChar("volume", 80);
    config.isMuted = preferences.getBool("isMuted", false);

    for (int i = 0; i < NUM_EQ_BANDS; i++) {
        char key[16];
        snprintf(key, sizeof(key), "eq_%d", i);
        config.eqGains[i] = preferences.getFloat(key, 0.0f);
    }

    config.preAmpGain = preferences.getFloat("preAmpGain", 0.0f);
    config.balance = preferences.getFloat("balance", 0.0f);
    config.monoMode = preferences.getBool("monoMode", false);
    config.phaseInvertL = preferences.getBool("phaseInvertL", false);
    config.phaseInvertR = preferences.getBool("phaseInvertR", false);
    config.dynamicBassGain = preferences.getFloat("dynBassGain", 3.0f);
    config.dynamicBassFreq = preferences.getFloat("dynBassFreq", 80.0f);
    config.dynamicBassEnabled = preferences.getBool("dynBassEn", true);
    config.stereoWidth = preferences.getFloat("stereoWidth", 1.0f);
    config.highPassCutoff = preferences.getFloat("hpCutoff", 20.0f);
    config.subLowPassCutoff = preferences.getFloat("subLpCutoff", 150.0f);
    config.loudnessCompensation = preferences.getBool("loudness", false);

    preferences.getString("staSsid", config.wifiStaSsid, sizeof(config.wifiStaSsid));
    preferences.getString("staPass", config.wifiStaPass, sizeof(config.wifiStaPass));
    preferences.getString("apSsid", config.wifiApSsid, sizeof(config.wifiApSsid));
    preferences.getString("apPass", config.wifiApPass, sizeof(config.wifiApPass));
    config.wifiApEnabled = preferences.getBool("apEnabled", true);

    config.useStaticIp = preferences.getBool("useStaticIp", false);
    preferences.getString("staticIp", config.staticIp, sizeof(config.staticIp));
    preferences.getString("staticGw", config.staticGateway, sizeof(config.staticGateway));
    preferences.getString("staticSub", config.staticSubnet, sizeof(config.staticSubnet));
    preferences.getString("mdnsHost", config.mdnsHost, sizeof(config.mdnsHost));

    preferences.getString("btName", config.btDeviceName, sizeof(config.btDeviceName));
    config.nightMode = preferences.getBool("nightMode", false);
    config.autoStandbyMinutes = preferences.getUShort("autoStandby", 15);
    preferences.getString("adminPass", config.adminPassword, sizeof(config.adminPassword));
    config.bootCount = preferences.getUInt("bootCount", 0);
    config.csiStreamingEnabled = preferences.getBool("csiEn", false);
    config.activeTheme = preferences.getUChar("theme", 0);
    config.powerProfile = preferences.getUChar("pwrProf", 0);
}

void ConfigManager::save() {
    preferences.putUChar("volume", config.volume);
    preferences.putBool("isMuted", config.isMuted);

    for (int i = 0; i < NUM_EQ_BANDS; i++) {
        char key[16];
        snprintf(key, sizeof(key), "eq_%d", i);
        preferences.putFloat(key, config.eqGains[i]);
    }

    preferences.putFloat("preAmpGain", config.preAmpGain);
    preferences.putFloat("balance", config.balance);
    preferences.putBool("monoMode", config.monoMode);
    preferences.putBool("phaseInvertL", config.phaseInvertL);
    preferences.putBool("phaseInvertR", config.phaseInvertR);
    preferences.putFloat("dynBassGain", config.dynamicBassGain);
    preferences.putFloat("dynBassFreq", config.dynamicBassFreq);
    preferences.putBool("dynBassEn", config.dynamicBassEnabled);
    preferences.putFloat("stereoWidth", config.stereoWidth);
    preferences.putFloat("hpCutoff", config.highPassCutoff);
    preferences.putFloat("subLpCutoff", config.subLowPassCutoff);
    preferences.putBool("loudness", config.loudnessCompensation);

    preferences.putString("staSsid", config.wifiStaSsid);
    preferences.putString("staPass", config.wifiStaPass);
    preferences.putString("apSsid", config.wifiApSsid);
    preferences.putString("apPass", config.wifiApPass);
    preferences.putBool("apEnabled", config.wifiApEnabled);

    preferences.putBool("useStaticIp", config.useStaticIp);
    preferences.putString("staticIp", config.staticIp);
    preferences.putString("staticGw", config.staticGateway);
    preferences.putString("staticSub", config.staticSubnet);
    preferences.putString("mdnsHost", config.mdnsHost);

    preferences.putString("btName", config.btDeviceName);
    preferences.putBool("nightMode", config.nightMode);
    preferences.putUShort("autoStandby", config.autoStandbyMinutes);
    preferences.putString("adminPass", config.adminPassword);
    preferences.putBool("csiEn", config.csiStreamingEnabled);
    preferences.putUChar("theme", config.activeTheme);
    preferences.putUChar("pwrProf", config.powerProfile);

    Serial.println("[NVS] All settings saved to non-volatile flash memory.");
}

void ConfigManager::resetToDefaults() {
    preferences.clear();
    config = SystemConfig();
    save();
    Serial.println("[NVS] Reset all settings to factory default.");
}

String ConfigManager::toJsonString() {
    StaticJsonDocument<1536> doc;

    doc["volume"] = config.volume;
    doc["isMuted"] = config.isMuted;

    JsonArray eqArray = doc.createNestedArray("eqGains");
    for (int i = 0; i < NUM_EQ_BANDS; i++) {
        eqArray.add(config.eqGains[i]);
    }

    doc["preAmpGain"] = config.preAmpGain;
    doc["balance"] = config.balance;
    doc["monoMode"] = config.monoMode;
    doc["phaseInvertL"] = config.phaseInvertL;
    doc["phaseInvertR"] = config.phaseInvertR;
    doc["dynamicBassGain"] = config.dynamicBassGain;
    doc["dynamicBassFreq"] = config.dynamicBassFreq;
    doc["dynamicBassEnabled"] = config.dynamicBassEnabled;
    doc["stereoWidth"] = config.stereoWidth;
    doc["highPassCutoff"] = config.highPassCutoff;
    doc["subLowPassCutoff"] = config.subLowPassCutoff;
    doc["loudnessCompensation"] = config.loudnessCompensation;

    doc["wifiStaSsid"] = config.wifiStaSsid;
    doc["wifiApSsid"] = config.wifiApSsid;
    doc["wifiApEnabled"] = config.wifiApEnabled;
    doc["useStaticIp"] = config.useStaticIp;
    doc["staticIp"] = config.staticIp;
    doc["mdnsHost"] = config.mdnsHost;
    doc["btDeviceName"] = config.btDeviceName;
    doc["nightMode"] = config.nightMode;
    doc["autoStandbyMinutes"] = config.autoStandbyMinutes;
    doc["bootCount"] = config.bootCount;
    doc["csiStreamingEnabled"] = config.csiStreamingEnabled;
    doc["activeTheme"] = config.activeTheme;
    doc["powerProfile"] = config.powerProfile;

    String jsonStr;
    serializeJson(doc, jsonStr);
    return jsonStr;
}

bool ConfigManager::updateFromJson(const JsonDocument& doc) {
    bool changed = false;

    if (doc.containsKey("volume")) { config.volume = doc["volume"]; changed = true; }
    if (doc.containsKey("isMuted")) { config.isMuted = doc["isMuted"]; changed = true; }

    if (doc.containsKey("eqGains")) {
        JsonArrayConst arr = doc["eqGains"].as<JsonArrayConst>();
        for (int i = 0; i < NUM_EQ_BANDS && i < (int)arr.size(); i++) {
            config.eqGains[i] = arr[i];
        }
        changed = true;
    }

    if (doc.containsKey("preAmpGain")) { config.preAmpGain = doc["preAmpGain"]; changed = true; }
    if (doc.containsKey("balance")) { config.balance = doc["balance"]; changed = true; }
    if (doc.containsKey("monoMode")) { config.monoMode = doc["monoMode"]; changed = true; }
    if (doc.containsKey("phaseInvertL")) { config.phaseInvertL = doc["phaseInvertL"]; changed = true; }
    if (doc.containsKey("phaseInvertR")) { config.phaseInvertR = doc["phaseInvertR"]; changed = true; }
    if (doc.containsKey("dynamicBassGain")) { config.dynamicBassGain = doc["dynamicBassGain"]; changed = true; }
    if (doc.containsKey("dynamicBassFreq")) { config.dynamicBassFreq = doc["dynamicBassFreq"]; changed = true; }
    if (doc.containsKey("dynamicBassEnabled")) { config.dynamicBassEnabled = doc["dynamicBassEnabled"]; changed = true; }
    if (doc.containsKey("stereoWidth")) { config.stereoWidth = doc["stereoWidth"]; changed = true; }
    if (doc.containsKey("highPassCutoff")) { config.highPassCutoff = doc["highPassCutoff"]; changed = true; }
    if (doc.containsKey("subLowPassCutoff")) { config.subLowPassCutoff = doc["subLowPassCutoff"]; changed = true; }
    if (doc.containsKey("loudnessCompensation")) { config.loudnessCompensation = doc["loudnessCompensation"]; changed = true; }

    if (doc.containsKey("wifiStaSsid")) { strncpy(config.wifiStaSsid, doc["wifiStaSsid"] | "", sizeof(config.wifiStaSsid)); changed = true; }
    if (doc.containsKey("wifiStaPass")) { strncpy(config.wifiStaPass, doc["wifiStaPass"] | "", sizeof(config.wifiStaPass)); changed = true; }
    if (doc.containsKey("wifiApSsid")) { strncpy(config.wifiApSsid, doc["wifiApSsid"] | "AuraForge-50X-AP", sizeof(config.wifiApSsid)); changed = true; }
    if (doc.containsKey("wifiApPass")) { strncpy(config.wifiApPass, doc["wifiApPass"] | "auraforge123", sizeof(config.wifiApPass)); changed = true; }
    if (doc.containsKey("wifiApEnabled")) { config.wifiApEnabled = doc["wifiApEnabled"]; changed = true; }
    if (doc.containsKey("btDeviceName")) { strncpy(config.btDeviceName, doc["btDeviceName"] | "AuraForge-50X-Audio", sizeof(config.btDeviceName)); changed = true; }

    if (doc.containsKey("csiStreamingEnabled")) { config.csiStreamingEnabled = doc["csiStreamingEnabled"]; changed = true; }
    if (doc.containsKey("activeTheme")) { config.activeTheme = doc["activeTheme"]; changed = true; }
    if (doc.containsKey("powerProfile")) { config.powerProfile = doc["powerProfile"]; changed = true; }

    if (changed) {
        save();
    }
    return changed;
}
