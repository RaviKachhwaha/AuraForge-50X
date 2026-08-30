# AuraForge 50X — Programmable 50W High-Fidelity Wireless DSP & CSI Sensing Platform

[![KiCad 10](https://img.shields.io/badge/EDA-KiCad%2010.0-blue.svg)](https://kicad.org)
[![PCB Layers](https://img.shields.io/badge/PCB%20Layers-4--Layer%20Stackup-orange.svg)]()
[![DRC Status](https://img.shields.io/badge/DRC%20Status-Passed%20(0%20Errors)-brightgreen.svg)]()
[![ESP32 Dual-Core](https://img.shields.io/badge/ESP32-240MHz%20Dual--Core%20LX6-red.svg)](https://espressif.com)
[![Wi-Fi CSI Radar](https://img.shields.io/badge/Wi--Fi%20CSI-RuView%2050Hz%20UDP-cyan.svg)]()
[![DSP Pipeline](https://img.shields.io/badge/Audio%20DSP-10--Band%20Biquad%20%2B%20FFT-brightgreen.svg)]()
[![Web UI](https://img.shields.io/badge/Web%20Dashboard-11--Tab%20Cyberpunk%20HUD-magenta.svg)]()
[![Hack Club](https://img.shields.io/badge/Hack%20Club-Forge%20Grant-red.svg)](https://hackclub.com)
[![License](https://img.shields.io/badge/License-MIT%20%2F%20CERN--OHL--P%20v2-green.svg)](LICENSE)

**AuraForge 50X** is an open-source, programmable 4-layer wireless audio computer, real-time digital signal processing (DSP) workstation, and wireless RF motion-sensing radar. Built around the dual-core **Espressif ESP32-WROOM-32E** microcontroller, AuraForge 50X transforms a high-efficiency Class-D stereo amplifier into a fully connected, software-defined sound station equipped with non-intrusive room presence detection.

The platform integrates:
1. **High-Power Class-D Audio Stage**: Texas Instruments **TPA3116D2** dual BTL stereo amplifier delivering up to $2 \times 50\text{W}$ into $4\Omega$ at $21.0\text{V}$.
2. **Synchronous 10A Boost Power Converter**: Texas Instruments **TPS61088** stepping a single-cell 3.7V lithium battery up to 21.0V with integrated soft-start anti-pop sequencing.
3. **Advanced Digital Signal Processing (DSP)**: 10-Band Parametric Biquad IIR Equalizer, Dynamic Sub-Bass Psychoacoustic Harmonics, 3D Spatial Stereo Expander, and Dynamic Range Compressor (DRC).
4. **Wi-Fi Channel State Information (CSI) Motion Sensing Radar**: 64-subcarrier spatial disturbance sensing engine streaming live at 50 Hz over UDP (Port 5000) directly to PC telemetry tools (**RuView**, Python, MATLAB).
5. **Cyberpunk Web Command Station**: Real-time bidirectional WebSocket telemetry bridge with 11 navigation tabs, 8 selectable sci-fi themes, and standalone zero-hardware PC simulation.

---

## Visual Showcase & Design Verification

### 3D Top View (Assembled SMT Rendering & Overhanging Antenna)
![AuraForge 50X 3D Top View](docs/images/3d_top_render.png)

### 3D Bottom View (Thermal Dissipation Plane & Thermal Vias)
![AuraForge 50X 3D Bottom View](docs/images/3d_bottom_render.png)

### 2D PCB Layout (4-Layer High-Current Power & Ground Routing)
![AuraForge 50X PCB Layout](docs/images/pcb_layout_2d.png)

### Full Schematic Architecture
![AuraForge 50X Schematic](docs/images/schematic_preview.png)

### Design Rules Check (KiCad 10 DRC: 0 Errors / 0 Unconnected Verified)
![KiCad 10 DRC 0 Errors Report](docs/images/drc_zero_errors.png)

---

## Technical Specifications

| Parameter | Specification |
| :--- | :--- |
| **Main Processing Core** | Espressif ESP32-WROOM-32E (240 MHz Dual-Core Tensilica LX6, 4MB Flash, Wi-Fi & Classic BT) |
| **Audio Power Stage** | Texas Instruments TPA3116D2 Dual BTL Class-D ($2 \times 50\text{W}$ into $4\Omega$ @ $21\text{V}$) |
| **Synchronous Boost Stage** | Texas Instruments TPS61088RHLR ($3.7\text{V} \rightarrow 21.0\text{V}$, up to $10\text{A}$ switch current) |
| **Battery Fast Charging** | Injoinic IP2312 Synchronous Buck Converter ($3.0\text{A}$ CC/CV charging via USB-C) |
| **Battery Protection Matrix** | Fortune Semi DW01 + FS8205A Dual N-MOSFET ($2.5\text{V}$ UVP, $4.3\text{V}$ OVP, $6\text{A}$ OCP) |
| **Wi-Fi CSI Sensing** | 64-Subcarrier Channel State Information at 50Hz with UDP broadcast on port 5000 (RuView compatible) |
| **DSP Equalizer** | 10-Band Direct Form II Transposed Biquad Cascade ($31\text{Hz}$ – $16\text{kHz}$, $\pm 12\text{dB}$ range) |
| **USB-to-UART Bridge** | WCH CH340N with dedicated manual hardware pushbuttons (`SW_BOOT1` & `SW_RST1`) |
| **Power Inputs** | Single-Cell 3.7V Li-ion/LiPo (via JST-XH) or External DC Barrel Jack ($12\text{V}$–$21\text{V}$) |
| **Board Dimensions** | $100.0\text{ mm} \times 80.0\text{ mm}$ (4-Layer FR-4 Standard TG140, 1.6mm finished thickness) |

---

## Grant Budget Justification (Turnkey PCBA Prototype Run)

Unlike simple low-power microcontroller breakouts or basic 2-layer sensor boards, the AuraForge 50X is a high-density, multi-rail audio computing platform combining an 83-component Bill of Materials with high-power switching stages. Sourcing the hardware and manufacturing requires an industrial 4-layer PCB fabrication process with solid ground plane isolation, automated SMT pick-and-place assembly, battery power, and acoustic test transducers.

### Manufacturing & Batch Quantity Modeling (3 Boards)
On turnkey PCB assembly portals (such as **pcbpower.com**), the minimum order quantity (MOQ) for automated SMT assembly setup is 2 units. Fixed manufacturing costs—such as laser-cut stainless steel solder stencils, pick-and-place feeder tooling, automated optical inspection (AOI), and 4-layer photolithography setup—amount to approximately **₹10,500 INR (~$110.10 USD)** for the initial setup.

Because fixed SMT setup fees are already covered, **scaling from 2 boards to 3 boards costs only ~₹1,300 INR ($13.63 USD) more in assembly labor**, making a 3-board prototype run the most cost-effective approach for hardware bring-up, firmware testing, and destructive power stage verification.

### Turnkey Budget Breakdown (Converted at ₹95.37 = $1.00 USD)

| Item # | Category | Description | Qty | Unit Price (₹ / $) | Total Cost (INR ₹) | Total Cost (USD $) | Sourcing / File Link |
| :---: | :--- | :--- | :---: | :---: | :---: | :---: | :--- |
| **1** | **Turnkey PCB & PCBA** | 3 units 4-Layer Fab + Automated SMT Assembly (Invoice Net Total incl. GST) | 1 | ₹10,617.00 ($111.32) | ₹10,617.00 | $111.32 | [pcbpower.com](https://www.pcbpower.com/) |
| **2** | **Components Sourcing** | Component BOM sourcing package on pcbpower (1.5× volume factor) | 1 | ₹5,519.84 ($57.88) | ₹5,519.84 | $57.88 | [`components_bom.csv`](components_bom.csv) |
| **3** | **Battery Power** | 18650 3.7V 3000mAh Li-ion Battery with JST-XH Lead & BMS Protection | 1 | ₹580.00 ($6.08) | ₹580.00 | $6.08 | [Amazon India](https://www.amazon.in/Charging-18650-Rechargeable-Connector-Protection/dp/B0DBR36C91) |
| **4** | **Acoustic Speakers** | Electronic Spices 5" $4\Omega$ 25W Full-Range Woofer Drivers (Stereo Pair) | 2 | ₹389.00 ($4.08) | ₹778.00 | $8.16 | [Amazon India](https://www.amazon.in/-/hi/Electronic-Spices-%E0%A4%95%E0%A5%88%E0%A4%AC%E0%A4%BF%E0%A4%A8%E0%A5%87%E0%A4%9F-%E0%A4%B0%E0%A4%BF%E0%A4%AA%E0%A5%8D%E0%A4%B2%E0%A5%87%E0%A4%B8%E0%A4%AE%E0%A5%87%E0%A4%82%E0%A4%9F-%E0%A4%A1%E0%A5%8D%E0%A4%B0%E0%A4%BE%E0%A4%87%E0%A4%B5%E0%A4%B0/dp/B0BN44KVVL) |
| **5** | **Flashing & Power** | Anker PowerLine III Flow 100W USB-C to USB-C Silicone Cable (3ft / 1.0m) | 1 | ₹786.00 ($8.24) | ₹786.00 | $8.24 | [Amazon India](https://www.amazon.in/-/hi/Anker-A8552/dp/B093GGVB89) |
| **—** | **TOTAL** | **Complete Prototype Kit (3 PCBs + PCBA Assembly + Components + Peripherals)** | **—** | **—** | **₹18,280.84** | **$191.68** | [`bom.csv`](bom.csv) |

> **Budget Scalability Note ($165 – $175 Minimum Target):** The table above represents the upper-bound buffer with a 1.5× component sourcing package. The actual grant expenditure scales down directly to **$165.00 – $175.00 USD (₹15,736 – ₹16,690 INR)** by procuring a single fully populated board BOM (1.0× baseline = ₹3,679.89 / $38.59, bringing the total to **$172.39 USD / ₹16,440.89 INR**) or through volume consolidation discounts finalized during the pcbpower.com post-order BOM engineering review.
>
> **Itemized Components Breakdown:** For the complete individual component list (resistors, capacitors, TI TPA3116D2, TI TPS61088, ESP32-WROOM-32E) and exact manufacturer part numbers (MPNs), refer to [`components_bom.csv`](components_bom.csv).

---

## 4-Layer PCB Stackup Architecture

Designed strictly to IPC-2152 and IPC-2221 Class 2 thermal and high-current standards:

```text
==================================================================================
 Layer 1 (F.Cu)   : High-Speed Differential Audio Signals, 1.8mm Power Distribution
----------------------------------------------------------------------------------
 Layer 2 (In1.Cu) : Continuous Solid Ground Plane (Zero-Impedance Return & Heat Sink)
----------------------------------------------------------------------------------
 Layer 3 (In2.Cu) : Split Power Routing (+3.3V Logic, +BAT Power Rail, Sub-Systems)
----------------------------------------------------------------------------------
 Layer 4 (B.Cu)   : Signal Jumpers, Escape Tracks & Secondary Ground Dissipation Pour
==================================================================================

---

## Hardware Subsystem Architecture

```text
                    +--------------------------------------------+
                    |               USB Type-C (J1)              |
                    +---------+------------------------+---------+
                              | 5V VBUS                | USB D+ / D-
                              v                        v
                    +-------------------+    +-------------------+
                    |    IP2312 (U3)    |    |   CH340N (U_UART1)|
                    |  3A Buck Charger  |    |    USB-to-UART    |
                    +---------+---------+    +---------+---------+
                              |                        | TX / RX (UART)
                              v                        v
+----------------+  +-------------------+    +-------------------+  <-- SW_BOOT1 (IO0)
| 3.7V Li-ion    |->|   DW01 + FS8205A  |    | ESP32-WROOM-32E   |  <-- SW_RST1  (EN)
| Battery (J6)   |  | Protection (U5/U6)|    | MCU & DSP Core(U4)|
+----------------+  +---------+---------+    +---------+---------+
                              |                        |
                              +-------> +BAT_3V7 ------+ (via NCP1117 / AMS1117 3.3V)
                                           |           | I2S / DAC Audio
                                           v           v
                                    +--------------+   |
                                    | TPS61088(U2) |   |
                                    |  10A Boost   |   |
                                    +------+-------+   |
                                           | +21V_BOOST|
                                           v           v
                                    +--------------------+
                                    |   TPA3116D2 (U1)   |
                                    | 2x50W Class-D Stage|
                                    +----+----------+----+
                                         |          |
                                         v          v
                                     Speaker L  Speaker R
                                       (J4)       (J5)

```

---

## Hardware Pinout Reference

| Function | ESP32 GPIO Pin | Connection on AuraForge 50X |
| --- | --- | --- |
| **Audio Left Channel (DAC 1)** | `GPIO 25` | AC-Coupled to TPA3116D2 `INPL` via capacitor `C24` ($1.0\mu\text{F}$) |
| **Audio Right Channel (DAC 2)** | `GPIO 26` | AC-Coupled to TPA3116D2 `INPR` via capacitor `C25` ($1.0\mu\text{F}$) |
| **Amp Shutdown / Mute (`SDZ`)** | `GPIO 22` | Direct gate connection to TPA3116D2 Pin 1 (`SDZ`) |
| **Amp Fault Monitoring (`FAULTZ`)** | `GPIO 23` | Active-low open-drain monitor on TPA3116D2 Pin 3 (`FAULTZ`) |
| **Status Indicator LED** | `GPIO 2` | Onboard SMT blue/green activity indicator |
| **UART Flashing / Debug** | `GPIO 1` (TX), `GPIO 3` (RX) | WCH CH340N USB-to-UART Bridge connected to USB Type-C |
| **Hardware Boot / Reset** | `IO0` (`SW_BOOT1`), `EN` (`SW_RST1`) | Dual tactile pushbuttons with $10\text{k}\Omega$ pull-up network |

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
* **10 Built-In Factory Profiles:** `FLAT_STUDIO`, `BASS_BOOST`, `VOCAL_ENHANCE`, `TREBLE_AIR`, `CLUB_DANCE`, `ROCK_ENERGY`, `ACOUSTIC_WARMTH`, `ELECTRONIC_SYNTH`, `GAMING_IMMERSION`, and `NIGHT_QUIET`.

### 2. High-Speed 128-Point Radix-2 Audio FFT Visualizer (`DspEngine`)

* **Cooley-Tukey In-Place Algorithm:** Computes a 128-point complex FFT on audio DMA buffer blocks at 60 FPS on Core 1.
* **Hann Window Conditioning:** Applies a 128-sample Hann window before FFT computation to suppress side-lobe leakage.
* **16 Logarithmic Acoustic Bands:** Groups 64 raw frequency bins into 16 standardized acoustic center bands ($31\text{ Hz}$ to $16\text{ kHz}$) with exponential smoothing and peak-hold indicators.

### 3. Dual-Bank Asynchronous Web OTA Firmware Flashing (`WebServerModule`)

* **Partition Layout (`partitions.csv`):** Configured with two symmetrical 1.81 MB application partitions (`ota_0` and `ota_1`) and a 64 KB NVS partition.
* **Browser Drag-and-Drop Portal:** Supports drag-and-drop `.bin` file uploads via `/update` with real-time percentage progress bars, MD5 verification, and automatic rollback protection.

### 4. Wi-Fi CSI Spatial Presence State Machine & Touchless Automation (`CsiRadarEngine`)

* **64-Subcarrier Channel State Information (CSI) PHY Capture:** Intercepts 802.11n OFDM frames in promiscuous mode to extract raw In-phase ($I$) and Quadrature ($Q$) subcarrier matrices.
* **4-State Finite State Machine (FSM):**
`PRESENCE_VACANT` → `PRESENCE_DETECTED` → `PRESENCE_ACTIVE` → `PRESENCE_COOLDOWN`
* **Touchless Room Automation:** Unmutes the amplifier when a person enters the room and enters power-saving standby mute after 10 minutes of vacancy.
* **RuView UDP Stream Engine:** Broadcasts formatted CSI frames at configurable rates (10 Hz, 25 Hz, 50 Hz, 100 Hz) to destination UDP Port `5000` on a host PC.

### 5. Closed-Loop Thermal & Digital Headroom Governor (`HardwareMonitor`)

* **On-Die Silicon Junction Sensor:** Samples the internal ESP32 temperature sensor via `temperatureRead()` conditioned with an Exponential Moving Average (EMA) filter ($\alpha = 0.05$).
* **4-Tier Protection Ladder:**
* `GOVERNOR_NORMAL` ($<65^\circ\text{C}$): $0.0\text{ dB}$ (100% full dynamic range).
* `GOVERNOR_WARM` ($65^\circ\text{C} - 75^\circ\text{C}$): $-1.0\text{ dB}$ subtle master digital attenuation.
* `GOVERNOR_THROTTLED` ($75^\circ\text{C} - 85^\circ\text{C}$): $-3.0\text{ dB}$ attenuation + aggressive DRC ratio boost ($4:1$).
* `GOVERNOR_CRITICAL` ($>85^\circ\text{C}$): $-6.0\text{ dB}$ digital foldback to prevent hardware over-temperature shutdown.



---

## Unified Cyberpunk Web UI (11 Navigation Tabs)

The Web UI is identical byte-for-byte across `preview_ui.html` (PC Standalone Viewer), `index.html` (Source), and `src/WebUiAssets.h` (PROGMEM embedded).

```text
+----------------------------------------------------------------------------------------------------+
|                                    AURA-FORGE 50X COMMAND HUD                                      |
+----------------------------------------------------------------------------------------------------+
| [1. Master] [2. 10-Band EQ] [3. Dynamics] [4. 16-Band FFT] [5. CSI Radar] [6. Thermal Governor]     |
| [7. Web OTA] [8. NVS Presets] [9. Cyber Themes] [10. Wi-Fi & AP] [11. Diagnostics & Telemetry]      |
+----------------------------------------------------------------------------------------------------+

```

### 11 Interactive Navigation Tabs

1. **Master Control HUD:** Radial rotary volume dial, pre-amp gain, balance, Dynamic Sub-Bass, 3D Spatial Expander width ($0.0\times - 2.0\times$), 10 factory presets, and 4 quick user preset buttons.
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

## Bill of Materials (BOM)

The board consists of **83 total components** consolidated into **45 unique manufacturer part numbers (MPNs)**.
All pricing is based on live distributor catalog rates (DigiKey & LCSC) converted at **₹95.37 = $1.00 USD**.

| Reference | Qty (1x) | Qty (3x) | Value | Package / Footprint | Manufacturer | Manufacturer Part Number (MPN) | Unit Price (1x) | Total Price (3x) |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| `C1` | 1 | 3 | 10uF 25V | 0805 | TDK | `C2012X7S1E106K125AC` | ₹56.37 ($0.59) | ₹169.11 ($1.77) |
| `C2, C9, C10` | 3 | 9 | 22uF 10V | 0805 | Yageo | `CC0805MKX5R6BB226` | ₹44.91 ($0.47) | ₹404.19 ($4.24) |
| `C3, C7, C8, C37` | 4 | 12 | 100nF 16V | 0603 | Yageo | `CC0603KRX7R7BB104` | ₹9.56 ($0.10) | ₹114.72 ($1.20) |
| `C4` | 1 | 3 | 10uF 16V | 0805 | Yageo | `CC0805KRX5R7BB106` | ₹60.20 ($0.63) | ₹180.60 ($1.89) |
| `C5` | 1 | 3 | 22uF 6.3V | 0805 | Yageo | `CC0805MKX5R5BB226` | ₹40.13 ($0.42) | ₹120.39 ($1.26) |
| `C6` | 1 | 3 | 10uF 10V | 0805 | Yageo | `CC0805KRX5R6BB106` | ₹51.60 ($0.54) | ₹154.80 ($1.62) |
| `C11, C24-C28` | 6 | 18 | 1uF 16V | 0603 | Yageo | `CC0603KRX7R7BB105` | ₹16.24 ($0.17) | ₹292.32 ($3.07) |
| `C12` | 1 | 3 | 100nF 25V | 0603 | Yageo | `CC0603KRX7R8BB104` | ₹9.56 ($0.10) | ₹28.68 ($0.30) |
| `C13` | 1 | 3 | 10nF 25V | 0603 | Yageo | `CC0603KRX7R8BB103` | ₹9.56 ($0.10) | ₹28.68 ($0.30) |
| `C14` | 1 | 3 | 2.2nF 25V | 0603 | Yageo | `CC0603KRX7R8BB222` | ₹9.56 ($0.10) | ₹28.68 ($0.30) |
| `C15, C16, C17` | 3 | 9 | 22uF 25V | 1206 | Samsung | `CL31A226KAHNNNE` | ₹36.31 ($0.38) | ₹326.79 ($3.43) |
| `C18` | 1 | 3 | 220uF 25V | Radial 8x10.5 | Würth Elektronik | `865060453007` | ₹69.75 ($0.73) | ₹209.25 ($2.19) |
| `C19-C23` | 5 | 15 | 1uF 25V | 0603 | Yageo | `CC0603KPX7R8BB105` | ₹9.62 ($0.10) | ₹144.30 ($1.51) |
| `C29-C32` | 4 | 12 | 0.22uF 25V | 0603 | Yageo | `CC0603KRX7R8BB224` | ₹15.29 ($0.16) | ₹183.48 ($1.92) |
| `C33-C36` | 4 | 12 | 0.68uF 25V | 0805 | Yageo | `CC0805KKX7R9BB684` | ₹27.71 ($0.29) | ₹332.52 ($3.49) |
| `D1, D2, D3` | 3 | 9 | SS34 (3A 40V) | SMA | onsemi | `MBRA340T3G` | ₹90.77 ($0.95) | ₹816.93 ($8.57) |
| `J1` | 1 | 3 | TYPE-C-31-M-12 | SMD + PTH Tabs | GCT | `USB4105-GF-A` | ₹76.44 ($0.80) | ₹229.32 ($2.40) |
| `J2` | 1 | 3 | PJ-002AH | 5.5x2.1mm PTH | CUI Devices | `PJ-002AH` | ₹67.84 ($0.71) | ₹203.52 ($2.13) |
| `J3` | 1 | 3 | PJ-320A | 3.5mm Stereo PTH | CUI Devices | `SJ-3524-SMT-TR` | ₹85.04 ($0.89) | ₹255.12 ($2.68) |
| `J4, J5` | 2 | 6 | Speaker Terminal | 5.08mm Screw PTH | Phoenix Contact | `1715721` | ₹110.84 ($1.16) | ₹665.04 ($6.97) |
| `J6` | 1 | 3 | B2B-XH-A | 2.5mm JST-XH PTH | JST | `B2B-XH-A(LF)(SN)` | ₹9.56 ($0.10) | ₹28.68 ($0.30) |
| `L1` | 1 | 3 | 2.2uH 6.5A | SMD 7.3x7.3 | Sunlord | `MWSA0603S-2R2MT` | ₹93.64 ($0.98) | ₹280.92 ($2.95) |
| `L2` | 1 | 3 | 2.2uH 10.0A | SMD 10.4x10.4 | Wurth Elektronik | `7443340220` | ₹204.48 ($2.14) | ₹613.44 ($6.43) |
| `L3, L4, L5, L6` | 4 | 12 | 10uH 3.8A | SMD 8.0x8.0 | Taiyo Yuden | `NRS8040T100MJGJ` | ₹33.44 ($0.35) | ₹401.28 ($4.21) |
| `LED1` | 1 | 3 | Red LED (Charge) | 0603 | Kingbright | `APT1608SURCK` | ₹20.06 ($0.21) | ₹60.18 ($0.63) |
| `LED2` | 1 | 3 | Green LED (Done) | 0603 | Kingbright | `APT1608ZGC` | ₹60.20 ($0.63) | ₹180.60 ($1.89) |
| `R1, R2` | 2 | 6 | 5.1k 1% | 0603 | Yageo | `RC0603FR-135K1L` | ₹9.56 ($0.10) | ₹57.36 ($0.60) |
| `R3` | 1 | 3 | 45.3k 1% | 0603 | Yageo | `RC0603FR-0745K3L` | ₹9.56 ($0.10) | ₹28.68 ($0.30) |
| `R4` | 1 | 3 | 51k 1% | 0603 | Yageo | `RC0603FR-1351KL` | ₹9.56 ($0.10) | ₹28.68 ($0.30) |
| `R5, R6, R17, R18` | 4 | 12 | 1k 1% | 0603 | Yageo | `RC0603FR-071KL` | ₹9.56 ($0.10) | ₹114.72 ($1.20) |
| `R7, R8, R10, R11` | 4 | 12 | 10k 1% | 0603 | Yageo | `RC0603FR-0710KL` | ₹9.56 ($0.10) | ₹114.72 ($1.20) |
| `R9` | 1 | 3 | 165k 1% | 0603 | Yageo | `RC0603FR-07165KL` | ₹9.56 ($0.10) | ₹28.68 ($0.30) |
| `R12, R14, R15` | 3 | 9 | 100k 1% | 0603 | Yageo | `RC0603FR-07100KL` | ₹9.56 ($0.10) | ₹86.04 ($0.90) |
| `R13` | 1 | 3 | 120k 1% | 0603 | Yageo | `RC0603FR-07120KL` | ₹9.56 ($0.10) | ₹28.68 ($0.30) |
| `R16` | 1 | 3 | 100R 1% | 0603 | Yageo | `RC0603FR-07100RL` | ₹10.51 ($0.11) | ₹31.53 ($0.33) |
| `R19` | 1 | 3 | 10R 1% | 0603 | Yageo | `RC0603FR-1310RL` | ₹9.56 ($0.10) | ₹28.68 ($0.30) |
| `SW_BOOT1, SW_RST1` | 2 | 6 | SMT Tact Switch | SMD 6.0x3.5 | C&K | `PTS645SL43SMTR92LFS` | ₹37.26 ($0.39) | ₹223.56 ($2.34) |
| `U1` | 1 | 3 | TPA3116D2 Amp | HTSSOP-32 | Texas Instruments | `TPA3116D2DADR` | ₹304.80 ($3.20) | ₹914.40 ($9.59) |
| `U2` | 1 | 3 | TPS61088 Boost | VQFN-20 | Texas Instruments | `TPS61088RHLR` | ₹261.81 ($2.75) | ₹785.43 ($8.24) |
| `U3` | 1 | 3 | IP2312 Charger | ESOP-8 | Injoinic | `IP2312` | ₹31.49 ($0.33) | ₹94.47 ($0.99) |
| `U4` | 1 | 3 | ESP32-WROOM-32E | SMD Module | Espressif Systems | `ESP32-WROOM-32E-N4` | ₹476.79 ($5.00) | ₹1,430.37 ($15.00) |
| `U5` | 1 | 3 | Battery Protect | SOT-23-6 | evvo | `DW01` | ₹10.51 ($0.11) | ₹31.53 ($0.33) |
| `U6` | 1 | 3 | Dual N-MOSFET | TSSOP-8 | Fortune Semi | `FS8205A` | ₹62.11 ($0.65) | ₹186.33 ($1.95) |
| `U7` | 1 | 3 | 3.3V 1A LDO | SOT-223 | onsemi | `NCP1117ST33T3G` | ₹59.24 ($0.62) | ₹177.72 ($1.86) |
| `U_UART1` | 1 | 3 | USB-to-UART | SOP-8 | WCH | `CH340N` | ₹64.85 ($0.68) | ₹194.55 ($2.04) |
| **Total** | **83** | **249** | — | — | — | — | **₹3,679.89 ($38.59)** | **₹11,039.67 ($115.76)** |

---

## Hardware Bring-Up & Testing Guide

1. **Unpowered Impedance Checks:**
* Verify $0\Omega$ continuity between any ground pad and the continuous Layer 2 ground plane.
* Verify high impedance ($>100\text{k}\Omega$) between `+21V_BOOST`, `+3V3_SYS`, `+BAT_3V7`, and `GND`.


2. **First Power-On (Current-Limited Bench Supply):**
* Connect a bench power supply set to $3.7\text{V}$ (current limited to $300\text{mA}$) to battery connector `J6`.
* Measure `+3V3_SYS` rail at `U7` output: confirm $3.30\text{V} \pm 2\%$.
* Measure `+21V_BOOST` across capacitor `C18`: confirm $21.0\text{V} \pm 1.5\%$.


3. **USB-C Fast Charging Validation:**
* Connect a 5V USB-C power source to `J1`.
* Confirm `LED1` (Red) lights up during active charging and transitions to `LED2` (Green) upon full charge completion.


4. **Firmware Flashing via Manual Boot Mode:**
* Connect USB-C to a host PC.
* To enter bootloader / flashing mode:
1. Press and hold the **`BOOT`** button (`SW_BOOT1`).
2. Press and release the **`RESET`** button (`SW_RST1`).
3. Release the **`BOOT`** button (`SW_BOOT1`).


* Initiate upload in PlatformIO:
```bash
cd firmware/
pio run -e auraforge_50x -t upload

```


* Press **`RESET`** once after flashing to start normal audio firmware execution.


5. **Audio Output Validation:**
* Connect $4\Omega$ speaker loads to terminals `J4` and `J5`.
* Stream a $1\text{kHz}$ audio test tone over Bluetooth or Wi-Fi to verify clean Class-D power output.



---

## License & Credits

Designed and engineered by **Ravi Kachhwaha**.

* **Hardware CAD & Schematics:** Licensed under [CERN-OHL-P v2](https://spdx.org/licenses/CERN-OHL-P-2.0.html).
* **Firmware & Web HUD Source Code:** Licensed under the [MIT License](https://www.google.com/search?q=LICENSE).
