/**
 * @file DspEngine.cpp
 * @brief AuraForge 50X - 10-Band Parametric Equalizer, 128-Point Radix-2 FFT &
 * Audio DSP Engine Implementation
 * @author Ravi Kachhwaha
 */

#include "DspEngine.h"
#include <math.h>

DspEngine g_dspEngine;

static const float EQ_CENTER_FREQS[NUM_EQ_BANDS] = {
    31.0f,   62.0f,   125.0f,  250.0f,  500.0f,
    1000.0f, 2000.0f, 4000.0f, 8000.0f, 16000.0f};

// Logarithmic bin mapping table for 16-band visualizer from 128-point FFT
static const uint8_t FFT_BIN_START[FFT_BANDS] = {1, 1,  2,  2,  3,  4,  5,  7,
                                                 9, 12, 16, 21, 28, 37, 48, 58};
static const uint8_t FFT_BIN_END[FFT_BANDS] = {1,  2,  2,  3,  4,  5,  7,  9,
                                               12, 16, 21, 28, 37, 48, 58, 63};

BiquadFilterBand::BiquadFilterBand() { reset(); }

void BiquadFilterBand::reset() {
  x1 = x2 = 0.0f;
  coeffs.b0 = 1.0f;
  coeffs.b1 = 0.0f;
  coeffs.b2 = 0.0f;
  coeffs.a1 = 0.0f;
  coeffs.a2 = 0.0f;
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
  float alpha =
      sin_w0 / 2.0f * sqrtf((A + 1.0f / A) * (1.0f / 0.707f - 1.0f) + 2.0f);

  float a0 = (A + 1.0f) + (A - 1.0f) * cos_w0 + 2.0f * sqrtf(A) * alpha;
  coeffs.b0 =
      (A * ((A + 1.0f) - (A - 1.0f) * cos_w0 + 2.0f * sqrtf(A) * alpha)) / a0;
  coeffs.b1 = (2.0f * A * ((A - 1.0f) - (A + 1.0f) * cos_w0)) / a0;
  coeffs.b2 =
      (A * ((A + 1.0f) - (A - 1.0f) * cos_w0 - 2.0f * sqrtf(A) * alpha)) / a0;
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
  float alpha =
      sin_w0 / 2.0f * sqrtf((A + 1.0f / A) * (1.0f / 0.707f - 1.0f) + 2.0f);

  float a0 = (A + 1.0f) - (A - 1.0f) * cos_w0 + 2.0f * sqrtf(A) * alpha;
  coeffs.b0 =
      (A * ((A + 1.0f) + (A - 1.0f) * cos_w0 + 2.0f * sqrtf(A) * alpha)) / a0;
  coeffs.b1 = (-2.0f * A * ((A - 1.0f) + (A + 1.0f) * cos_w0)) / a0;
  coeffs.b2 =
      (A * ((A + 1.0f) + (A - 1.0f) * cos_w0 - 2.0f * sqrtf(A) * alpha)) / a0;
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
    fftPeakHold[i] = 0.0f;
  }
  for (int i = 0; i < FFT_SIZE; i++) {
    fftAudioBuffer[i] = 0.0f;
    fftReal[i] = 0.0f;
    fftImag[i] = 0.0f;
  }
  initFftWindow();
}

void DspEngine::initFftWindow() {
  // Standard Hann Window: 0.5 * (1 - cos(2*PI*n / (N-1)))
  for (int i = 0; i < FFT_SIZE; i++) {
    fftWindow[i] =
        0.5f * (1.0f - cosf(2.0f * PI * (float)i / (float)(FFT_SIZE - 1)));
  }
}

void DspEngine::begin() {
  initFftWindow();
  Serial.println("[DSP Engine] Initialized 10-Band Parametric EQ & 128-Point "
                 "Radix-2 FFT.");
}

void DspEngine::updateFromConfig(const SystemConfig &cfg) {
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

  drcThresholdLinear = powf(10.0f, cfg.drcThreshold / 20.0f);
  drcRatio = max(1.0f, cfg.drcRatio);

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
    dynamicBassL.setLowShelf(cfg.dynamicBassFreq, DSP_SAMPLE_RATE,
                             cfg.dynamicBassGain);
    dynamicBassR.setLowShelf(cfg.dynamicBassFreq, DSP_SAMPLE_RATE,
                             cfg.dynamicBassGain);
  } else {
    dynamicBassL.reset();
    dynamicBassR.reset();
  }
}

void DspEngine::setThermalGovernorGain(float multiplier) {
  if (multiplier < 0.1f)
    multiplier = 0.1f;
  if (multiplier > 1.0f)
    multiplier = 1.0f;
  thermalGovernorMultiplier = multiplier;
}

void DspEngine::setSignalGenerator(SignalGenWaveform mode, float freqHz,
                                   float amplitude) {
  genMode = mode;
  genFreq = freqHz;
  genAmplitude = amplitude;
  genPhase = 0.0f;
}

// 128-Point Radix-2 Cooley-Tukey In-Place Decimation-in-Time Fast Fourier
// Transform
void DspEngine::runRadix2FFT() {
  // 1. Bit-Reversal Permutation (7 bits for N=128)
  for (int i = 0; i < FFT_SIZE; i++) {
    uint8_t rev = 0;
    uint8_t temp = (uint8_t)i;
    for (int b = 0; b < 7; b++) {
      rev = (rev << 1) | (temp & 1);
      temp >>= 1;
    }
    if (rev > i) {
      float tr = fftReal[i];
      fftReal[i] = fftReal[rev];
      fftReal[rev] = tr;
      float ti = fftImag[i];
      fftImag[i] = fftImag[rev];
      fftImag[rev] = ti;
    }
  }

  // 2. Butterfly Computations across 7 stages
  for (int s = 1; s <= 7; s++) {
    int m = 1 << s;  // 2, 4, 8, 16, 32, 64, 128
    int m2 = m >> 1; // 1, 2, 4, 8, 16, 32, 64
    float theta = -2.0f * PI / (float)m;
    float wpr = cosf(theta);
    float wpi = sinf(theta);

    for (int k = 0; k < FFT_SIZE; k += m) {
      float wr = 1.0f;
      float wi = 0.0f;
      for (int j = 0; j < m2; j++) {
        int idx1 = k + j;
        int idx2 = idx1 + m2;
        float tempr = wr * fftReal[idx2] - wi * fftImag[idx2];
        float tempi = wr * fftImag[idx2] + wi * fftReal[idx2];

        fftReal[idx2] = fftReal[idx1] - tempr;
        fftImag[idx2] = fftImag[idx1] - tempi;
        fftReal[idx1] += tempr;
        fftImag[idx1] += tempi;

        float wtemp = wr;
        wr = wr * wpr - wi * wpi;
        wi = wtemp * wpi + wi * wpr;
      }
    }
  }
}

void DspEngine::computeSpectrumBands() {
  runRadix2FFT();

  // Map 64 single-sided FFT bins into 16 logarithmically scaled energy bands
  for (int b = 0; b < FFT_BANDS; b++) {
    float sum = 0.0f;
    int startBin = FFT_BIN_START[b];
    int endBin = FFT_BIN_END[b];
    int count = (endBin - startBin + 1);

    for (int k = startBin; k <= endBin; k++) {
      float mag = sqrtf(fftReal[k] * fftReal[k] + fftImag[k] * fftImag[k]);
      sum += mag;
    }
    float avgMag = (sum / (float)count) / 327.68f; // Normalized 0 - 100%
    if (avgMag > 100.0f)
      avgMag = 100.0f;

    // Smooth Exponential Decay Falloff
    if (avgMag >= fftMags[b]) {
      fftMags[b] = avgMag;
    } else {
      fftMags[b] = fftMags[b] * 0.72f + avgMag * 0.28f;
    }

    // Peak Hold
    if (fftMags[b] > fftPeakHold[b]) {
      fftPeakHold[b] = fftMags[b];
    } else {
      fftPeakHold[b] *= 0.96f;
    }
  }
}

void DspEngine::processAudioBlock(int16_t *samples, uint32_t sampleCount) {
  if (isMuted) {
    memset(samples, 0, sampleCount * 4);
    return;
  }

  // Smooth Closed-Loop Thermal Governor Multiplier
  smoothedThermalGain =
      smoothedThermalGain * 0.99f + thermalGovernorMultiplier * 0.01f;
  float effectiveMasterVol = masterVolumeLinear * smoothedThermalGain;

  float maxBlockPeak = 0.0f;

  for (uint32_t i = 0; i < sampleCount; i++) {
    float sampleL = 0.0f;
    float sampleR = 0.0f;

    if (genMode != WAVE_OFF) {
      // Signal Generator Processing
      if (genMode == WAVE_SINE) {
        float val = sinf(genPhase) * 32767.0f * genAmplitude;
        sampleL = sampleR = val;
        genPhase += (2.0f * PI * genFreq) / DSP_SAMPLE_RATE;
        if (genPhase >= 2.0f * PI)
          genPhase -= 2.0f * PI;
      } else if (genMode == WAVE_SQUARE) {
        float val =
            (sinf(genPhase) >= 0 ? 1.0f : -1.0f) * 32767.0f * genAmplitude;
        sampleL = sampleR = val;
        genPhase += (2.0f * PI * genFreq) / DSP_SAMPLE_RATE;
        if (genPhase >= 2.0f * PI)
          genPhase -= 2.0f * PI;
      } else if (genMode == WAVE_PINK_NOISE) {
        float raw = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * 32767.0f *
                    genAmplitude;
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

    // 7. Master Volume & Closed-Loop Thermal Governor Scaling
    sampleL *= effectiveMasterVol;
    sampleR *= effectiveMasterVol;

    // Peak Tracking for Dynamic Range Compressor (DRC)
    float currentPeak = max(fabsf(sampleL), fabsf(sampleR)) / 32768.0f;
    if (currentPeak > maxBlockPeak)
      maxBlockPeak = currentPeak;

    // 8. Soft-clipping Hyperbolic Saturation Limiter
    const float threshold = 30000.0f;
    if (fabsf(sampleL) > threshold) {
      float sign = (sampleL > 0) ? 1.0f : -1.0f;
      sampleL = sign * (threshold + (32767.0f - threshold) *
                                        tanhf((fabsf(sampleL) - threshold) /
                                              (32767.0f - threshold)));
    }
    if (fabsf(sampleR) > threshold) {
      float sign = (sampleR > 0) ? 1.0f : -1.0f;
      sampleR = sign * (threshold + (32767.0f - threshold) *
                                        tanhf((fabsf(sampleR) - threshold) /
                                              (32767.0f - threshold)));
    }

    samples[i * 2] = (int16_t)sampleL;
    samples[i * 2 + 1] = (int16_t)sampleR;

    // Capture mono downmix sample into 128-point FFT window buffer
    fftAudioBuffer[fftBufIndex++] = (sampleL + sampleR) * 0.5f;
    if (fftBufIndex >= FFT_SIZE) {
      fftBufIndex = 0;
      for (int k = 0; k < FFT_SIZE; k++) {
        fftReal[k] = fftAudioBuffer[k] * fftWindow[k]; // Apply Hann Window
        fftImag[k] = 0.0f;
      }
      computeSpectrumBands();
    }
  }

  // Dynamic Range Compressor (DRC) Gain Reduction Calculation
  if (maxBlockPeak > drcThresholdLinear && drcThresholdLinear > 0.001f) {
    float peak_dB = 20.0f * log10f(maxBlockPeak);
    float thresh_dB = 20.0f * log10f(drcThresholdLinear);
    drcGainReduction_dB = (1.0f - 1.0f / drcRatio) * (peak_dB - thresh_dB);
  } else {
    drcGainReduction_dB *= 0.85f; // Release decay
  }
}

void DspEngine::getSpectrumMagnitudes(uint8_t *bandLevels, uint8_t count) {
  for (uint8_t i = 0; i < count && i < FFT_BANDS; i++) {
    bandLevels[i] = (uint8_t)fftMags[i];
  }
}
