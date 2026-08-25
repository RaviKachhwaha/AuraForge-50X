/**
 * @file main.cpp
 * @brief AuraForge 50X - High-Fidelity Bluetooth Audio & DSP Firmware
 * @author Ravi Kachhwaha
 * @license MIT
 */

#include <Arduino.h>
#include <WiFi.h>
#include <driver/i2s.h>
#include <driver/dac.h>
#include "BluetoothA2DPSink.h"

// ==============================================================================
// HARDWARE PIN DEFINITIONS
// ==============================================================================
#define PIN_AMP_SDZ       22    // TPA3116D2 Shutdown / Mute Pin (Active Low)
#define PIN_AMP_FAULT     23    // TPA3116D2 Fault Status (Input, Pull-up)
#define PIN_STATUS_LED    2     // General Status / Bluetooth LED
#define PIN_AUDIO_DAC_L   25    // ESP32 DAC Channel 1 (Audio Left)
#define PIN_AUDIO_DAC_R   26    // ESP32 DAC Channel 2 (Audio Right)

// I2S Configuration (Set to Internal 8-bit DAC routing or External I2S DAC)
#define USE_INTERNAL_DAC  true
#define AUDIO_SAMPLE_RATE 44100

// ==============================================================================
// 3-BAND BIQUAD IIR DSP FILTER ENGINE
// ==============================================================================
struct BiquadCoeffs {
    float b0, b1, b2, a1, a2;
};

class BiquadFilter {
public:
    BiquadFilter() { reset(); }

    void reset() {
        x1 = x2 = y1 = y2 = 0.0f;
    }

    void setLowShelf(float f0, float fs, float gain_dB) {
        float A = powf(10.0f, gain_dB / 40.0f);
        float w0 = 2.0f * PI * f0 / fs;
        float cos_w0 = cosf(w0);
        float sin_w0 = sinf(w0);
        float alpha = sin_w0 / 2.0f * sqrtf((A + 1.0f / A) * (1.0f / 0.707f - 1.0f) + 2.0f);

        float a0 = (A + 1.0f) + (A - 1.0f) * cos_w0 + 2.0f * sqrtf(A) * alpha;
        coeffs.b0 = (A * ((A + 1.0f) - (A - 1.0f) * cos_w0 + 2.0f * sqrtf(A) * alpha)) / a0;
        coeffs.b1 = (2.0f * A * ((A - 1.0f) - (A + 1.0f) * cos_w0)) / a0;
        coeffs.b2 = (A * ((A + 1.0f) - (A - 1.0f) * cos_w0 - 2.0f * sqrtf(A) * alpha)) / a0;
        coeffs.a1 = (-2.0f * ((A - 1.0f) + (A + 1.0f) * cos_w0)) / a0;
        coeffs.a2 = ((A + 1.0f) + (A - 1.0f) * cos_w0 - 2.0f * sqrtf(A) * alpha)) / a0;
    }

    void setPeaking(float f0, float fs, float gain_dB, float Q = 1.0f) {
        float A = powf(10.0f, gain_dB / 40.0f);
        float w0 = 2.0f * PI * f0 / fs;
        float cos_w0 = cosf(w0);
        float sin_w0 = sinf(w0);
        float alpha = sin_w0 / (2.0f * Q);

        float a0 = 1.0f + alpha / A;
        coeffs.b0 = (1.0f + alpha * A) / a0;
        coeffs.b1 = (-2.0f * cos_w0) / a0;
        coeffs.b2 = (1.0f - alpha * A) / a0;
        coeffs.a1 = (-2.0f * cos_w0) / a0;
        coeffs.a2 = (1.0f - alpha / A) / a0;
    }

    void setHighShelf(float f0, float fs, float gain_dB) {
        float A = powf(10.0f, gain_dB / 40.0f);
        float w0 = 2.0f * PI * f0 / fs;
        float cos_w0 = cosf(w0);
        float sin_w0 = sinf(w0);
        float alpha = sin_w0 / 2.0f * sqrtf((A + 1.0f / A) * (1.0f / 0.707f - 1.0f) + 2.0f);

        float a0 = (A + 1.0f) - (A - 1.0f) * cos_w0 + 2.0f * sqrtf(A) * alpha;
        coeffs.b0 = (A * ((A + 1.0f) + (A - 1.0f) * cos_w0 + 2.0f * sqrtf(A) * alpha)) / a0;
        coeffs.b1 = (-2.0f * A * ((A - 1.0f) + (A + 1.0f) * cos_w0)) / a0;
        coeffs.b2 = (A * ((A + 1.0f) + (A - 1.0f) * cos_w0 - 2.0f * sqrtf(A) * alpha)) / a0;
        coeffs.a1 = (2.0f * ((A - 1.0f) - (A + 1.0f) * cos_w0)) / a0;
        coeffs.a2 = ((A + 1.0f) - (A - 1.0f) * cos_w0 - 2.0f * sqrtf(A) * alpha)) / a0;
    }

    inline float process(float in) {
        // Direct Form II Transposed structure for numerical stability
        float out = coeffs.b0 * in + x1;
        x1 = coeffs.b1 * in - coeffs.a1 * out + x2;
        x2 = coeffs.b2 * in - coeffs.a2 * out;
        return out;
    }

private:
    BiquadCoeffs coeffs;
    float x1, x2, y1, y2;
};

// Stereo Filter Banks
BiquadFilter filterBassL, filterBassR;
BiquadFilter filterMidL,  filterMidR;
BiquadFilter filterTrebL, filterTrebR;

float g_bassGain_dB  = 2.5f;   // Bass Boost (+2.5dB @ 100Hz)
float g_midGain_dB   = 0.0f;   // Mid Flat (0dB @ 1kHz)
float g_trebleGain_dB= 1.5f;   // Treble Crisp (+1.5dB @ 8kHz)

void updateDspParameters() {
    filterBassL.setLowShelf(100.0f, AUDIO_SAMPLE_RATE, g_bassGain_dB);
    filterBassR.setLowShelf(100.0f, AUDIO_SAMPLE_RATE, g_bassGain_dB);
    
    filterMidL.setPeaking(1000.0f, AUDIO_SAMPLE_RATE, g_midGain_dB, 1.2f);
    filterMidR.setPeaking(1000.0f, AUDIO_SAMPLE_RATE, g_midGain_dB, 1.2f);
    
    filterTrebL.setHighShelf(8000.0f, AUDIO_SAMPLE_RATE, g_trebleGain_dB);
    filterTrebR.setHighShelf(8000.0f, AUDIO_SAMPLE_RATE, g_trebleGain_dB);
}

// ==============================================================================
// AUDIO PIPELINE & BLUETOOTH A2DP SINK
// ==============================================================================
BluetoothA2DPSink a2dp_sink;

void audioDataCallback(const uint8_t *data, uint32_t len) {
    int16_t *samples = (int16_t *)data;
    uint32_t sample_count = len / 4; // 2 channels * 16-bit (2 bytes) = 4 bytes per sample frame

    for (uint32_t i = 0; i < sample_count; i++) {
        float sampleL = (float)samples[i * 2];
        float sampleR = (float)samples[i * 2 + 1];

        // Process through 3-Stage Biquad Cascade (Left Channel)
        sampleL = filterBassL.process(sampleL);
        sampleL = filterMidL.process(sampleL);
        sampleL = filterTrebL.process(sampleL);

        // Process through 3-Stage Biquad Cascade (Right Channel)
        sampleR = filterBassR.process(sampleR);
        sampleR = filterMidR.process(sampleR);
        sampleR = filterTrebR.process(sampleR);

        // Soft-clipping saturation limiter (Prevents digital wrap-around distortion)
        if (sampleL > 32767.0f) sampleL = 32767.0f;
        if (sampleL < -32768.0f) sampleL = -32768.0f;
        if (sampleR > 32767.0f) sampleR = 32767.0f;
        if (sampleR < -32768.0f) sampleR = -32768.0f;

        samples[i * 2]     = (int16_t)sampleL;
        samples[i * 2 + 1] = (int16_t)sampleR;
    }
}

void avrcpStateCallback(esp_a2d_connection_state_t state, void *ptr) {
    if (state == ESP_A2D_CONNECTION_STATE_CONNECTED) {
        Serial.println("[AuraForge 50X] Bluetooth Source Connected!");
        digitalWrite(PIN_STATUS_LED, HIGH);
    } else if (state == ESP_A2D_CONNECTION_STATE_DISCONNECTED) {
        Serial.println("[AuraForge 50X] Bluetooth Source Disconnected.");
        digitalWrite(PIN_STATUS_LED, LOW);
    }
}

// ==============================================================================
// AMPLIFIER POWER MANAGEMENT (ANTI-POP SEQUENCE)
// ==============================================================================
void enableAmplifier(bool enable) {
    if (enable) {
        delay(100); // Allow DC power rail (+21V_BOOST) to fully stabilize
        digitalWrite(PIN_AMP_SDZ, HIGH); // De-assert shutdown -> Enable TPA3116D2
        Serial.println("[AuraForge 50X] Amplifier Output Unmuted.");
    } else {
        digitalWrite(PIN_AMP_SDZ, LOW);  // Assert shutdown -> Mute instantly
        Serial.println("[AuraForge 50X] Amplifier Output Muted.");
    }
}

// ==============================================================================
// SERIAL COMMAND INTERPRETER (CLI)
// ==============================================================================
void handleSerialCommands() {
    if (!Serial.available()) return;

    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd.startsWith("bass ")) {
        g_bassGain_dB = cmd.substring(5).toFloat();
        updateDspParameters();
        Serial.printf("[DSP] Bass set to: %.1f dB\n", g_bassGain_dB);
    } else if (cmd.startsWith("mid ")) {
        g_midGain_dB = cmd.substring(4).toFloat();
        updateDspParameters();
        Serial.printf("[DSP] Mid set to: %.1f dB\n", g_midGain_dB);
    } else if (cmd.startsWith("treble ")) {
        g_trebleGain_dB = cmd.substring(7).toFloat();
        updateDspParameters();
        Serial.printf("[DSP] Treble set to: %.1f dB\n", g_trebleGain_dB);
    } else if (cmd.equalsIgnoreCase("status")) {
        Serial.println("--- AuraForge 50X System Status ---");
        Serial.printf("ESP32 Chip Rev: %d | CPU Freq: %d MHz\n", ESP.getChipRevision(), ESP.getCpuFreqMHz());
        Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
        Serial.printf("DSP EQ -> Bass: %.1f dB | Mid: %.1f dB | Treble: %.1f dB\n", g_bassGain_dB, g_midGain_dB, g_trebleGain_dB);
        Serial.printf("TPA3116D2 FAULT Pin State: %s\n", digitalRead(PIN_AMP_FAULT) == HIGH ? "NORMAL (OK)" : "FAULT DETECTED");
    }
}

// ==============================================================================
// SETUP & MAIN LOOP
// ==============================================================================
void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("\n=============================================");
    Serial.println(" AuraForge 50X - High-Fidelity Audio DSP ");
    Serial.println(" Hardware: 4-Layer KiCad 10 | TPA3116D2 + TPS61088");
    Serial.println("=============================================");

    // Initialize Hardware Pins
    pinMode(PIN_AMP_SDZ, OUTPUT);
    pinMode(PIN_STATUS_LED, OUTPUT);
    pinMode(PIN_AMP_FAULT, INPUT_PULLUP);

    digitalWrite(PIN_AMP_SDZ, LOW); // Start with amplifier muted (anti-pop)
    digitalWrite(PIN_STATUS_LED, LOW);

    // Initialize DSP Filters
    updateDspParameters();

    // Configure I2S / Internal DAC Settings
    static i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN),
        .sample_rate = AUDIO_SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_MSB,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 128,
        .use_apll = true,
        .tx_desc_auto_clear = true
    };

    a2dp_sink.set_i2s_config(i2s_config);
    a2dp_sink.set_stream_reader(audioDataCallback, false);
    a2dp_sink.set_on_connection_state_changed(avrcpStateCallback);

    // Start Bluetooth A2DP Sink
    a2dp_sink.start("AuraForge-50X-Audio");
    Serial.println("[AuraForge 50X] Bluetooth Advertising as 'AuraForge-50X-Audio'...");

    // Unmute amplifier stage after audio pipeline initialization
    enableAmplifier(true);
}

void loop() {
    handleSerialCommands();

    // Check Hardware Over-Temperature / DC-Detect Fault Line
    if (digitalRead(PIN_AMP_FAULT) == LOW) {
        Serial.println("[CRITICAL WARNING] TPA3116D2 Fault line pulled LOW! Check speaker load impedance.");
        delay(1000);
    }

    vTaskDelay(pdMS_TO_TICKS(10));
}