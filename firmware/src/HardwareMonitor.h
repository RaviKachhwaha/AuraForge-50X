/**
 * @file HardwareMonitor.h
 * @brief AuraForge 50X - Closed-Loop Thermal & Digital Governor, Amplifier Fault Protection & Power Monitor
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

enum ThermalGovernorState {
    GOVERNOR_NORMAL = 0,     // < 65°C: 0.0 dB attenuation (100% gain, full dynamics)
    GOVERNOR_WARM,           // 65°C - 75°C: -1.0 dB soft limiter reduction
    GOVERNOR_THROTTLED,      // 75°C - 85°C: -3.0 dB digital foldback attenuation
    GOVERNOR_CRITICAL        // >= 85°C: -6.0 dB digital attenuation / protective foldback
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

    // Closed-Loop Thermal Governor Telemetry
    float temperatureC = 45.0f;
    float temperatureF = 113.0f;
    ThermalGovernorState governorState = GOVERNOR_NORMAL;
    float thermalAttenuation_dB = 0.0f;
    float thermalGainMultiplier = 1.0f;
    bool governorActive = true;
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

    // Thermal Governor Controls & Queries
    float getTemperatureC() const { return telemetry.temperatureC; }
    ThermalGovernorState getGovernorState() const { return telemetry.governorState; }
    float getThermalGainMultiplier() const { return telemetry.thermalGainMultiplier; }

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
    unsigned long tempCheckTimer = 0;

    void handleLedPatterns();
    void updateTelemetry();
    void processThermalGovernor();
};

extern HardwareMonitor g_hardwareMonitor;

#endif // HARDWARE_MONITOR_H
