/**
 * @file DspEngine.cpp
 * @brief AuraForge 50X - 10-Band Parametric Equalizer & Audio DSP Processing Engine Implementation
 * @author Ravi Kachhwaha
 */

#include "DspEngine.h"
#include <math.h>

DspEngine g_dspEngine;

static const float EQ_CENTER_FREQS[NUM_EQ_BANDS] = {
    31.0f, 62.0f, 125.0f, 250.0f, 500.0f, 1000.0f, 2000.0f, 4000.0f, 8000.0f, 16000.0f
};

BiquadFilterBand::BiquadFilterBand() { reset(); }

void BiquadFilterBand::reset() {
    x1 = x2 = 0.0f;
    coeffs.b0 = 1.0f; coeffs.b1 = 0.0f; coeffs.b2 = 0.0f;
    coeffs.a1 = 0.0f; coeffs.a2 = 0.0f;
}

void BiquadFilterBand::setPeaking(float f0, float fs, float gain_dB, float Q) {
    if (fabsf(gain_dB) < 0.01f) {
        reset();
        return;
    }
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

void BiquadFilterBand::setLowShelf(float f0, float fs, float gain_dB) {
    if (fabsf(gain_dB) < 0.01f) {
        reset();
        return;
    }
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
    coeffs.a2 = ((A + 1.0f) + (A - 1.0f) * cos_w0 - 2.0f * sqrtf(A) * alpha) / a0;
}

void BiquadFilterBand::setHighShelf(float f0, float fs, float gain_dB) {
    if (fabsf(gain_dB) < 0.01f) {
        reset();
        return;
    }
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
    coeffs.a2 = ((A + 1.0f) - (A - 1.0f) * cos_w0 - 2.0f * sqrtf(A) * alpha) / a0;
}

void BiquadFilterBand::setHighPass(float f0, float fs, float Q) {
    float w0 = 2.0f * PI * f0 / fs;
    float cos_w0 = cosf(w0);
    float sin_w0 = sinf(w0);
    float alpha = sin_w0 / (2.0f * Q);

    float a0 = 1.0f + alpha;
    coeffs.b0 = ((1.0f + cos_w0) / 2.0f) / a0;
    coeffs.b1 = (-(1.0f + cos_w0)) / a0;
    coeffs.b2 = ((1.0f + cos_w0) / 2.0f) / a0;
    coeffs.a1 = (-2.0f * cos_w0) / a0;
    coeffs.a2 = (1.0f - alpha) / a0;
}

void BiquadFilterBand::setLowPass(float f0, float fs, float Q) {
    float w0 = 2.0f * PI * f0 / fs;
    float cos_w0 = cosf(w0);
    float sin_w0 = sinf(w0);
    float alpha = sin_w0 / (2.0f * Q);

    float a0 = 1.0f + alpha;
    coeffs.b0 = ((1.0f - cos_w0) / 2.0f) / a0;
    coeffs.b1 = (1.0f - cos_w0) / a0;
    coeffs.b2 = ((1.0f - cos_w0) / 2.0f) / a0;
    coeffs.a1 = (-2.0f * cos_w0) / a0;
    coeffs.a2 = (1.0f - alpha) / a0;
}

DspEngine::DspEngine() {
    for (int i = 0; i < FFT_BANDS; i++) {
        fftMags[i] = 0.0f;
    }
}

void DspEngine::begin() {
    Serial.println("[DSP Engine] Initialized 10-Band Parametric EQ & Dynamic Audio Engine.");
}

void DspEngine::updateFromConfig(const SystemConfig& cfg) {
    isMuted = cfg.isMuted;
    masterVolumeLinear = (float)cfg.volume / 100.0f;
    preAmpLinear = powf(10.0f, cfg.preAmpGain / 20.0f);

    if (cfg.balance < 0.0f) {
        balanceGainL = 1.0f;
        balanceGainR = 1.0f + cfg.balance;
    } else {
        balanceGainL = 1.0f - cfg.balance;
        balanceGainR = 1.0f;
    }

    stereoWidth = cfg.stereoWidth;
    monoMode = cfg.monoMode;
    phaseInvertL = cfg.phaseInvertL;
    phaseInvertR = cfg.phaseInvertR;
    dynamicBassEn = cfg.dynamicBassEnabled;

    // Configure 10-Band EQ Filters
    for (int i = 0; i < NUM_EQ_BANDS; i++) {
        float f0 = EQ_CENTER_FREQS[i];
        float gain = cfg.eqGains[i];

        if (i == 0) {
            eqL[i].setLowShelf(f0, DSP_SAMPLE_RATE, gain);
            eqR[i].setLowShelf(f0, DSP_SAMPLE_RATE, gain);
        } else if (i == NUM_EQ_BANDS - 1) {
            eqL[i].setHighShelf(f0, DSP_SAMPLE_RATE, gain);
            eqR[i].setHighShelf(f0, DSP_SAMPLE_RATE, gain);
        } else {
            eqL[i].setPeaking(f0, DSP_SAMPLE_RATE, gain, 1.414f);
            eqR[i].setPeaking(f0, DSP_SAMPLE_RATE, gain, 1.414f);
        }
    }

    // High Pass Subsonic Filter
    highPassL.setHighPass(cfg.highPassCutoff, DSP_SAMPLE_RATE);
    highPassR.setHighPass(cfg.highPassCutoff, DSP_SAMPLE_RATE);

    // Dynamic Bass Filter
    if (dynamicBassEn) {
        dynamicBassL.setLowShelf(cfg.dynamicBassFreq, DSP_SAMPLE_RATE, cfg.dynamicBassGain);
        dynamicBassR.setLowShelf(cfg.dynamicBassFreq, DSP_SAMPLE_RATE, cfg.dynamicBassGain);
    } else {
        dynamicBassL.reset();
        dynamicBassR.reset();
    }
}

void DspEngine::setSignalGenerator(SignalGenWaveform mode, float freqHz, float amplitude) {
    genMode = mode;
    genFreq = freqHz;
    genAmplitude = amplitude;
    genPhase = 0.0f;
}

void DspEngine::processAudioBlock(int16_t* samples, uint32_t sampleCount) {
    if (isMuted) {
        memset(samples, 0, sampleCount * 4);
        return;
    }

    // Temporary accumulator for spectrum telemetry decay
    float bandEnergy[FFT_BANDS] = {0};

    for (uint32_t i = 0; i < sampleCount; i++) {
        float sampleL = 0.0f;
        float sampleR = 0.0f;

        if (genMode != WAVE_OFF) {
            // Signal Generator Processing
            if (genMode == WAVE_SINE) {
                float val = sinf(genPhase) * 32767.0f * genAmplitude;
                sampleL = sampleR = val;
                genPhase += (2.0f * PI * genFreq) / DSP_SAMPLE_RATE;
                if (genPhase >= 2.0f * PI) genPhase -= 2.0f * PI;
            } else if (genMode == WAVE_SQUARE) {
                float val = (sinf(genPhase) >= 0 ? 1.0f : -1.0f) * 32767.0f * genAmplitude;
                sampleL = sampleR = val;
                genPhase += (2.0f * PI * genFreq) / DSP_SAMPLE_RATE;
                if (genPhase >= 2.0f * PI) genPhase -= 2.0f * PI;
            } else if (genMode == WAVE_PINK_NOISE) {
                float raw = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * 32767.0f * genAmplitude;
                sampleL = sampleR = raw;
            }
        } else {
            // Input audio from A2DP Sink
            sampleL = (float)samples[i * 2];
            sampleR = (float)samples[i * 2 + 1];
        }

        // 1. Pre-Amp Gain
        sampleL *= preAmpLinear;
        sampleR *= preAmpLinear;

        // 2. High-Pass Subsonic Filter
        sampleL = highPassL.process(sampleL);
        sampleR = highPassR.process(sampleR);

        // 3. Dynamic Bass Enhancer
        if (dynamicBassEn) {
            sampleL = dynamicBassL.process(sampleL);
            sampleR = dynamicBassR.process(sampleR);
        }

        // 4. 10-Band Equalizer Cascade
        for (int b = 0; b < NUM_EQ_BANDS; b++) {
            sampleL = eqL[b].process(sampleL);
            sampleR = eqR[b].process(sampleR);
        }

        // Simple band energy bucket calculation for spectrum visualizer
        float monoAbs = fabsf(sampleL + sampleR) * 0.5f;
        int bucket = (i % FFT_BANDS);
        bandEnergy[bucket] += monoAbs;

        // 5. Stereo Expander / Mono Matrix
        if (monoMode) {
            float monoVal = (sampleL + sampleR) * 0.5f;
            sampleL = sampleR = monoVal;
        } else if (fabsf(stereoWidth - 1.0f) > 0.01f) {
            float diff = (sampleL - sampleR) * 0.5f * (stereoWidth - 1.0f);
            sampleL += diff;
            sampleR -= diff;
        }

        // 6. Balance & Channel Phase Inversion
        sampleL *= balanceGainL * (phaseInvertL ? -1.0f : 1.0f);
        sampleR *= balanceGainR * (phaseInvertR ? -1.0f : 1.0f);

        // 7. Master Volume Scaling
        sampleL *= masterVolumeLinear;
        sampleR *= masterVolumeLinear;

        // 8. Soft-clipping Hyperbolic Saturation Limiter
        const float threshold = 30000.0f;
        if (fabsf(sampleL) > threshold) {
            float sign = (sampleL > 0) ? 1.0f : -1.0f;
            sampleL = sign * (threshold + (32767.0f - threshold) * tanhf((fabsf(sampleL) - threshold) / (32767.0f - threshold)));
        }
        if (fabsf(sampleR) > threshold) {
            float sign = (sampleR > 0) ? 1.0f : -1.0f;
            sampleR = sign * (threshold + (32767.0f - threshold) * tanhf((fabsf(sampleR) - threshold) / (32767.0f - threshold)));
        }

        samples[i * 2]     = (int16_t)sampleL;
        samples[i * 2 + 1] = (int16_t)sampleR;
    }

    // Smooth decay updates for spectrum telemetry
    for (int b = 0; b < FFT_BANDS; b++) {
        float normLevel = (bandEnergy[b] / (float)(sampleCount / FFT_BANDS + 1)) / 327.0f;
        if (normLevel > 100.0f) normLevel = 100.0f;
        fftMags[b] = fftMags[b] * 0.6f + normLevel * 0.4f;
    }
}

void DspEngine::getSpectrumMagnitudes(uint8_t* bandLevels, uint8_t count) {
    for (uint8_t i = 0; i < count && i < FFT_BANDS; i++) {
        bandLevels[i] = (uint8_t)fftMags[i];
    }
}
