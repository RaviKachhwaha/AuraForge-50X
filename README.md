# AuraForge 50X — Programmable 50W High-Fidelity Wireless DSP & CSI Sensing Platform

[![KiCad 10](https://img.shields.io/badge/EDA-KiCad%2010.0-blue.svg)](https://kicad.org)
[![PCB Layers](https://img.shields.io/badge/PCB%20Layers-4--Layer%20Stackup-orange.svg)]()
[![DRC Status](https://img.shields.io/badge/DRC%20Status-Passed%20(0%20Errors)-brightgreen.svg)]()
[![ESP32 Dual-Core](https://img.shields.io/badge/ESP32-240MHz%20Dual--Core-red.svg)](https://espressif.com)
[![Wi-Fi CSI Radar](https://img.shields.io/badge/Wi--Fi%20CSI-RuView%2050Hz%20UDP-cyan.svg)]()
[![Hack Club](https://img.shields.io/badge/Hack%20Club-Forge%20Grant-red.svg)](https://hackclub.com)

**AuraForge 50X** is an open-source, programmable 4-layer wireless audio computer, real-time digital signal processing (DSP) station, and wireless RF sensing platform. Going far beyond standard single-purpose Bluetooth receiver breakout boards, AuraForge 50X combines:

1. **High-Power Class-D Audio Stage**: Texas Instruments **TPA3116D2** dual BTL stereo amplifier delivering up to $2 \times 50\text{W}$ into $4\Omega$ at $21\text{V}$.
2. **Synchronous 10A Boost Power Converter**: Texas Instruments **TPS61088** stepping a single-cell 3.7V lithium battery up to 21.0V with hardware soft-start anti-pop sequencing.
3. **Advanced Digital Signal Processing (DSP)**: 10-Band Parametric Biquad IIR Equalizer, Dynamic Sub-Bass Psychoacoustic Harmonics, 3D Spatial Stereo Expander, and Dynamic Range Compressor (DRC).
4. **Wi-Fi Channel State Information (CSI) Motion Sensing Radar**: 64-subcarrier spatial disturbance sensing engine streaming live at 50 Hz over UDP (Port 5000) directly to PC software (**RuView**, Python, MATLAB).
5. **Cyberpunk Web Command Station**: Real-time bidirectional WebSocket telemetry bridge with 5 selectable sci-fi themes (`CYBER_CYAN`, `SOLAR_FLARE`, `MATRIX_NEON`, `VAPORWAVE_80S`, and `DEEP_SPACE`).

---

## Visual Showcase & Design Verification

### 3D Top View (Assembled SMT Rendering)
![AuraForge 50X 3D Top View](docs/images/3d_top_render.png)

### 3D Bottom View (Thermal Dissipation Plane & Vias)
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
| **Main Processing Core** | Espressif ESP32-WROOM-32E (240 MHz Dual-Core Tensilica LX6, 4MB Flash, Wi-Fi & BLE) |
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

## Grant Budget Justification (3-Board Turnkey PCBA Prototype Run)

Unlike simple low-power microcontroller breakouts or basic 2-layer sensor modules, the AuraForge 50X is a high-density, multi-rail audio computing platform combining an 83-component Bill of Materials with high-power switching stages. The high-current 10A boost converter and 50W Class-D switching amplifiers strictly require an industrial **4-layer PCB stackup** with a continuous internal ground plane ($35\ \mu\text{m}$ outer copper, continuous $17.5\ \mu\text{m}$ inner return layer) to handle heavy switching transients, maintain low impedance, and prevent thermal runaway or RF interference with the onboard ESP32 Wi-Fi/Bluetooth antenna.

### Manufacturing & Batch Quantity Modeling (3 Boards)
On turnkey PCB assembly portals (such as **pcbpower.com**), the minimum order quantity (MOQ) for automated SMT assembly setup is 2 units. Fixed manufacturing costs—such as laser-cut stainless steel solder stencils, pick-and-place feeder tooling, automated optical inspection (AOI), and 4-layer photolithography setup—amount to approximately **₹10,500 INR (~$110.10 USD)** for the initial setup.

Because fixed SMT setup fees are already covered, **scaling from 2 boards to 3 boards costs only ~₹1,300 INR ($13.63 USD) more in assembly labor**, making a 3-board prototype run the most cost-effective approach for hardware bring-up, firmware testing, and destructive power stage verification.

### Detailed Turnkey Budget Breakdown (₹95.37 = $1.00 USD)

| Expense Item | Description | Cost (INR ₹) | Cost (USD $) |
| :--- | :--- | :--- | :--- |
| **Bare PCB Fabrication** | 5 pcs 4-Layer FR-4 ($100\text{ mm} \times 80\text{ mm}$, TG140, 100% E-Test) | ₹5,200.00 | $54.52 |
| **Laser SMT Stencil** | Top-layer framed laser-cut stainless steel stencil | ₹1,400.00 | $14.68 |
| **SMT Assembly & Setup** | Turnkey pick-and-place mounting & reflow for 3 boards (249 total placements) | ₹5,200.00 | $54.52 |
| **Components (3 Units)** | 3 full sets of 83 genuine components ($3 \times \text{₹3,679.89}$) | ₹11,039.67 | $115.76 |
| **Taxes (18% GST)** | Mandatory Indian Government GST on turnkey fabrication & parts | ₹4,111.14 | $43.11 |
| **Logistics & Shipping** | Insured courier delivery to lab workspace | ₹450.00 | $4.72 |
| **Total Estimated Budget**| **Complete 3-Unit Turnkey PCBA Prototype Run** | **₹27,400.81** | **$287.31** |

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
                    |              USB Type-C (J1)               |
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
| 3.7V Li-ion    |->|  DW01A + FS8205A  |    | ESP32-WROOM-32E   |  <-- SW_RST1  (EN)
| Battery (J6)   |  |  Protection (U5/6)|    | MCU & DSP Core(U4)|
+----------------+  +---------+---------+    +---------+---------+
                              |                        |
                              +-------> +BAT_3V7 ------+ (via AMS1117 3.3V)
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

## Bill of Materials (BOM)

The board consists of **83 total components** consolidated into **45 unique manufacturer part numbers (MPNs)**:

| Designator | Qty | Value | Package / Footprint | Manufacturer | Manufacturer Part Number (MPN) |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `C1` | 1 | 10uF 25V | 0805 | Samsung | `CL21A106KAYNNNE` |
| `C2, C9, C10` | 3 | 22uF 10V | 0805 | Samsung | `CL21A226MPCLRNC` |
| `C3, C7, C8, C37` | 4 | 100nF 16V | 0603 | Yageo | `CC0603KRX7R7BB104` |
| `C4` | 1 | 10uF 16V | 0805 | Samsung | `CL21A106KOQNNNE` |
| `C5` | 1 | 22uF 6.3V | 0805 | Samsung | `CL21A226MQQNNNE` |
| `C6` | 1 | 10uF 10V | 0805 | Murata | `GRM21BR61A106KE19L` |
| `C11, C24-C28` | 6 | 1uF 16V | 0603 | Yageo | `CC0603KRX7R7BB105` |
| `C12` | 1 | 100nF 25V | 0603 | Yageo | `CC0603KRX7R8BB104` |
| `C13` | 1 | 10nF 25V | 0603 | Yageo | `CC0603KRX7R8BB103` |
| `C14` | 1 | 2.2nF 25V | 0603 | Yageo | `CC0603KRX7R8BB222` |
| `C15, C16, C17` | 3 | 22uF 25V | 1206 | Samsung | `CL31A226KAHNNNE` |
| `C18` | 1 | 220uF 25V | Radial 8x10.5 | Panasonic | `EEE-FK1E221P` |
| `C19-C23` | 5 | 1uF 25V | 0603 | Yageo | `CC0603KRX7R8BB105` |
| `C29-C32` | 4 | 0.22uF 25V | 0603 | Yageo | `CC0603KRX7R8BB224` |
| `C33-C36` | 4 | 0.68uF 25V | 0805 | Yageo | `CC0805KKX7R9BB684` |
| `D1, D2, D3` | 3 | SS34 (3A 40V) | SMA (DO-214AC) | MDD | `SS34` |
| `J1` | 1 | Type-C 16-Pin | SMD + PTH Tabs | Korean Hroparts | `TYPE-C-31-M-12` |
| `J2` | 1 | DC Jack 5.5x2.1mm | PTH | CUI Devices | `PJ-002AH` |
| `J3` | 1 | 3.5mm Stereo Jack | PTH | XKB Enterprise | `PJ-320A` |
| `J4, J5` | 2 | 2-Pin Screw Terminal | 5.08mm PTH | Phoenix Contact | `1715721` |
| `J6` | 1 | JST-XH 2-Pin 2.5mm | PTH | JST | `B2B-XH-A(LF)(SN)` |
| `L1` | 1 | 2.2uH 6.5A | SMD 7.3x7.3 | Sunlord | `MWSA0603S-2R2MT` |
| `L2` | 1 | 2.2uH 10.0A | SMD 10.4x10.4 | Wurth Elektronik | `7443340220` |
| `L3, L4, L5, L6` | 4 | 10uH 3.8A | SMD 8.0x8.0 | Taiyo Yuden | `NRS8040T100MJGJ` |
| `LED1` | 1 | Red LED (Charge) | 0603 | Kingbright | `APT1608SURCK` |
| `LED2` | 1 | Green LED (Done) | 0603 | Kingbright | `APT1608ZGC` |
| `R1, R2` | 2 | 5.1k 1% | 0603 | Yageo | `RC0603FR-075K1L` |
| `R3` | 1 | 45.3k 1% | 0603 | Yageo | `RC0603FR-0745K3L` |
| `R4` | 1 | 51k 1% | 0603 | Yageo | `RC0603FR-0751KL` |
| `R5, R6, R17, R18` | 4 | 1k 1% | 0603 | Yageo | `RC0603FR-071KL` |
| `R7, R8, R10, R11` | 4 | 10k 1% | 0603 | Yageo | `RC0603FR-0710KL` |
| `R9` | 1 | 165k 1% | 0603 | Yageo | `RC0603FR-07165KL` |
| `R12, R14, R15` | 3 | 100k 1% | 0603 | Yageo | `RC0603FR-07100KL` |
| `R13` | 1 | 120k 1% | 0603 | Yageo | `RC0603FR-07120KL` |
| `R16` | 1 | 100R 1% | 0603 | Yageo | `RC0603FR-07100RL` |
| `R19` | 1 | 10R 1% | 0603 | Yageo | `RC0603FR-0710RL` |
| `SW_BOOT1, SW_RST1`| 2 | SMT Tact Switch | SMD 6.0x3.5 | C&K | `PTS645SL43SMTR92LFS` |
| `U1` | 1 | TPA3116D2 50W Amp | HTSSOP-32 | Texas Instruments | `TPA3116D2DADR` |
| `U2` | 1 | TPS61088 10A Boost | VQFN-20 | Texas Instruments | `TPS61088RHLR` |
| `U3` | 1 | IP2312 3A Charger | ESOP-8 | Injoinic | `IP2312` |
| `U4` | 1 | ESP32-WROOM-32E | Module | Espressif Systems | `ESP32-WROOM-32E-N4` |
| `U5` | 1 | Battery Protection | SOT-23-6 | Fortune Semi | `DW01A-G` |
| `U6` | 1 | Dual N-MOSFET | TSSOP-8 | Fortune Semi | `FS8205A` |
| `U7` | 1 | 3.3V 1A LDO | SOT-223 | AMS | `AMS1117-3.3` |
| `U_UART1` | 1 | USB-to-UART Bridge | SOP-8 | WCH | `CH340N` |

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
   * Initiate upload in PlatformIO or Arduino IDE to flash the firmware.
   * Press **`RESET`** once after flashing to start normal audio firmware execution.
5. **Audio Output Validation:**
   * Connect $4\Omega$ speaker loads to terminals `J4` and `J5`.
   * Stream a $1\text{kHz}$ audio test tone over Bluetooth or Wi-Fi to verify clean Class-D power output.
