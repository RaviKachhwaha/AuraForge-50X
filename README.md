# AuraForge 50X - A Programmable 50W High-Fidelity Wireless DSP & CSI Sensing Platform

[![KiCad 10](https://img.shields.io/badge/EDA-KiCad%2010.0-blue.svg)](https://kicad.org)
[![PCB Layers](https://img.shields.io/badge/PCB%20Layers-4--Layer%20Stackup-orange.svg)]()
[![DRC Status](https://img.shields.io/badge/DRC%20Status-Passed%20(0%20Errors)-brightgreen.svg)]()
[![ESP32 Dual-Core](https://img.shields.io/badge/ESP32-240MHz%20Dual--Core%20LX6-red.svg)](https://espressif.com)
[![Wi-Fi CSI Radar](https://img.shields.io/badge/Wi--Fi%20CSI-RuView%2050Hz%20UDP-cyan.svg)]()
[![DSP Pipeline](https://img.shields.io/badge/Audio%20DSP-10--Band%20Biquad%20%2B%20FFT-brightgreen.svg)]()
[![Web UI](https://img.shields.io/badge/Web%20Dashboard-11--Tab%20Cyberpunk%20HUD-magenta.svg)]()
[![Hack Club](https://img.shields.io/badge/Hack%20Club-Forge%20Grant-red.svg)](https://hackclub.com)
[![License](https://img.shields.io/badge/License-MIT%20%2F%20CERN--OHL--P%20v2-green.svg)](LICENSE)

I built **AuraForge 50X** a PCB based programmable 4-layer wireless audio computer with real-time digital signal processing (DSP), and with wireless RF motion-sensing radar.
And it also built on dual-core **Espressif ESP32-WROOM-32E** microcontroller and AuraForge 50X transforms a high-efficiency Class-D stereo amplifier to a fully connected, software-defined sound station with non-intrusive room presence detection system.

The board integrates:
1. **High-Power Class-D Audio Stage**: Texas Instruments **TPA3116D2** dual BTL stereo amplifier.
2. **Hybrid Dual-Rail Power Architecture**: Battery and DC Jack.
3. **Advanced Digital Signal Processing (DSP)** with a Dynamic Range Compressor (DRC).
4. **Wi-Fi Channel State Information (CSI) Motion Sensing Radar**: to use CSI in tools like (**RuView**, Python, MATLAB).
5. **A Cyberpunk Web Command Station**.

---

## This is Visual Showcase & For Design Verification

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

## Technical Specifications with main board components:

1. Main Processing Core: Espressif ESP32-WROOM-32E
2. Audio Power Stage: Texas Instruments TPA3116D2
3. Synchronous Boost Stage: Texas Instruments TPS61088RHLR
4. Battery Fast Charging: Injoinic IP2312 Synchronous Buck Converter
5. Battery Protection Matrix: Fortune Semi DW01 + FS8205A Dual N-MOSFET
6. USB-to-UART Bridge: WCH CH340N with an dedicated manual hardware pushbuttons (SW_BOOT1 & SW_RST1)
7. Power Inputs: Single-Cell 3.7V Li-ion/LiPo or External DC Jack
8. Board Dimensions: 100.0 mm X 80.0 mm, 1.6mm thickness

---

## The Grant Budget Justification (Turnkey PCBA Prototype Run)

I want to tell that unlike simple low-power microcontroller boards or basic 2-layer sensor boards, this AuraForge 50X is a high-density, multi-rail audio computing platform with combining 83-component Bill of Materials with high-power switching stages not a simple Amp. and sourcing the components and manufacturing requires an 4-layer PCB fabrication with solid ground plane, and an automated SMT pick-and-place assembly system and battery power.

### The Need of Manufacturing and Batch Quantity Modeling (3 Boards)
Due to turnkey PCB assembly on portals like (**pcbpower.com**), have minimum order quantity (MOQ) for automated PCBA setup and it is min 2 units. and a fixed manufacturing costs like laser-cut stainless steel solder stencils and pick-and-place feeder, automated optical inspection (AOI), and the main 4-layer setup-amount which is total approximately **₹10,500 INR (~$110.10 USD)** for only the PCBA setup.

Because it have fixed PCBA setup fees are already covered, **building from 2 boards to 3 boards costs only ~₹1,300 INR ($13.63 USD) more in assembly no extra cost in setup**, and making a 3-board built the most cost effective approach for hardware build, firmware testing, and destructive power stage verification which is needed.

The full PCBA Budget Breakdown (Converted at ₹95.37 = $1.00 USD) are uploaded at [bom.csv](bom.csv).
Note:
**Itemized Components file:** For the complete individual component list whoch have (resistors, capacitors, TI TPA3116D2, TI TPS61088, ESP32-WROOM-32E) and exact manufacturer part numbers (MPNs), refer to [components_bom.csv](components_bom.csv).

---

## Main 4-Layer PCB Board Stackup Architecture

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

## The Main Hardware Subsystem Architecture

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

## 5 Advanced Pure-Firmware Subsystems to make it best

I added all 5 core systems purely in software on the physical ESP32 board without requiring external dedicated DSP hardware and external EEPROMs, or any sensors:

1. Non-Volatile Storage (NVS) Persistent Preset Manager (ConfigManager)
2. High-Speed 128-Point Radix-2 Audio FFT Visualizer (DspEngine)
3. Dual-Bank Asynchronous Web OTA Firmware Flashing (WebServerModule)
4. Wi-Fi CSI Spatial Presence State Machine & Touchless Automation System (CsiRadarEngine)
5. Closed-Loop Thermal & Digital Headroom Governor (HardwareMonitor)

---

## Unified Cyberpunk and Sci-Fi Web UI with 11 Navigation Tabs

I also add system to check the Web UI byte-for-byte across preview_ui.html (PC Standalone Viewer), index.html (Source), and src/WebUiAssets.h (PROGMEM embedded).

And this are the 11 Interactive Navigation Tabs name and you can check it by opening the preview_ui.html in your browsor:
1. Master Control HUD
2. 10-Band Equalizer Canvas
3. Compressor & Crossover
4. 16-Band FFT & Signal Gen
5. Wi-Fi CSI Presence & RuView
6. Thermal & Digital Governor
7. Dual-Bank Web OTA
8. NVS Preset Manager
9. Cyber Themes & Power
10. Wi-Fi & Network
11. Diagnostics & Telemetry

### I added 8 Selectable Cyberpunk Themes For Sci-Fi lovers

| Theme ID | Primary Glow Accent | Secondary Accent | Background Gradient |
| --- | --- | --- | --- |
| CYBER_CYAN | Electric Cyan | Neon Violet | Deep Slate Dark |
| SOLAR_FLARE | Sunburst Amber | Flame Orange | Obsidian Charcoal |
| MATRIX_NEON | Cyber Mint | Phosphor Green | Terminal Black |
| VAPORWAVE_80S | Neon Magenta | Retro Indigo | Synthwave Night |
| DEEP_SPACE | Galactic Sapphire | Nebula Purple | Deep Void Navy |
| TOKYO_DRIFT | Sakura Hot Pink | Turquoise | Midnight Cyber |
| STEALTH_OLED | Titanium Ice | Carbon Silver | Pure Pitch Black |
| CRIMSON_CORE | Blood Ruby | Blaze Orange | Volcanic Dark |

---

## Bill of Materials (BOM) & Sourcing Files:

The hardware bill of materials is maintained in two structured CSV files in the repository root:

1. **[bom.csv](bom.csv)**: It have complete project budget covering 4-layer PCB fabrication, automated SMT assembly, total component sourcing, 18650 Li-ion battery, stereo woofer speakers, and Anker 100W USB-C data cable.
2. **[components_bom.csv](components_bom.csv)**: It have detailed line-by-line itemization of all 83 SMD/PTH components across all 45 unique Manufacturer Part Numbers (MPNs) with package footprints, quantities, verified distributor pricing, and direct distributor links (DigiKey, LCSC) and with detailed description of every component.

## This are Assembly & Hardware Build Instructions

Follow this step-by-step guide to assemble and bring up the AuraForge 50X hardware:

### Step 1: The Component SMT Pick-and-Place & Soldering
1. **Turnkey SMT Assembly (Recommended):** First submit the Gerber archive (hardware/gerbers/AuraForge_50X_Gerbers.zip), BOM (components_bom.csv), and Centroid file (hardware/production/AuraForge_50X-all-pos.csv) to **pcbpower.com** or which PCBA provider you want for automated SMT pick-and-place and reflow soldering.

### Step 2: The Thermal Dissipation & Heatsink Installation Steps
1. First apply a small thermally conductive silicone pad 15 mm X 15 mm, 1.0 mm thickness on the top surface of U1 (TPA3116D2).
2. Then attach a low-profile anodized aluminum heatsink 15 mm X 15 mm X 10 mm to remove the heat during continuous 2X50W Class-D at 4Omhs operation.

### Step 3: The Mechanical Mounting & Wiring
1. First mount the PCB inside an enclosure or where you want using 4 X M3 screws through the 3.0 mm radius corner mounting holes.
2. Then connect a single 3.7V 3000mAh 18650 Li-ion battery to J6 using the 2.5mm JST-XH keyed connector (Red = +BAT, Black = GND).
3. And then the last step remove 5 mm of insulation from 4Omhs stereo speaker wires, adn then insert them into terminal blocks of J4 (Left Channel) and J5 (Right Channel), and tighten the clamping screws with a flathead screwdriver and your hardware setup is done.

---

## Main Firmware Flashing and Software Setup Guide

### 1. Toolchain Installation
* Install **VS Code** with the **PlatformIO IDE** extension (or install platformio-core via CLI).

### 2. Main Flashing Over USB-C (Hardware UART) Only needed for first time:
1. First connect the AuraForge 50X board to your computer or Laptop using a USB Type-C data cable connected to port J1.
2. Then enter hardware bootloader mode:
   * 1. Press and hold the **BOOT** button (SW_BOOT1).
   * 2. Press and release the **RESET** button (SW_RST1).
   * 3. Release the **BOOT** button (SW_BOOT1).
3. And then open a terminal in the firmware/ directory and execute this command:

```bash
   # Build and upload the firmware image and partition table
   pio run -e auraforge_50x -t upload

   # Launch serial monitor at 115200 baud
   pio device monitor

```

4. After all flashing done press the **RESET** button (SW_RST1) once. The onboard LED (LED1) will indicate active system boot.

### 3. Now you can run Over-the-Air (OTA) Web Flashing

1. First connect your PC or phone to the Wi-Fi AP **AuraForge-50X-AP** (Default IP: 192.168.4.1).
2. Then open http://192.168.4.1 or the correct IP in any browser and navigate to the **Dual-Bank Web OTA** tab.
3. Then drag and drop the compiled firmware.bin file into the upload zone to update the inactive partition (ota_1) with real-time percentage progress and automatic reboot rollback protection system.

And after all done you are all set !! You Can Now Run it.

---

## List of Known Issues & Workarounds

1. **ESP32 ADC Non-Linearity at Low Battery Voltages:**
* Issue: The internal ESP32 SAR ADC exhibits non-linear voltage measurements below 0.1V and above 3.1V.
* Workaround: The firmware implements a calibrated two-point piecewise lookup table and a 16-sample Exponential Moving Average (EMA) filter in HardwareMonitor.cpp to linearize battery percentage reporting.

2. **Bluetooth Audio & Wi-Fi CSI Radio Coexistence:**
* Issue: Classic Bluetooth A2DP audio streaming and 2.4 GHz Wi-Fi CSI promiscuous frame capture share the same internal RF transceiver on the ESP32.
* Workaround: The FreeRTOS scheduler time-slices RF events: when Bluetooth A2DP is actively receiving audio packets, CSI capture frequency is throttled to 25 Hz to prevent audio buffer underruns.

3. **Thermal Throttling During Sustained High-Power Output:**
* Issue: Continuous playback at >40W RMS per channel inside sealed enclosures can heat the TPA3116D2 amplifier die.
* Workaround: The closed-loop thermal governor in HardwareMonitor.cpp monitors junction temperature and automatically attenuates digital pre-amp gain by -3.0 dB if temperature exceeds 75 C, to preventing thermal shutdown trips.

---

## Peer Review & Design Sanity Check

The schematic architecture, 4-layer PCB layout, and high-power thermal calculations were independently reviewed by community engineers on the Hack Club forum before fabrication sign-off:

* **RF Antenna Isolation Review (by *tty7* & *Madhav*):**
* *Feedback:* Pointed out that placing the ESP32 MIFA antenna directly over internal copper plane pours would detune the 50Ω antenna impedance and sink RF energy into the ground plane.
* *Resolution Implemented:* Re-engineered the board edge in KiCad 10 so the entire ESP32 antenna overhangs past the Edge.Cuts boundary into open air, with zero copper planes beneath the radiating element.


* **Power Inductor Saturation Current Review (by *Claude Sonnet 5 AI*):**
* *Feedback:* Verified that the synchronous boost converter (TPS61088) requires a shielded inductor capable of sustaining >10A peak saturation current under maximum 50W audio transients.
* *Resolution Implemented:* Specified the Würth Elektronik 7443340220 2.2muH, 10.0A high-current power inductor in L2.

And Very Very Thanks to Madhav, tty7 and Claude Team.

---

## Credits & Open-Source Attributions

The full project AuraForge 50X is made possible thanks to the following open-source frameworks, libraries, and EDA tools:

* **[KiCad EDA](https://www.kicad.org/):** Professional open-source schematic capture and 4-layer PCB design suite.
* **[Espressif Systems](https://github.com/espressif):** ESP-IDF development framework, FreeRTOS dual-core SMP scheduler, and Wi-Fi CSI promiscuous RX API.
* **[ESP32-A2DP](https://github.com/pschatzmann/ESP32-A2DP) by Phil Schatzmann:** High-performance Classic Bluetooth A2DP audio sink library with I2S DMA streaming.
* **[ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer) & [AsyncTCP](https://github.com/me-no-dev/AsyncTCP):** Asynchronous HTTP and WebSocket engine powering the Cyberpunk HUD.
* **[ArduinoJson](https://arduinojson.org/) by Benoît Blanchon:** Efficient embedded JSON parser and serializer for real-time telemetry streaming.
* **[RuView](https://www.google.com/search?q=https://github.com/mikus/ruview):** Real-time PC visualization engine for 64-subcarrier Wi-Fi Channel State Information streams.
* **[Hack Club](https://hackclub.com/):** For supporting open-source hardware makers through the Forge Grant program.

## License
Designed and engineered by Ravi Kachhwaha.

Hardware CAD & Schematics: Licensed under CERN-OHL-P v2.
Firmware & Web HUD Source Code: Licensed under the MIT License.
