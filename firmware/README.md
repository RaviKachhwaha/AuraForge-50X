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
| **Boot Mode Selection** | `GPIO 0` (`IO0`) | Manual Tactile Switch `SW_BOOT1` |
| **Hardware Reset** | `EN` (Reset) | Manual Tactile Switch `SW_RST1` |

---

## Manual Bootloader & Flashing Procedure

Since the hardware utilizes direct manual pushbuttons for deterministic flashing control:

1. Connect the AuraForge 50X board to your computer via USB Type-C.
2. Put the ESP32 into ROM Bootloader Mode:
   * **Press and hold** the **`BOOT`** button (`SW_BOOT1`).
   * **Press and release** the **`RESET`** button (`SW_RST1`).
   * **Release** the **`BOOT`** button (`SW_BOOT1`).
3. Start the firmware upload in your IDE or terminal.
4. Once the upload finishes (100%), press the **`RESET`** button (`SW_RST1`) once to boot into the audio firmware.

---

## Building and Flashing

### Method 1: Using PlatformIO (VS Code)

1. Open VS Code and ensure the **PlatformIO IDE** extension is installed.
2. Open the `firmware/` folder (`File -> Open Folder... -> firmware`).
3. Connect the board via USB Type-C and put it in Bootloader mode using the button sequence above.
4. Click the **PlatformIO: Build** icon (`✓`) to compile.
5. Click the **PlatformIO: Upload** icon (`→`) to flash.
6. Open the **Serial Monitor** at **115200 baud** to view system boot logs.

### Method 2: PlatformIO Command Line (CLI)

```bash
cd firmware/

# 1. Compile the firmware
pio run

# 2. Put board in bootloader mode (Hold BOOT -> Click RESET -> Release BOOT)

# 3. Flash to board
pio run --target upload

# 4. Open serial terminal monitor
pio device monitor -b 115200
```

---

## Serial CLI Commands

Connect over USB-C at **115200 baud** (using PlatformIO Monitor, PuTTY, or Arduino Serial Monitor) to send live runtime commands:

| Command | Example | Description |
| :--- | :--- | :--- |
| `bass <dB>` | `bass 3.5` | Adjusts Low-Shelf filter boost/cut ($\pm 12.0\text{ dB}$) at $100\text{Hz}$ |
| `mid <dB>` | `mid -1.0` | Adjusts Peaking EQ filter ($\pm 12.0\text{ dB}$) at $1.0\text{kHz}$ |
| `treble <dB>` | `treble 2.0` | Adjusts High-Shelf filter ($\pm 12.0\text{ dB}$) at $8.0\text{kHz}$ |
| `status` | `status` | Prints free heap memory, CPU frequency, active EQ gains, and amplifier status |
