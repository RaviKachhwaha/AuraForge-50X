/**
 * @file HardwareMonitor.cpp
 * @brief AuraForge 50X - Amplifier Fault Protection & Power Telemetry Monitor Implementation
 * @author Ravi Kachhwaha
 */

#include "HardwareMonitor.h"
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

    Serial.println("[Hardware Monitor] Power & Fault Diagnostics Subsystem Initialized.");
}

void HardwareMonitor::setAmplifierState(bool enable) {
    if (enable) {
        if (digitalRead(PIN_AMP_FAULT) == LOW) {
            Serial.println("[HARDWARE PROTECTION] Cannot enable TPA3116D2 - Active FAULT line low!");
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

void HardwareMonitor::setLedMode(LedMode mode) {
    currentLedMode = mode;
}

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

void HardwareMonitor::update() {
    handleLedPatterns();
    updateTelemetry();

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
                Serial.printf("[HARDWARE FAULT DETECTED] TPA3116D2 FAULT pin pulled LOW! (Count: %u)\n", telemetry.faultCount);
            }
        } else {
            if (telemetry.ampFaultDetected) {
                // Self-healing attempt after fault clears
                telemetry.ampFaultDetected = false;
                Serial.println("[HARDWARE RECOVERY] TPA3116D2 FAULT condition cleared.");
                setAmplifierState(true);
                setLedMode(LED_ON);
            }
        }
    }
}

String HardwareMonitor::getTelemetryJson() {
    StaticJsonDocument<512> doc;

    doc["ampEnabled"] = telemetry.ampEnabled;
    doc["ampFaultDetected"] = telemetry.ampFaultDetected;
    doc["faultCount"] = telemetry.faultCount;
    doc["batteryVoltage"] = telemetry.estimatedBatteryVoltage;
    doc["batteryPercent"] = telemetry.estimatedBatteryPercent;
    doc["uptimeSeconds"] = telemetry.uptimeSeconds;
    doc["freeHeap"] = telemetry.freeHeapBytes;
    doc["cpuFreqMHz"] = telemetry.cpuFreqMHz;
    doc["chipRevision"] = telemetry.chipRevision;

    String output;
    serializeJson(doc, output);
    return output;
}
