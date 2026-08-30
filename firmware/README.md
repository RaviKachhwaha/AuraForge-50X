# AuraForge 50X — Programmable 50W Wireless Audio DSP & 50Hz Wi-Fi CSI Radar Platform

[![PlatformIO](https://img.shields.io/badge/PlatformIO-Core%20v6.1-orange.svg)](https://platformio.org)
[![Framework](https://img.shields.io/badge/Framework-ESP--IDF%20%2F%20Arduino-blue.svg)](https://espressif.com)
[![ESP32 Dual-Core](https://img.shields.io/badge/ESP32-240MHz%20Dual--Core%20LX6-red.svg)](https://espressif.com)
[![Wi-Fi CSI Radar](https://img.shields.io/badge/Wi--Fi%20CSI-RuView%2050Hz%20UDP-cyan.svg)]()
[![DSP Pipeline](https://img.shields.io/badge/Audio%20DSP-10--Band%20Biquad%20%2B%20FFT-brightgreen.svg)]()
[![Web UI](https://img.shields.io/badge/Web%20Dashboard-11--Tab%20Cyberpunk%20HUD-magenta.svg)]()
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

**AuraForge 50X** is a programmable embedded audio computing platform, real-time digital signal processing (DSP) workstation, and wireless RF motion sensing radar. Built on the dual-core **Espressif ESP32-WROOM-32E** microcontroller, it transforms a high-power $2 \times 50\text{W}$ Class-D amplifier into a fully connected, software-defined sound station with touchless room presence detection.

---

## System Architecture & Dual-Core Task Allocation

The firmware leverages FreeRTOS symmetric multiprocessing (SMP) across both 240 MHz Tensilica LX6 cores to maintain absolute isolation between hard real-time audio computation and asynchronous network telemetry:

```text
====================================================================================================
 CORE 0: Network, Telemetry & RF Sensing Engine (High-Throughput / Asynchronous)
====================================================================================================
 [Wi-Fi 802.11b/g/n PHY] ---> [Promiscuous CSI RX Callback] ---> [64-Subcarrier Variance Filter]
                                                                        |
                                                                        +---> [RuView UDP Stream (50Hz)]
                                                                        +---> [Presence State Machine]
 [AsyncWebServer (Port 80)] <---> [WebSocket Server (/ws)] <------------+---> [Hardware Telemetry (20Hz)]
 [Dual-Bank Web OTA Engine] <---> [ConfigManager (NVS Flash)]

====================================================================================================
 CORE 1: Hard Real-Time Audio DSP & Synthesis Pipeline (Deterministic / Zero-Jitter)
====================================================================================================
 [Bluetooth A2DP / Audio Gen] ---> [DMA Audio In Buffer (128 Samples @ 44.1 kHz)]
                                               |
  +--------------------------------------------+
  |
  v
 [10-Band Biquad IIR Transposed Direct Form II Cascade (31Hz - 16kHz)]
  |
  v
 [Dynamic Sub-Bass Psychoacoustic Harmonics Synthesizer (<60Hz)]
  |
  v
 [3D Spatial Stereo Phase Expander (0.0x - 2.0x)]
  |
  v
 [Dynamic Range Compressor (DRC) & Peak Soft-Limiter]
  |
  v
 [Closed-Loop Thermal & Battery Headroom Governor Multiplier]
  |
  +---> [In-Place Radix-2 128-Point FFT Spectrum Analyzer (60 FPS)] ---> [WebSocket JSON Buffer]
  |
  v
 [I2S DMA Output Ring Buffer] ---> [Internal High-Resolution DACs (GPIO25 / GPIO26)] ---> [TPA3116D2]
====================================================================================================

---

## 5 Advanced Pure-Firmware Subsystems

All 5 core systems are implemented purely in software on the physical ESP32 board without requiring external dedicated DSP hardware, external EEPROMs, or sensors:

```text
+--------------------------------------------------------------------------------------------------+
|                              5 ADVANCED PURE-FIRMWARE SUBSYSTEMS                                 |
+--------------------------------------------------------------------------------------------------+
| 1. NVS Flash Preset Engine      | Atomic non-volatile store for 10 factory + 4 custom user slots |
| 2. 128-Point Radix-2 FFT Engine | Real-time 60 FPS spectrum analyzer with 16 acoustic octave bands|
| 3. Dual-Bank Web OTA Updater    | Seamless in-browser .bin flashing across ota_0 and ota_1       |
| 4. Wi-Fi CSI Presence Radar     | 64-subcarrier Doppler variance tracking + 50Hz RuView UDP link  |
| 5. Closed-Loop Thermal Governor | On-die junction temp monitoring with 4-tier digital headroom   |
+--------------------------------------------------------------------------------------------------+

```

### 1. Non-Volatile Storage (NVS) Persistent Preset Manager (`ConfigManager`)

* **Flash Wear-Leveling Storage:** Uses the ESP32 `Preferences` API to persist volume levels, 10-band EQ gains, DRC parameters, Wi-Fi credentials, and active theme choices in SPI flash.
* **4 User Preset Banks (`UserPreset`):** Allows users to save, name, and recall custom acoustic profiles directly from the Web UI.
* **10 Built-In Factory Profiles:**
* `FLAT_STUDIO`: Linear reference response across all bands.
* `BASS_BOOST`: +6.0 dB low-shelf boost at 62 Hz with virtual sub-bass harmonics.
* `VOCAL_ENHANCE`: Mid-frequency elevation (+4.0 dB at 1 kHz–2 kHz) for speech clarity.
* `TREBLE_AIR`: Smooth high-shelf lift (+5.0 dB at 8 kHz–16 kHz) for crisp high-end detail.
* `CLUB_DANCE`: Deep V-curve contour with dynamic sub-bass enabled.
* `ROCK_ENERGY`: Elevated punch at 125 Hz and presence lift at 4 kHz.
* `ACOUSTIC_WARMTH`: Gentle low-mid saturation with flat highs.
* `ELECTRONIC_SYNTH`: Dynamic sub-bass coupled with stereo expansion ($1.5\times$).
* `GAMING_IMMERSION`: Wide stereo field ($1.85\times$) with enhanced transient response.
* `NIGHT_QUIET`: Compressed dynamic range with sub-60Hz frequencies cut to prevent room bleed.



### 2. High-Speed 128-Point Radix-2 Audio FFT Visualizer (`DspEngine`)

* **Cooley-Tukey In-Place Algorithm:** Computes a 128-point complex FFT on audio DMA buffer blocks at 60 FPS on Core 1.
* **Hann Window Conditioning:** Applies a 128-sample Hann window before FFT computation to suppress side-lobe leakage:

$$w[n] = 0.5 \times \left(1 - \cos\left(\frac{2\pi n}{N - 1}\right)\right)$$


* **16 Logarithmic Acoustic Bands:** Groups 64 raw frequency bins into 16 standardized acoustic center bands ($31\text{ Hz}$ to $16\text{ kHz}$) with exponential smoothing and peak-hold indicators.

### 3. Dual-Bank Asynchronous Web OTA Firmware Flashing (`WebServerModule`)

* **Partition Layout (`partitions.csv`):** Configured with two symmetrical 1.81 MB application partitions (`ota_0` and `ota_1`) and a 64 KB NVS partition:
```text
# Name,   Type, SubType, Offset,   Size,     Flags
nvs,      data, nvs,     0x9000,   0x10000,
otadata,  data, ota,     0x1f000,  0x2000,
app0,     app,  ota_0,   0x20000,  0x1D0000,
app1,     app,  ota_1,   0x1F0000, 0x1D0000,

```


* **Browser Drag-and-Drop Portal:** Supports drag-and-drop `.bin` file uploads via `/update` with real-time percentage progress bars, MD5 verification, and automatic rollback protection.

### 4. Wi-Fi CSI Spatial Presence State Machine & Touchless Automation (`CsiRadarEngine`)

* **64-Subcarrier Channel State Information (CSI) PHY Capture:** Intercepts 802.11n OFDM frames in promiscuous mode to extract raw In-phase ($I$) and Quadrature ($Q$) subcarrier matrices.
* **4-State Finite State Machine (FSM):**
```text
[ PRESENCE_VACANT ] --(Variance > Detect Threshold)--> [ PRESENCE_DETECTED ]
        ^                                                        |
        | (10 min Vacancy Timeout)                     (Sustained Activity)
        |                                                        v
[ PRESENCE_COOLDOWN ] <---(Activity Ceased for 15s)--- [ PRESENCE_ACTIVE ]

```


* **Touchless Room Automation:** Unmutes the amplifier when a person enters the room and enters power-saving standby mute after 10 minutes of vacancy.
* **RuView UDP Stream Engine:** Broadcasts formatted CSI frames at configurable rates (10 Hz, 25 Hz, 50 Hz, 100 Hz) to destination UDP Port `5000` on a host PC:
```text
CSI,<packet_index>,<rssi_dbm>,<motion_flag>,<variance>,<sub0>,<sub1>,...,<sub63>\n

```



### 5. Closed-Loop Thermal & Digital Headroom Governor (`HardwareMonitor`)

* **On-Die Silicon Junction Sensor:** Samples the internal ESP32 temperature sensor via `temperatureRead()` conditioned with an Exponential Moving Average (EMA) filter ($\alpha = 0.05$).
* **4-Tier Protection Ladder:**
* `GOVERNOR_NORMAL` ($<65^\circ\text{C}$): $0.0\text{ dB}$ (100% full dynamic range).
* `GOVERNOR_WARM` ($65^\circ\text{C} - 75^\circ\text{C}$): $-1.0\text{ dB}$ subtle master digital attenuation.
* `GOVERNOR_THROTTLED` ($75^\circ\text{C} - 85^\circ\text{C}$): $-3.0\text{ dB}$ attenuation + aggressive DRC ratio boost ($4:1$).
* `GOVERNOR_CRITICAL` ($>85^\circ\text{C}$): $-6.0\text{ dB}$ digital foldback to prevent hardware over-temperature shutdown.



---

## Unified Cyberpunk Web UI (11 Navigation Tabs)

The Web UI is identical byte-for-byte across:

* `preview_ui.html` (Standalone PC viewer with offline simulation)
* `index.html` (Workspace root)
* `src/WebUiAssets.h` (PROGMEM embedded header served directly by ESP32)

```text
+----------------------------------------------------------------------------------------------------+
|                                    AURA-FORGE 50X COMMAND HUD                                      |
+----------------------------------------------------------------------------------------------------+
| [1. Master] [2. 10-Band EQ] [3. Dynamics] [4. 16-Band FFT] [5. CSI Radar] [6. Thermal Governor]     |
| [7. Web OTA] [8. NVS Presets] [9. Cyber Themes] [10. Wi-Fi & AP] [11. Diagnostics & Telemetry]      |
+----------------------------------------------------------------------------------------------------+

```

### 11 Interactive Navigation Tabs

1. **Master Control HUD:** Radial rotary volume dial, pre-amp gain slider, balance control, Dynamic Sub-Bass toggles, 3D Spatial Expander width ($0.0\times - 2.0\times$), 10 factory preset buttons, and 4 quick user preset buttons.
2. **10-Band Equalizer Canvas:** Interactive Bézier frequency response curve with 10 vertical neon faders ($-12\text{ dB}$ to $+12\text{ dB}$ across $31\text{ Hz}$ to $16\text{ kHz}$).
3. **Compressor & Crossover:** Dynamic Range Compressor (DRC) threshold/ratio sliders, real-time gain reduction meter bar, and tunable subsonic high-pass and subwoofer low-pass filters.
4. **16-Band FFT & Signal Gen:** Real-time 16-band spectrum visualizer with peak hold + DSP test tone generator (Sine, Square, Pink Noise).
5. **Wi-Fi CSI Presence & RuView:** 64-Subcarrier spectrogram canvas, 4-state presence badge, variance meter, target PC IP/Port inputs, and a manual "Transmit Test UDP Packet" button.
6. **Thermal & Digital Governor:** Live junction temperature gauge (°C / °F), 4-tier governor state indicator, digital attenuation meter, and an interactive test temperature simulator slider.
7. **Dual-Bank Web OTA:** Active partition indicator (`ota_0` vs `ota_1`), drag-and-drop `.bin` file uploader, and live upload progress bar.
8. **NVS Preset Manager:** Slot list for 4 custom user presets with Load, Save Active EQ, and Rename functions.
9. **Cyber Themes & Power:** 8 selectable themes and CPU clock scaling selector (80 MHz, 160 MHz, 240 MHz).
10. **Wi-Fi & Network:** SoftAP and Station credential management, IP address display, and live Wi-Fi site survey scanner.
11. **Diagnostics & Telemetry:** TPA3116D2 fault monitor, battery fuel gauge, free heap memory monitor, uptime counter, and reboot/factory reset controls.

### 8 Selectable Cyberpunk Themes

| Theme ID | Primary Glow Accent | Secondary Accent | Background Gradient |
| --- | --- | --- | --- |
| `CYBER_CYAN` | Electric Cyan (`#00f2fe`) | Neon Violet (`#7f00ff`) | Deep Slate Dark (`#0a0e17`) |
| `SOLAR_FLARE` | Sunburst Amber (`#ffb703`) | Flame Orange (`#ff7518`) | Obsidian Charcoal (`#120c08`) |
| `MATRIX_NEON` | Cyber Mint (`#00f5d4`) | Phosphor Green (`#39ff14`) | Terminal Black (`#050d08`) |
| `VAPORWAVE_80S` | Neon Magenta (`#f72585`) | Retro Indigo (`#7209b7`) | Synthwave Night (`#120818`) |
| `DEEP_SPACE` | Galactic Sapphire (`#38bdf8`) | Nebula Purple (`#c084fc`) | Deep Void Navy (`#060a12`) |
| `TOKYO_DRIFT` | Sakura Hot Pink (`#ff007f`) | Turquoise (`#00f5d4`) | Midnight Cyber (`#0e0714`) |
| `STEALTH_OLED` | Titanium Ice (`#e2e8f0`) | Carbon Silver (`#94a3b8`) | Pure Pitch Black (`#000000`) |
| `CRIMSON_CORE` | Blood Ruby (`#ff0055`) | Blaze Orange (`#ff2a2a`) | Volcanic Dark (`#140608`) |

---

## Hardware Pinout Reference

| Function | ESP32 GPIO Pin | Physical Connection on AuraForge 50X PCB |
| --- | --- | --- |
| **Audio Left Channel (DAC 1)** | `GPIO 25` | AC-coupled to TPA3116D2 `INPL` via capacitor `C24` ($1.0\mu\text{F}$) |
| **Audio Right Channel (DAC 2)** | `GPIO 26` | AC-coupled to TPA3116D2 `INPR` via capacitor `C25` ($1.0\mu\text{F}$) |
| **Amplifier Mute / Shutdown (`SDZ`)** | `GPIO 22` | Direct gate connection to TPA3116D2 Pin 1 (`SDZ`) |
| **Amplifier Fault Alert (`FAULTZ`)** | `GPIO 23` | Active-low open-drain monitor on TPA3116D2 Pin 3 (`FAULTZ`) |
| **Status Indicator LED** | `GPIO 2` | Onboard SMT blue/green activity indicator |
| **UART Flashing / Debug** | `GPIO 1` (TX), `GPIO 3` (RX) | WCH CH340N USB-to-UART bridge connected to USB Type-C |
| **Hardware Boot / Reset** | `IO0` (`SW_BOOT1`), `EN` (`SW_RST1`) | Dual tactile pushbuttons with $10\text{k}\Omega$ pull-up network |

---

## RuView & Python CSI UDP Integration

AuraForge 50X streams raw 64-subcarrier CSI packets directly to any host computer over UDP Port `5000`.

### Python Real-Time Ingestion Script

```python
import socket
import numpy as np

UDP_IP = "0.0.0.0"
UDP_PORT = 5000

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

print(f"[*] AuraForge 50X CSI UDP Stream Listening on {UDP_IP}:{UDP_PORT}...")

while True:
    data, addr = sock.recvfrom(2048)
    line = data.decode('utf-8', errors='ignore').strip()
    
    if line.startswith("CSI,"):
        parts = line.split(",")
        pkt_cnt = int(parts[1])
        rssi = int(parts[2])
        motion = int(parts[3])
        variance = float(parts[4])
        subcarriers = np.array([float(x) for x in parts])
        
        print(f"[Packet #{pkt_cnt:06d}] RSSI: {rssi:3d} dBm | Motion: {motion} | Variance: {variance:6.2f} | Subcarriers: {len(subcarriers)}")

```

---

## Serial Command Line Interface (CLI)

Connect via serial monitor at **115200 baud**:

| Command | Arguments | Description |
| --- | --- | --- |
| `help` | — | Displays the full list of available firmware commands |
| `status` | — | Prints CPU usage, free heap, battery voltage, and junction temperature |
| `volume` | `<0-100>` | Sets master audio volume percentage |
| `eq` | `<band:0-9>` `<gain:-12 to +12>` | Adjusts a specific equalizer band gain in decibels |
| `preset` | `<0-9>` | Loads a factory acoustic preset |
| `userpreset` | `save <1-4>` / `load <1-4>` | Saves or loads a custom user profile to/from NVS flash |
| `csi` | `on` / `off` / `rate <10/25/50/100>` | Controls the Wi-Fi CSI spatial sensing radar and stream rate |
| `csi_target` | `<ip>` `<port>` | Sets destination PC IP address and UDP port for RuView streaming |
| `theme` | `<0-7>` | Changes active Cyberpunk HUD theme across all connected clients |
| `test_udp` | — | Transmits a test CSI packet to the configured destination PC |
| `reboot` | — | Restarts the ESP32 processing core |
| `reset` | — | Clears NVS flash and restores all factory configurations |

---

## Quick Start & Flashing Guide

### 1. Standalone PC Browser Preview (Zero-Hardware Simulation)

1. Open `preview_ui.html` directly in Chrome, Edge, or Firefox (or start a local server: `python -m http.server 8000`).
2. Test all 11 tabs, 8 themes, animated 16-band audio spectrums, and simulated CSI radar data offline.

### 2. PlatformIO Build & Physical Hardware Upload

1. Connect the AuraForge 50X board to your computer via USB Type-C.
2. Open the project in **Visual Studio Code** with the **PlatformIO IDE** extension.
3. Build and flash the firmware:
```bash
# Navigate to firmware directory
cd firmware/

# Compile and flash to physical hardware
pio run -e auraforge_50x -t upload

# Open interactive serial monitor
pio device monitor

```



### 3. Connect to Audio & Web HUD

1. **Bluetooth Audio:** Pair your smartphone or computer with Bluetooth device **`AuraForge 50X`**.
2. **Wi-Fi Web HUD:** Connect to Wi-Fi access point **`AuraForge-50X-AP`** (Password: `auraforge50x`) and open **`http://192.168.4.1`** in any browser.
3. **PC Remote Control:** Open `preview_ui.html` on your PC, type `192.168.4.1` (or the board's LAN IP) into the top bridge bar, and click **`CONNECT LIVE`**.

---

## License & Author Credits

* **Hardware Design:** Licensed under the [CERN-OHL-P v2](https://spdx.org/licenses/CERN-OHL-P-2.0.html).
* **Firmware & Web HUD:** Licensed under the [MIT License](https://www.google.com/search?q=LICENSE).
* **Author & Lead Engineer:** **Ravi Kachhwaha**

