/**
 * @file main.cpp
 * @brief AuraForge 50X - Enterprise High-Fidelity Wireless Audio & DSP Station
 * @author Ravi Kachhwaha
 * @license MIT
 */

#include <Arduino.h>
#include <WiFi.h>
#include <driver/i2s.h>
#include "BluetoothA2DPSink.h"
#include "ConfigManager.h"
#include "DspEngine.h"
#include "HardwareMonitor.h"
#include "WifiManagerModule.h"
#include "WebServerModule.h"
#include "CsiRadarEngine.h"

// ==============================================================================
// BLUETOOTH A2DP SINK & AUDIO CALLBACKS
// ==============================================================================
BluetoothA2DPSink *a2dp_sink = nullptr;

void audioDataCallback(const uint8_t *data, uint32_t len) {
    uint32_t sample_count = len / 4; // 2 channels * 16-bit (2 bytes) = 4 bytes per frame
    g_dspEngine.processAudioBlock((int16_t *)data, sample_count);
}

void avrcpStateCallback(esp_a2d_connection_state_t state, void *ptr) {
    if (state == ESP_A2D_CONNECTION_STATE_CONNECTED) {
        Serial.println("[AuraForge 50X] Bluetooth Source Connected!");
        g_hardwareMonitor.setLedMode(LED_ON);
    } else if (state == ESP_A2D_CONNECTION_STATE_DISCONNECTED) {
        Serial.println("[AuraForge 50X] Bluetooth Source Disconnected.");
        g_hardwareMonitor.setLedMode(LED_SLOW_BLINK);
    }
}

// ==============================================================================
// SERIAL CLI COMMAND INTERPRETER
// ==============================================================================
void handleSerialCommands() {
    if (!Serial.available()) return;

    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd.equalsIgnoreCase("status")) {
        Serial.println("--- AuraForge 50X System Diagnostics ---");
        Serial.printf("ESP32 Chip Rev: %d | CPU Freq: %d MHz\n", ESP.getChipRevision(), ESP.getCpuFreqMHz());
        Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
        Serial.printf("Wi-Fi STA Connected: %s | Local IP: %s\n", g_wifiManager.isConnectedSTA() ? "YES" : "NO", g_wifiManager.getLocalIp().c_str());
        Serial.printf("Wi-Fi AP Active: %s | AP IP: %s\n", g_wifiManager.isAPActive() ? "YES" : "NO", g_wifiManager.getApIp().c_str());
        Serial.printf("TPA3116D2 FAULT Pin State: %s\n", digitalRead(PIN_AMP_FAULT) == HIGH ? "NORMAL (OK)" : "FAULT DETECTED");
    } else if (cmd.startsWith("vol ")) {
        uint8_t vol = cmd.substring(4).toInt();
        g_configManager.getConfig().volume = vol;
        g_configManager.save();
        g_dspEngine.updateFromConfig(g_configManager.getConfig());
        Serial.printf("[CLI] Volume set to %u%%\n", vol);
    } else if (cmd.equalsIgnoreCase("reset")) {
        g_configManager.resetToDefaults();
        g_dspEngine.updateFromConfig(g_configManager.getConfig());
        Serial.println("[CLI] Reset configuration to defaults.");
    }
}

// ==============================================================================
// SYSTEM INITIALIZATION & MAIN EXECUTION LOOP
// ==============================================================================
void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("\n==========================================================");
    Serial.println(" AuraForge 50X - Enterprise High-Fidelity Audio Station ");
    Serial.println(" Hardware Platform: 4-Layer KiCad 10 | TPA3116D2 + TPS61088");
    Serial.println("==========================================================\n");

    // 1. Initialize NVS Configuration Store
    g_configManager.begin();

    // 2. Initialize Hardware Protection & Power Monitor
    g_hardwareMonitor.begin();

    // 3. Initialize 10-Band Parametric Equalizer & Audio Engine
    g_dspEngine.begin();
    g_dspEngine.updateFromConfig(g_configManager.getConfig());

    // 4. Initialize Wi-Fi Manager, Captive Portal & mDNS Responder
    g_wifiManager.begin(g_configManager.getConfig());

    // 4b. Initialize Wi-Fi CSI Motion Sensing Radar Engine
    g_csiRadarEngine.begin();

    // 5. Initialize Async HTTP Server & WebSocket Telemetry
    g_webServerModule.begin();

    // 6. Configure I2S Settings for High-Fidelity Output
    static i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = DSP_SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_MSB,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 128,
        .use_apll = false,
        .tx_desc_auto_clear = true
    };

    a2dp_sink = new BluetoothA2DPSink();
    a2dp_sink->set_i2s_config(i2s_config);
    a2dp_sink->set_stream_reader(audioDataCallback, false);
    a2dp_sink->set_on_connection_state_changed(avrcpStateCallback);

    // 7. Start Bluetooth A2DP Sink
    Serial.printf("[AuraForge 50X] Initializing Audio Engine & Bluetooth '%s'...\n", g_configManager.getConfig().btDeviceName);
    if (btStart()) {
        a2dp_sink->start(g_configManager.getConfig().btDeviceName);
        Serial.printf("[AuraForge 50X] Bluetooth Advertising as '%s'...\n", g_configManager.getConfig().btDeviceName);
    }

    // 8. Enable TPA3116D2 Power Output with Anti-Pop Sequencing
    g_hardwareMonitor.setAmplifierState(true);
}

void loop() {
    handleSerialCommands();
    g_hardwareMonitor.update();
    g_wifiManager.update();
    g_webServerModule.update();

    vTaskDelay(pdMS_TO_TICKS(10));
}