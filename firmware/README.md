# AuraForge 50X — Programmable 50W High-Fidelity Wireless DSP & CSI Radar Platform

[![KiCad 10](https://img.shields.io/badge/EDA-KiCad%2010.0-blue.svg)](https://kicad.org)
[![PCB Layers](https://img.shields.io/badge/PCB%20Layers-4--Layer%20Stackup-orange.svg)]()
[![DRC Status](https://img.shields.io/badge/DRC%20Status-Passed%20(0%20Errors)-brightgreen.svg)]()
[![ESP32 Dual-Core](https://img.shields.io/badge/ESP32-240MHz%20Dual--Core-red.svg)](https://espressif.com)
[![Wi-Fi CSI Radar](https://img.shields.io/badge/Wi--Fi%20CSI-RuView%2050Hz%20UDP-cyan.svg)]()
[![Hack Club](https://img.shields.io/badge/Hack%20Club-Forge%20Grant-red.svg)](https://hackclub.com)

**AuraForge 50X** is a programmable 4-layer audio computer, digital signal processing (DSP) station, and wireless sensing platform. Going far beyond standard single-purpose Bluetooth receiver boards, AuraForge 50X combines:
1. **High-Power Class-D Audio Stage**: Texas Instruments **TPA3116D2** dual BTL stereo amplifier delivering up to $2 \times 50\text{W}$ into $4\Omega$ at $21\text{V}$.
2. **Synchronous 10A Boost Power Converter**: Texas Instruments **TPS61088** boosting a single-cell 3.7V lithium battery up to 21.0V with anti-pop soft-start sequencing.
3. **Advanced Digital Signal Processing (DSP)**: 10-Band Parametric Biquad IIR Equalizer, Dynamic Sub-Bass Psychoacoustic Harmonics, 3D Spatial Stereo Expander, and Dynamic Range Compressor (DRC).
4. **Wi-Fi Channel State Information (CSI) Motion Sensing Radar**: 64-subcarrier spatial disturbance sensing engine streaming live at 50 Hz over UDP (Port 5000) directly to PC software (**RuView**, Python, MATLAB).
5. **Cyberpunk Holographic Web Command Station**: Real-time bidirectional WebSocket telemetry bridge with 5 selectable sci-fi themes (`CYBER_CYAN`, `SOLAR_FLARE`, `MATRIX_NEON`, `VAPORWAVE_80S`, and `DEEP_SPACE`).

---

## Technical Specifications

| Parameter | Specification |
| :--- | :--- |
| **Main Processing Core** | Espressif ESP32-WROOM-32E (240 MHz Dual-Core Tensilica LX6, 4MB Flash, Wi-Fi & Classic BT) |
| **Audio Power Stage** | Texas Instruments TPA3116D2 Dual BTL Class-D ($2 \times 50\text{W}$ into $4\Omega$ @ $21\text{V}$) |
| **Synchronous Boost Stage** | Texas Instruments TPS61088RHLR ($3.7\text{V} \rightarrow 21.0\text{V}$, up to $10\text{A}$ switch current) |
| **Battery Fast Charging** | Injoinic IP2312 Synchronous Buck Converter ($3.0\text{A}$ CC/CV charging via USB-C) |
| **Battery Protection Matrix** | Fortune Semi DW01A-G + FS8205A Dual N-MOSFET ($2.5\text{V}$ UVP, $4.3\text{V}$ OVP, $6\text{A}$ OCP) |
| **Wi-Fi CSI Sensing** | 64-Subcarrier Channel State Information at 50Hz with UDP broadcast on port 5000 (RuView compatible) |
| **DSP Equalizer** | 10-Band Direct Form II Transposed Biquad Cascade ($31\text{Hz}$ – $16\text{kHz}$, $\pm 12\text{dB}$ range) |
| **USB-to-UART Bridge** | WCH CH340N with automatic DTR/RTS hardware bootloader circuit |
| **Power Inputs** | Single-Cell 3.7V Li-ion/LiPo (via JST-XH) or External DC Barrel Jack ($12\text{V}$–$21\text{V}$) |
| **Board Dimensions** | $100.0\text{ mm} \times 80.0\text{ mm}$ (4-Layer FR-4 Standard TG140, 1.6mm finished thickness) |

---

## Advanced Software & Firmware Capabilities

### 1. 10-Band Parametric Biquad Equalizer & Psychoacoustic DSP
- **Direct Form II Transposed Cascade**: 10 independent Biquad IIR filters calculated at 44.1 kHz sampling rate across $31\text{Hz}$, $62\text{Hz}$, $125\text{Hz}$, $250\text{Hz}$, $500\text{Hz}$, $1\text{kHz}$, $2\text{kHz}$, $4\text{kHz}$, $8\text{kHz}$, and $16\text{kHz}$.
- **Dynamic Sub-Bass Harmonics**: Synthesizes psychoacoustic virtual lower harmonics to produce deep perceptual bass from compact speaker enclosures.
- **3D Spatial Stereo Expander**: Phase-aligned stereo field widener ($0.0\times$ to $2.0\times$).
- **Dynamic Range Compressor (DRC)**: Prevents clipping and thermal overdriving at high volume levels.

### 2. Wi-Fi CSI Motion Sensing Radar & RuView 50Hz Live UDP Stream
- **64-Subcarrier Extraction**: Captures raw Wi-Fi Channel State Information (CSI) PHY packets.
- **Real-Time Spatial Disturbance Index**: Detects human movement, breathing, and presence through RF Doppler perturbation analysis.
- **RuView / PC UDP Streaming Engine**: Streams raw CSI frames over UDP to Port `5000` on any destination PC for real-time visualization in **RuView**, Python, or MATLAB.

### 3. Cyberpunk Web Command Station & Standalone PC Dashboard
- **Standalone PC Browser Interface**: Open `preview_ui.html` or `http://localhost:8000` directly in any web browser without needing to upload heavy web assets to the ESP32 flash memory.
- **Bidirectional WebSocket Device Bridge**: Connects live to the ESP32 board over WebSocket (`/ws`) to stream live hardware telemetry (`freeHeap`, `uptime`, `batteryVoltage`, amplifier state, 16-band audio spectrum, 64-subcarrier CSI radar).
- **5 Selectable Sci-Fi Themes**:
  - `CYBER_CYAN` (Electric Cyan, Violet, Hot Pink)
  - `SOLAR_FLARE` (Sunburst Amber, Flame Orange, Crimson)
  - `MATRIX_NEON` (Emerald Terminal, Toxic Lime, Laser Mint)
  - `VAPORWAVE_80S` (Neon Magenta, Deep Purple, Aqua)
  - `DEEP_SPACE` (Galactic Sapphire, Indigo, Nebula Violet)

---

## Hardware Pinout Reference

| Function | ESP32 GPIO Pin | Connection on AuraForge 50X |
| :--- | :--- | :--- |
| **Audio Left Channel (DAC 1)** | `GPIO 25` | AC-Coupled to TPA3116D2 `INPL` via `C24` |
| **Audio Right Channel (DAC 2)** | `GPIO 26` | AC-Coupled to TPA3116D2 `INPR` via `C25` |
| **Amp Shutdown / Mute (`SDZ`)** | `GPIO 22` | TPA3116D2 Pin 1 (`SDZ`) |
| **Amp Fault Monitoring (`FAULTZ`)** | `GPIO 23` | TPA3116D2 Pin 3 (`FAULTZ`) |
| **Status Indicator LED** | `GPIO 2` | Onboard blue/green activity LED |
| **UART Flashing / Debug** | `GPIO 1` (TX), `GPIO 3` (RX) | WCH CH340N USB-to-UART Bridge |

---

## Quick Start & Testing Guide

### Option 1: Live PC Dashboard & Simulation (Zero-Hardware)
1. Open [preview_ui.html](preview_ui.html) directly in Chrome, Edge, or Firefox (or visit `http://localhost:8000`).
2. Test all 5 Cyberpunk themes, interactive 10-band EQ canvas, DRC compressor, signal generator, and RuView CSI radar export!

### Option 2: Virtual Simulation in Wokwi (`wokwi_sim`)
1. In VS Code, press `Ctrl + Shift + P` -> **`Wokwi: Start Simulator`**.
2. Run the virtual ESP32 to test Wi-Fi AP (`AuraForge-50X-AP`), AsyncWebServer, DSP engine, and Serial CLI.

### Option 3: Flash to Physical ESP32 Hardware (`auraforge_50x`)
1. Connect the AuraForge 50X board via USB Type-C.
2. Build and upload using PlatformIO:
   ```bash
   cd firmware/
   pio run -e auraforge_50x -t upload
   pio device monitor
   ```
3. Connect your smartphone to Bluetooth audio device **`AuraForge 50X`** or Wi-Fi AP **`AuraForge-50X-AP`** (`http://192.168.4.1`)!

---

## License & Credits
Designed and engineered with passion by **Ravi Kachhwaha**. Released under the open-source **MIT License**.