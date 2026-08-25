# AuraForge 50X — ESP32 DSP Audio Firmware

This directory contains the production firmware for the **AuraForge 50X** 50W audio computer platform. It configures the onboard ESP32-WROOM-32E module as a high-fidelity Bluetooth 4.2 A2DP audio sink featuring an active 3-band parametric Biquad IIR equalizer running on the Xtensa dual-core processor.

---

## Key Firmware Capabilities

* **Audio Sink:** Bluetooth 4.2 A2DP sink streaming at 44.1 kHz / 16-bit stereo.
* **Real-Time DSP Engine:** Custom 3-band Biquad IIR filter cascade (Direct Form II Transposed) for independent Bass, Mid, and Treble manipulation without phase artifacts.
* **Anti-Pop Power Sequencing:** Software-controlled delay assertion on the `PIN_AMP_SDZ` line to prevent audible switch-on transients ("thump") while the TPS61088 21V boost rail charges up.
* **Hardware Fault Diagnostics:** Continuous monitoring of the TPA3116D2 active-low `FAULTZ` line for short circuits, DC offsets, or thermal overloads.
* **Serial Command Line (CLI):** Real-time adjustment of DSP equalizer parameters over USB-UART without rebooting.

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

## Building and Flashing

### Method 1: Using PlatformIO (VS Code Recommended)

1. Open VS Code and install the **PlatformIO IDE** extension.
2. Open the `firmware/` directory (`File -> Open Folder... -> firmware`).
3. Connect the AuraForge 50X board to your computer via USB Type-C.
4. Click the **PlatformIO: Build** checkmark icon in the bottom status bar.
5. Click the **PlatformIO: Upload** right-arrow icon to flash the firmware.
6. Open the **Serial Monitor** at **115200 baud** to view system initialization logs.

### Method 2: PlatformIO Command Line (CLI)

```bash
cd firmware/

# Build project
pio run

# Flash to board
pio run --target upload

# Open serial terminal
pio device monitor -b 115200