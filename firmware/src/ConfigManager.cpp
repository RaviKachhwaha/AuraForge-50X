/**
 * @file ConfigManager.cpp
 * @brief AuraForge 50X - Non-Volatile Storage (NVS) Configuration Manager
 * Implementation
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
  Serial.printf("[NVS] Configuration Loaded. Boot count: %u\n",
                config.bootCount);
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

  config.drcThreshold = preferences.getFloat("drcThresh", -18.0f);
  config.drcRatio = preferences.getFloat("drcRatio", 4.0f);

  config.thermalGovernorEnabled = preferences.getBool("thermGovEn", true);
  config.thermalThrottlingTemp = preferences.getFloat("thermThrot", 75.0f);
  config.thermalCriticalTemp = preferences.getFloat("thermCrit", 85.0f);

  preferences.getString("staSsid", config.wifiStaSsid,
                        sizeof(config.wifiStaSsid));
  preferences.getString("staPass", config.wifiStaPass,
                        sizeof(config.wifiStaPass));
  preferences.getString("apSsid", config.wifiApSsid, sizeof(config.wifiApSsid));
  preferences.getString("apPass", config.wifiApPass, sizeof(config.wifiApPass));
  config.wifiApEnabled = preferences.getBool("apEnabled", true);

  config.useStaticIp = preferences.getBool("useStaticIp", false);
  preferences.getString("staticIp", config.staticIp, sizeof(config.staticIp));
  preferences.getString("staticGw", config.staticGateway,
                        sizeof(config.staticGateway));
  preferences.getString("staticSub", config.staticSubnet,
                        sizeof(config.staticSubnet));
  preferences.getString("mdnsHost", config.mdnsHost, sizeof(config.mdnsHost));

  preferences.getString("btName", config.btDeviceName,
                        sizeof(config.btDeviceName));
  config.nightMode = preferences.getBool("nightMode", false);
  config.autoStandbyMinutes = preferences.getUShort("autoStandby", 15);
  preferences.getString("adminPass", config.adminPassword,
                        sizeof(config.adminPassword));
  config.bootCount = preferences.getUInt("bootCount", 0);

  config.csiStreamingEnabled = preferences.getBool("csiEn", false);
  preferences.getString("csiIp", config.csiPcIp, sizeof(config.csiPcIp));
  config.csiPcPort = preferences.getUShort("csiPort", 5000);
  config.csiStreamRateHz = preferences.getUChar("csiRate", 50);
  config.csiPresenceAutomation = preferences.getBool("csiAutoEn", false);
  config.csiSensitivity = preferences.getFloat("csiSens", 5.0f);
  config.csiTimeoutSeconds = preferences.getUShort("csiTimeout", 120);
  config.csiAutoMute = preferences.getBool("csiAutoMute", false);
  config.csiAutoPlay = preferences.getBool("csiAutoPlay", false);

  config.activeTheme = preferences.getUChar("theme", 0);
  config.powerProfile = preferences.getUChar("pwrProf", 0);

  // Load User Custom Presets from NVS
  for (uint8_t slot = 0; slot < NUM_USER_PRESETS; slot++) {
    char keySet[16], keyName[16];
    snprintf(keySet, sizeof(keySet), "uP%u_set", slot);
    snprintf(keyName, sizeof(keyName), "uP%u_name", slot);
    config.userPresets[slot].isSet = preferences.getBool(keySet, false);
    preferences.getString(keyName, config.userPresets[slot].name,
                          sizeof(config.userPresets[slot].name));
    for (int b = 0; b < NUM_EQ_BANDS; b++) {
      char keyGain[16];
      snprintf(keyGain, sizeof(keyGain), "uP%u_b%d", slot, b);
      config.userPresets[slot].eqGains[b] = preferences.getFloat(keyGain, 0.0f);
    }
  }
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

  preferences.putFloat("drcThresh", config.drcThreshold);
  preferences.putFloat("drcRatio", config.drcRatio);

  preferences.putBool("thermGovEn", config.thermalGovernorEnabled);
  preferences.putFloat("thermThrot", config.thermalThrottlingTemp);
  preferences.putFloat("thermCrit", config.thermalCriticalTemp);

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
  preferences.putString("csiIp", config.csiPcIp);
  preferences.putUShort("csiPort", config.csiPcPort);
  preferences.putUChar("csiRate", config.csiStreamRateHz);
  preferences.putBool("csiAutoEn", config.csiPresenceAutomation);
  preferences.putFloat("csiSens", config.csiSensitivity);
  preferences.putUShort("csiTimeout", config.csiTimeoutSeconds);
  preferences.putBool("csiAutoMute", config.csiAutoMute);
  preferences.putBool("csiAutoPlay", config.csiAutoPlay);

  preferences.putUChar("theme", config.activeTheme);
  preferences.putUChar("pwrProf", config.powerProfile);

  // Save User Presets
  for (uint8_t slot = 0; slot < NUM_USER_PRESETS; slot++) {
    char keySet[16], keyName[16];
    snprintf(keySet, sizeof(keySet), "uP%u_set", slot);
    snprintf(keyName, sizeof(keyName), "uP%u_name", slot);
    preferences.putBool(keySet, config.userPresets[slot].isSet);
    preferences.putString(keyName, config.userPresets[slot].name);
    for (int b = 0; b < NUM_EQ_BANDS; b++) {
      char keyGain[16];
      snprintf(keyGain, sizeof(keyGain), "uP%u_b%d", slot, b);
      preferences.putFloat(keyGain, config.userPresets[slot].eqGains[b]);
    }
  }

  Serial.println("[NVS] All settings and user presets saved to non-volatile "
                 "flash memory.");
}

bool ConfigManager::saveUserPreset(uint8_t slot, const char *name,
                                   const float *gains) {
  if (slot >= NUM_USER_PRESETS)
    return false;

  if (name && strlen(name) > 0) {
    strncpy(config.userPresets[slot].name, name,
            sizeof(config.userPresets[slot].name) - 1);
    config.userPresets[slot].name[sizeof(config.userPresets[slot].name) - 1] =
        '\0';
  }
  if (gains) {
    for (int i = 0; i < NUM_EQ_BANDS; i++) {
      config.userPresets[slot].eqGains[i] = gains[i];
    }
  }
  config.userPresets[slot].isSet = true;
  save();
  return true;
}

bool ConfigManager::loadUserPreset(uint8_t slot, float *gains) {
  if (slot >= NUM_USER_PRESETS || !config.userPresets[slot].isSet)
    return false;
  for (int i = 0; i < NUM_EQ_BANDS; i++) {
    config.eqGains[i] = config.userPresets[slot].eqGains[i];
    if (gains)
      gains[i] = config.userPresets[slot].eqGains[i];
  }
  save();
  return true;
}

String ConfigManager::getUserPresetsJson() {
  StaticJsonDocument<1024> doc;
  JsonArray arr = doc.createNestedArray("presets");
  for (uint8_t i = 0; i < NUM_USER_PRESETS; i++) {
    JsonObject obj = arr.createNestedObject();
    obj["slot"] = i;
    obj["name"] = config.userPresets[i].name;
    obj["isSet"] = config.userPresets[i].isSet;
    JsonArray g = obj.createNestedArray("eqGains");
    for (int b = 0; b < NUM_EQ_BANDS; b++) {
      g.add(config.userPresets[i].eqGains[b]);
    }
  }
  String out;
  serializeJson(doc, out);
  return out;
}

void ConfigManager::resetToDefaults() {
  preferences.clear();
  config = SystemConfig();
  save();
  Serial.println("[NVS] Reset all settings to factory default.");
}

String ConfigManager::toJsonString() {
  StaticJsonDocument<2048> doc;

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

  doc["drcThreshold"] = config.drcThreshold;
  doc["drcRatio"] = config.drcRatio;

  doc["thermalGovernorEnabled"] = config.thermalGovernorEnabled;
  doc["thermalThrottlingTemp"] = config.thermalThrottlingTemp;
  doc["thermalCriticalTemp"] = config.thermalCriticalTemp;

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
  doc["csiPcIp"] = config.csiPcIp;
  doc["csiPcPort"] = config.csiPcPort;
  doc["csiStreamRateHz"] = config.csiStreamRateHz;
  doc["csiPresenceAutomation"] = config.csiPresenceAutomation;
  doc["csiSensitivity"] = config.csiSensitivity;
  doc["csiTimeoutSeconds"] = config.csiTimeoutSeconds;
  doc["csiAutoMute"] = config.csiAutoMute;
  doc["csiAutoPlay"] = config.csiAutoPlay;

  doc["activeTheme"] = config.activeTheme;
  doc["powerProfile"] = config.powerProfile;

  String jsonStr;
  serializeJson(doc, jsonStr);
  return jsonStr;
}

bool ConfigManager::updateFromJson(const JsonDocument &doc) {
  bool changed = false;

  if (doc.containsKey("volume")) {
    config.volume = doc["volume"];
    changed = true;
  }
  if (doc.containsKey("isMuted")) {
    config.isMuted = doc["isMuted"];
    changed = true;
  }

  if (doc.containsKey("eqGains")) {
    JsonArrayConst arr = doc["eqGains"].as<JsonArrayConst>();
    for (int i = 0; i < NUM_EQ_BANDS && i < (int)arr.size(); i++) {
      config.eqGains[i] = arr[i];
    }
    changed = true;
  }

  if (doc.containsKey("preAmpGain")) {
    config.preAmpGain = doc["preAmpGain"];
    changed = true;
  }
  if (doc.containsKey("balance")) {
    config.balance = doc["balance"];
    changed = true;
  }
  if (doc.containsKey("monoMode")) {
    config.monoMode = doc["monoMode"];
    changed = true;
  }
  if (doc.containsKey("phaseInvertL")) {
    config.phaseInvertL = doc["phaseInvertL"];
    changed = true;
  }
  if (doc.containsKey("phaseInvertR")) {
    config.phaseInvertR = doc["phaseInvertR"];
    changed = true;
  }
  if (doc.containsKey("dynamicBassGain")) {
    config.dynamicBassGain = doc["dynamicBassGain"];
    changed = true;
  }
  if (doc.containsKey("dynamicBassFreq")) {
    config.dynamicBassFreq = doc["dynamicBassFreq"];
    changed = true;
  }
  if (doc.containsKey("dynamicBassEnabled")) {
    config.dynamicBassEnabled = doc["dynamicBassEnabled"];
    changed = true;
  }
  if (doc.containsKey("stereoWidth")) {
    config.stereoWidth = doc["stereoWidth"];
    changed = true;
  }
  if (doc.containsKey("highPassCutoff")) {
    config.highPassCutoff = doc["highPassCutoff"];
    changed = true;
  }
  if (doc.containsKey("subLowPassCutoff")) {
    config.subLowPassCutoff = doc["subLowPassCutoff"];
    changed = true;
  }
  if (doc.containsKey("loudnessCompensation")) {
    config.loudnessCompensation = doc["loudnessCompensation"];
    changed = true;
  }

  if (doc.containsKey("drcThreshold")) {
    config.drcThreshold = doc["drcThreshold"];
    changed = true;
  }
  if (doc.containsKey("drcRatio")) {
    config.drcRatio = doc["drcRatio"];
    changed = true;
  }

  if (doc.containsKey("thermalGovernorEnabled")) {
    config.thermalGovernorEnabled = doc["thermalGovernorEnabled"];
    changed = true;
  }
  if (doc.containsKey("thermalThrottlingTemp")) {
    config.thermalThrottlingTemp = doc["thermalThrottlingTemp"];
    changed = true;
  }
  if (doc.containsKey("thermalCriticalTemp")) {
    config.thermalCriticalTemp = doc["thermalCriticalTemp"];
    changed = true;
  }

  if (doc.containsKey("wifiStaSsid")) {
    strncpy(config.wifiStaSsid, doc["wifiStaSsid"] | "",
            sizeof(config.wifiStaSsid));
    changed = true;
  }
  if (doc.containsKey("wifiStaPass")) {
    strncpy(config.wifiStaPass, doc["wifiStaPass"] | "",
            sizeof(config.wifiStaPass));
    changed = true;
  }
  if (doc.containsKey("wifiApSsid")) {
    strncpy(config.wifiApSsid, doc["wifiApSsid"] | "AuraForge-50X-AP",
            sizeof(config.wifiApSsid));
    changed = true;
  }
  if (doc.containsKey("wifiApPass")) {
    strncpy(config.wifiApPass, doc["wifiApPass"] | "auraforge123",
            sizeof(config.wifiApPass));
    changed = true;
  }
  if (doc.containsKey("wifiApEnabled")) {
    config.wifiApEnabled = doc["wifiApEnabled"];
    changed = true;
  }
  if (doc.containsKey("btDeviceName")) {
    strncpy(config.btDeviceName, doc["btDeviceName"] | "AuraForge-50X-Audio",
            sizeof(config.btDeviceName));
    changed = true;
  }

  if (doc.containsKey("csiStreamingEnabled")) {
    config.csiStreamingEnabled = doc["csiStreamingEnabled"];
    changed = true;
  }
  if (doc.containsKey("csiPcIp")) {
    strncpy(config.csiPcIp, doc["csiPcIp"] | "192.168.4.2",
            sizeof(config.csiPcIp));
    changed = true;
  }
  if (doc.containsKey("csiPcPort")) {
    config.csiPcPort = doc["csiPcPort"];
    changed = true;
  }
  if (doc.containsKey("csiStreamRateHz")) {
    config.csiStreamRateHz = doc["csiStreamRateHz"];
    changed = true;
  }
  if (doc.containsKey("csiPresenceAutomation")) {
    config.csiPresenceAutomation = doc["csiPresenceAutomation"];
    changed = true;
  }
  if (doc.containsKey("csiSensitivity")) {
    config.csiSensitivity = doc["csiSensitivity"];
    changed = true;
  }
  if (doc.containsKey("csiTimeoutSeconds")) {
    config.csiTimeoutSeconds = doc["csiTimeoutSeconds"];
    changed = true;
  }
  if (doc.containsKey("csiAutoMute")) {
    config.csiAutoMute = doc["csiAutoMute"];
    changed = true;
  }
  if (doc.containsKey("csiAutoPlay")) {
    config.csiAutoPlay = doc["csiAutoPlay"];
    changed = true;
  }

  if (doc.containsKey("activeTheme")) {
    config.activeTheme = doc["activeTheme"];
    changed = true;
  }
  if (doc.containsKey("powerProfile")) {
    config.powerProfile = doc["powerProfile"];
    changed = true;
  }

  if (changed) {
    save();
  }
  return changed;
}
