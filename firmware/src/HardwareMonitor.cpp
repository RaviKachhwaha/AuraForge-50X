/**
 * @file HardwareMonitor.cpp
 * @brief AuraForge 50X - Closed-Loop Thermal & Digital Governor, Amplifier
 * Fault Protection & Power Monitor Implementation
 * @author Ravi Kachhwaha
 */

#include "HardwareMonitor.h"
#include "ConfigManager.h"
#include "DspEngine.h"
#include <ArduinoJson.h>

HardwareMonitor g_hardwareMonitor;

HardwareMonitor::HardwareMonitor() {}

void HardwareMonitor::begin() {
  pinMode(PIN_AMP_SDZ, OUTPUT);
  pinMode(PIN_STATUS_LED, OUTPUT);
  pinMode(PIN_AMP_FAULT, INPUT_PULLUP);

  // Initial state: Start muted to prevent startup pop transients
  digitalWrite(PIN_AMP_SDZ, LOW);
  digitalWrite(PIN_STATUS_LED, LOW);
  telemetry.ampEnabled = false;

  telemetry.cpuFreqMHz = ESP.getCpuFreqMHz();
  telemetry.chipRevision = ESP.getChipRevision();

  // Initial silicon temperature reading
  float rawTemp = temperatureRead();
  if (rawTemp > 0.0f && rawTemp < 130.0f) {
    telemetry.temperatureC = rawTemp;
    telemetry.temperatureF = rawTemp * 1.8f + 32.0f;
  }

  Serial.printf("[Hardware Monitor] Subsystem initialized. Silicon Temp: "
                "%.1f°C | CPU: %u MHz\n",
                telemetry.temperatureC, telemetry.cpuFreqMHz);
}

void HardwareMonitor::setAmplifierState(bool enable) {
  if (enable) {
    if (digitalRead(PIN_AMP_FAULT) == LOW) {
      Serial.println("[HARDWARE PROTECTION] Cannot enable TPA3116D2 - Active "
                     "FAULT line low!");
      telemetry.ampFaultDetected = true;
      setLedMode(LED_FAST_BLINK);
      return;
    }
    delay(100); // 100ms Anti-pop delay allowing TPS61088 21V rail to charge
    digitalWrite(PIN_AMP_SDZ, HIGH);
    telemetry.ampEnabled = true;
    telemetry.ampFaultDetected = false;
    Serial.println("[AuraForge 50X] TPA3116D2 Amplifier Output Unmuted.");
  } else {
    digitalWrite(PIN_AMP_SDZ, LOW);
    telemetry.ampEnabled = false;
    Serial.println("[AuraForge 50X] TPA3116D2 Amplifier Output Muted.");
  }
}

void HardwareMonitor::setLedMode(LedMode mode) { currentLedMode = mode; }

void HardwareMonitor::handleLedPatterns() {
  unsigned long now = millis();
  uint32_t interval = 500;

  switch (currentLedMode) {
  case LED_OFF:
    digitalWrite(PIN_STATUS_LED, LOW);
    return;
  case LED_ON:
    digitalWrite(PIN_STATUS_LED, HIGH);
    return;
  case LED_SLOW_BLINK:
    interval = 500;
    break;
  case LED_FAST_BLINK:
    interval = 100;
    break;
  case LED_HEARTBEAT:
    interval = 1000;
    break;
  }

  if (now - lastLedToggle >= interval) {
    lastLedToggle = now;
    ledState = !ledState;
    digitalWrite(PIN_STATUS_LED, ledState ? HIGH : LOW);
  }
}

void HardwareMonitor::updateTelemetry() {
  unsigned long now = millis();
  if (now - lastUptimeTick >= 1000) {
    lastUptimeTick = now;
    telemetry.uptimeSeconds++;
    telemetry.freeHeapBytes = ESP.getFreeHeap();
  }
}

void HardwareMonitor::processThermalGovernor() {
  unsigned long now = millis();
  if (now - tempCheckTimer < 250)
    return; // 4 Hz Thermal Governor Execution Loop
  tempCheckTimer = now;

  // Read on-chip silicon junction temperature
  float rawTemp = temperatureRead();
  if (rawTemp > 0.0f && rawTemp < 130.0f) {
    // Exponential Moving Average filter (alpha = 0.15) for smooth thermal
    // telemetry
    telemetry.temperatureC = telemetry.temperatureC * 0.85f + rawTemp * 0.15f;
    telemetry.temperatureF = telemetry.temperatureC * 1.8f + 32.0f;
  }

  const SystemConfig &cfg = g_configManager.getConfig();
  telemetry.governorActive = cfg.thermalGovernorEnabled;

  if (!cfg.thermalGovernorEnabled) {
    telemetry.governorState = GOVERNOR_NORMAL;
    telemetry.thermalAttenuation_dB = 0.0f;
    telemetry.thermalGainMultiplier = 1.0f;
    g_dspEngine.setThermalGovernorGain(1.0f);
    return;
  }

  float t = telemetry.temperatureC;
  float throtLimit = cfg.thermalThrottlingTemp; // e.g. 75.0°C
  float critLimit = cfg.thermalCriticalTemp;    // e.g. 85.0°C

  if (t >= critLimit) {
    // Critical Tier: -6.0 dB digital attenuation (0.501 multiplier)
    telemetry.governorState = GOVERNOR_CRITICAL;
    telemetry.thermalAttenuation_dB = -6.0f;
    telemetry.thermalGainMultiplier = 0.501f;
    setLedMode(LED_FAST_BLINK);
  } else if (t >= throtLimit) {
    // Throttled Tier: -3.0 dB digital foldback (0.708 multiplier)
    telemetry.governorState = GOVERNOR_THROTTLED;
    telemetry.thermalAttenuation_dB = -3.0f;
    telemetry.thermalGainMultiplier = 0.708f;
  } else if (t >= (throtLimit - 10.0f)) {
    // Warm Warning Tier: -1.0 dB soft limiter reduction (0.891 multiplier)
    telemetry.governorState = GOVERNOR_WARM;
    telemetry.thermalAttenuation_dB = -1.0f;
    telemetry.thermalGainMultiplier = 0.891f;
  } else {
    // Normal Safe Tier: Full Dynamics (1.000 multiplier, 0 dB attenuation)
    telemetry.governorState = GOVERNOR_NORMAL;
    telemetry.thermalAttenuation_dB = 0.0f;
    telemetry.thermalGainMultiplier = 1.0f;
  }

  // Closed-loop active link directly feeding DSP engine
  g_dspEngine.setThermalGovernorGain(telemetry.thermalGainMultiplier);
}

void HardwareMonitor::update() {
  handleLedPatterns();
  updateTelemetry();
  processThermalGovernor();

  // Check Hardware Fault Line (Active Low)
  if (millis() - faultCheckTimer >= 200) {
    faultCheckTimer = millis();

    if (digitalRead(PIN_AMP_FAULT) == LOW) {
      if (!telemetry.ampFaultDetected) {
        telemetry.ampFaultDetected = true;
        telemetry.faultCount++;
        digitalWrite(PIN_AMP_SDZ, LOW); // Mute instantly to protect speakers
        telemetry.ampEnabled = false;
        setLedMode(LED_FAST_BLINK);
        Serial.printf("[HARDWARE FAULT DETECTED] TPA3116D2 FAULT pin pulled "
                      "LOW! (Count: %u)\n",
                      telemetry.faultCount);
      }
    } else {
      if (telemetry.ampFaultDetected) {
        // Self-healing attempt after fault clears
        telemetry.ampFaultDetected = false;
        Serial.println(
            "[HARDWARE RECOVERY] TPA3116D2 FAULT condition cleared.");
        setAmplifierState(true);
        setLedMode(LED_ON);
      }
    }
  }
}

String HardwareMonitor::getTelemetryJson() {
  StaticJsonDocument<768> doc;

  doc["ampEnabled"] = telemetry.ampEnabled;
  doc["ampFaultDetected"] = telemetry.ampFaultDetected;
  doc["faultCount"] = telemetry.faultCount;
  doc["batteryVoltage"] = telemetry.estimatedBatteryVoltage;
  doc["batteryPercent"] = telemetry.estimatedBatteryPercent;
  doc["uptimeSeconds"] = telemetry.uptimeSeconds;
  doc["freeHeap"] = telemetry.freeHeapBytes;
  doc["cpuFreqMHz"] = telemetry.cpuFreqMHz;
  doc["chipRevision"] = telemetry.chipRevision;

  // Thermal Governor Telemetry
  doc["temperatureC"] = telemetry.temperatureC;
  doc["temperatureF"] = telemetry.temperatureF;
  doc["governorState"] = (int)telemetry.governorState;
  doc["thermalAttenuation_dB"] = telemetry.thermalAttenuation_dB;
  doc["thermalGainMultiplier"] = telemetry.thermalGainMultiplier;
  doc["governorActive"] = telemetry.governorActive;

  String output;
  serializeJson(doc, output);
  return output;
}
