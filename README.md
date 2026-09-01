# Glove80 Sidecar - ZMK Bluetooth Cirque Trackpad

An open-source, standalone wireless trackpad built using the **Seeed Studio XIAO BLE (nRF52840)** microcontroller and a **40mm Cirque GlidePoint Trackpad** running mainline ZMK firmware. Designed as a peripheral sidecar companion for the MoErgo Glove80 keyboard.

This build uses **SPI communication mode** with direct wire connections to the trackpad test pads. No adapter board, OLED socket, or pull-up resistors needed. The trackpad ships in SPI mode by default — no resistor changes required.

### Gestures Included
* **Tap-to-Click:** Supported natively anywhere on the inner circle surface.
* **Double-Tap:** Supported natively anywhere on the inner circle surface.
* **Right-Click:** Activated by tapping your finger specifically inside the **bottom-right quadrant** zone.
* **Circular Rim Scrolling:** Trace your finger in an arc around the outermost **12% edge** of the circle (Clockwise to scroll Down, Counter-Clockwise to scroll Up).
* **Inertial Cursor:** Flicking your finger and lifting off keeps the cursor gliding in the same direction, gradually decelerating.

---

## Parts List

| Component | Purpose | Sourcing Reference |
| :--- | :--- | :--- |
| **Seeed Studio XIAO BLE (Sense)** | Main Microcontroller | [Amazon Germany](https://amazon.de) |
| **EEMB 3.7V 1S 250mAh Battery** | Rechargable Lithium Polymer battery | [Amazon Germany](https://amazon.de) |
| **30 AWG silicone wire** | 7 wires for SPI + power + DR | [Amazon Germany](https://amazon.de) |
| **40mm Cirque GlidePoint Trackpad** | Trackpad only, no adapter needed | [Keycapsss](https://keycapsss.com) |

---

## Hardware Wiring Guide

Solder 30 AWG wires directly from the XIAO BLE pads to the test pads on the back of the Cirque trackpad PCB. See the [Cirque Pinnacle pinout reference](https://cirquepinnacle.readthedocs.io/en/latest/#pinout) for a photo of the trackpad test pad locations.

SPI is remapped so that power + all 4 SPI signals are on 6 consecutive pads on the right side of the XIAO BLE. Only DR is on the left side:

```
Left side          Right side
---------          ----------
  D0  ← DR          5V
  D1                 GND  ← trackpad GND
  D2                 3V3  ← trackpad VDD
  D3                 D10  ← SCK
  D4                 D9   ← MOSI (SI)
  D5                 D8   ← MISO (SO)
  D6                 D7   ← CS (SS)
```

| XIAO BLE Pad | nRF52840 GPIO | SPI Signal | Trackpad Test Pad |
| :--- | :--- | :--- | :--- |
| **GND** (right side) | — | Ground | GND (FFC pin 11) |
| **3V3** (right side) | — | Power | VDD (FFC pin 12) |
| **D10** (right side) | P1.15 | SCK | SCK (FFC pin 1) |
| **D9** (right side) | P1.14 | MOSI | SI (FFC pin 5) |
| **D8** (right side) | P1.13 | MISO | SO (FFC pin 2) |
| **D7** (right side) | P1.12 | CS | SS (FFC pin 3) |
| **D0** (left side) | P0.02 | DR | DR (FFC pin 4) |

### Battery
Solder the battery wires to the **BAT+** and **BAT-** pads on the bottom of the XIAO BLE. The XIAO has a built-in charge controller — it charges via USB-C automatically.

There is no power switch in this build. The firmware uses ZMK deep sleep (`CONFIG_ZMK_SLEEP=y`) to minimize power consumption during idle periods. For long-term storage, disconnect the battery.

---

## Software Gestures Map

* **Pointer Movement:** Glide your index finger anywhere across the broad **inner 88%** surface of the circular overlay.
* **Left Click:** Tap once quickly anywhere on the inner circle surface.
* **Double Click:** Tap twice quickly anywhere on the inner circle surface.
* **Right Click:** Tap once quickly specifically inside the **bottom-right quadrant** area of the circle surface.
* **Circular Scrolling:** Place your finger directly onto the outermost **12% rim/edge** of the trackpad. Tracing your finger in a **clockwise** circle scrolls downwards; tracing an arc **counter-clockwise** scrolls upwards.

---

## Flashing the Firmware

1. Extract the downloaded `firmware.zip` from your repository artifacts to locate the **`sidecar-seeeduino_xiao_ble-zmk.uf2`** asset.
2. Link your XIAO BLE board to your PC using a USB-C data cable.
3. Press the tiny physical **Reset Button** on the side of the board twice rapidly.
4. The device will reboot and mount as a flash drive named **`XIAO-SENSE`**.
5. Drag and drop your **`sidecar-seeeduino_xiao_ble-zmk.uf2`** file into the drive.
6. The board will flash and unmount automatically.
7. Pair via your computer's Bluetooth settings — the device appears as **"Glove80 Sidecar"**.
