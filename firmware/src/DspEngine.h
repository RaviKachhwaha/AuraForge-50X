/**
 * @file DspEngine.h
 * @brief AuraForge 50X - 10-Band Parametric Equalizer & Audio DSP Processing Engine
 * @author Ravi Kachhwaha
 */

#ifndef DSP_ENGINE_H
#define DSP_ENGINE_H

#include <Arduino.h>
#include "ConfigManager.h"

#define DSP_SAMPLE_RATE 44100
#define FFT_BANDS 16

enum SignalGenWaveform {
    WAVE_OFF = 0,
    WAVE_SINE,
    WAVE_SQUARE,
    WAVE_PINK_NOISE,
    WAVE_SWEEP
};

struct BiquadCoefficients {
    float b0, b1, b2, a1, a2;
};

class BiquadFilterBand {
public:
    BiquadFilterBand();
    void reset();
    void setPeaking(float f0, float fs, float gain_dB, float Q = 1.414f);
    void setLowShelf(float f0, float fs, float gain_dB);
    void setHighShelf(float f0, float fs, float gain_dB);
    void setHighPass(float f0, float fs, float Q = 0.707f);
    void setLowPass(float f0, float fs, float Q = 0.707f);
    
    inline float process(float in) {
        float out = coeffs.b0 * in + x1;
        x1 = coeffs.b1 * in - coeffs.a1 * out + x2;
        x2 = coeffs.b2 * in - coeffs.a2 * out;
        return out;
    }

private:
    BiquadCoefficients coeffs;
    float x1, x2;
};

class DspEngine {
public:
    DspEngine();
    void begin();
    void updateFromConfig(const SystemConfig& cfg);

    // Main Audio Block Processor
    void processAudioBlock(int16_t* samples, uint32_t sampleCount);

    // Audio Signal Generator Controls
    void setSignalGenerator(SignalGenWaveform mode, float freqHz = 1000.0f, float amplitude = 0.5f);

    // FFT Spectrum Telemetry for Web UI
    void getSpectrumMagnitudes(uint8_t* bandLevels, uint8_t count = FFT_BANDS);

private:
    BiquadFilterBand eqL[NUM_EQ_BANDS];
    BiquadFilterBand eqR[NUM_EQ_BANDS];
    BiquadFilterBand highPassL, highPassR;
    BiquadFilterBand dynamicBassL, dynamicBassR;

    float preAmpLinear = 1.0f;
    float balanceGainL = 1.0f;
    float balanceGainR = 1.0f;
    float stereoWidth = 1.0f;
    bool monoMode = false;
    bool phaseInvertL = false;
    bool phaseInvertR = false;
    bool dynamicBassEn = true;
    bool isMuted = false;
    float masterVolumeLinear = 1.0f;

    // Signal Generator state
    SignalGenWaveform genMode = WAVE_OFF;
    float genFreq = 1000.0f;
    float genAmplitude = 0.5f;
    float genPhase = 0.0f;

    // Spectrum Analyzer telemetry state
    float fftMags[FFT_BANDS];
};

extern DspEngine g_dspEngine;

#endif // DSP_ENGINE_H
