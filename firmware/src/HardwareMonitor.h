/**
 * @file HardwareMonitor.h
 * @brief AuraForge 50X - Amplifier Fault Protection & Power Telemetry Monitor
 * @author Ravi Kachhwaha
 */

#ifndef HARDWARE_MONITOR_H
#define HARDWARE_MONITOR_H

#include <Arduino.h>

#define PIN_AMP_SDZ     22   // TPA3116D2 Mute/Shutdown Pin (Active Low)
#define PIN_AMP_FAULT   23   // TPA3116D2 Fault Output (Active Low, Pull-up)
#define PIN_STATUS_LED  2    // Board Activity LED

enum LedMode {
    LED_OFF = 0,
    LED_ON,
    LED_SLOW_BLINK,
    LED_FAST_BLINK,
    LED_HEARTBEAT
};

struct HardwareTelemetry {
    bool ampEnabled = false;
    bool ampFaultDetected = false;
    uint32_t faultCount = 0;
    float estimatedBatteryVoltage = 3.85f;
    uint8_t estimatedBatteryPercent = 85;
    uint32_t uptimeSeconds = 0;
    uint32_t freeHeapBytes = 0;
    uint8_t cpuFreqMHz = 240;
    int chipRevision = 1;
};

class HardwareMonitor {
public:
    HardwareMonitor();
    void begin();
    void update();

    // Amplifier Control
    void setAmplifierState(bool enable);
    bool isAmplifierEnabled() const { return telemetry.ampEnabled; }

    // LED Pattern Controls
    void setLedMode(LedMode mode);

    // Diagnostics Telemetry
    const HardwareTelemetry& getTelemetry() const { return telemetry; }
    String getTelemetryJson();

private:
    HardwareTelemetry telemetry;
    LedMode currentLedMode = LED_SLOW_BLINK;
    unsigned long lastLedToggle = 0;
    bool ledState = false;
    unsigned long lastUptimeTick = 0;
    unsigned long faultCheckTimer = 0;

    void handleLedPatterns();
    void updateTelemetry();
};

extern HardwareMonitor g_hardwareMonitor;

#endif // HARDWARE_MONITOR_H
