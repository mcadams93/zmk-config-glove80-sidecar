# 🛸 Standalone ZMK Bluetooth Cirque Trackpad (SPI Mode)

An open-source, standalone wireless trackpad built using the **Seeed Studio XIAO BLE (nRF52840)** microcontroller and a **40mm Cirque GlidePoint Trackpad** running mainline ZMK firmware. 

This build utilizes high-speed **SPI Communication Mode**, which complies natively with standard ZMK pointing drivers to offer fast tracking refresh rates, smooth pointer navigation, and rock-solid compilation without relying on custom backend configuration patches.

### ✨ Gestures Included
* **Tap-to-Click:** Supported natively anywhere on the inner circle surface.
* **Double-Tap:** Supported natively anywhere on the inner circle surface.
* **Right-Click:** Activated by tapping your finger specifically inside the **bottom-right quadrant** zone.
* **Circular Rim Scrolling:** Trace your finger in an arc around the outermost **12% edge** of the circle (Clockwise to scroll Down, Counter-Clockwise to scroll Up).

---

## 📦 European Sourcing Checklist

To minimize international shipping fees, avoid customs import duties, and bypass long transit delivery delays inside the EU, electronics can be ordered together from Amazon Germany, and the keyboard tracking module from Keycapsss.

| Component | Purpose | Sourcing Reference |
| :--- | :--- | :--- |
| **Seeed Studio XIAO BLE Sense** | Main Microcontroller (with pre-soldered headers) | [Amazon Germany](https://amazon.de) |
| **Seeed Studio Grove Shield** | Expansion base breakout with built-in power switch | [Amazon Germany](https://amazon.de) |
| **EEMB 3.7V 1S 250mAh Battery** | Rechargable Lithium Polymer battery | [Amazon Germany](https://amazon.de) |
| **LightingWill 30 AWG Wires** | Ultra-flexible heat-resistant silicone soldering wires | [Amazon Germany](https://amazon.de) |
| **40mm Cirque GlidePoint Kit** | Circular trackpad module & FFC adapter breakout board | [Keycapsss](https://keycapsss.com) |

---

## 🔌 Hardware Configuration & Wiring Guide

### ⚠️ Step 1: Confirm SPI Solder Jumper Configuration
Most Cirque GlidePoint trackpads ship set to SPI mode out-of-the-box. Before soldering wires, turn the circular trackpad over to visually inspect the back circuit board:
* Ensure that the surface resistor pad labeled **R1** is **Closed/Bridged** (Enables SPI communication mode).
* Ensure that the surface resistor pad labeled **R2** remains **Open/Disconnected** (Disables I2C communication mode).

### ⚡ Step 2: The Solder Routing Blueprint
Press your Seeed Studio XIAO BLE module into the female sockets on top of your Grove Shield. Use your 30 AWG silicone wires to route the power and data signal matrices. 

*Slide a small piece of the included 4mm heat-shrink tubing onto each wire prior to heating up the joint, then solder the lines using this specific pinout mapping:*

#### 🔋 Power Configuration
* Solder the **Red (Positive)** wire of your LiPo battery to the pad marked **`BAT+`** on the back underside of the Grove Shield.
* Solder the **Black (Negative)** wire of your LiPo battery to the pad marked **`GND`** on the back underside of the Grove Shield.

#### 📊 Data Bus Signal Mapping
Route your flexible silicone lines from the side pin header columns of the Grove Shield over to the corresponding solder loops on your Keycapsss FFC adapter breakout board:

| Seeed Studio Grove Shield Pin | Wire Line Route | Cirque FFC Breakout Board Pad |
| :--- | :---: | :--- |
| **3.3V** | ➡️ | **VCC** (System Power Input) |
| **GND** | ➡️ | **GND** (System Ground Loop) |
| **D5** | ➡️ | **SCK** (SPI Master Clock) |
| **D10** | ➡️ | **MOSI / SDI** (Serial Data In) |
| **D9** | ➡️ | **MISO / SDO** (Serial Data Out) |
| **D2** | ➡️ | **CS / SS** (Chip / Slave Select) |
| **D3** | ➡️ | **DR** (Data Ready Interrupt Line) |

---

## 🛠️ Software Gestures Map

All interaction is routed directly to the trackpad skin surface via absolute coordinate translation. No extra mechanical mouse buttons are needed:

* 🖱️ **Pointer Movement:** Glide your index finger anywhere across the broad **inner 88%** surface of the circular overlay.
* 👆 **Left Click:** Tap once quickly anywhere on the inner circle surface.
* ✌️ **Double Click:** Tap twice quickly anywhere on the inner circle surface.
* 📑 **Right Click:** Tap once quickly specifically inside the **bottom-right quadrant** area of the circle surface.
* 🌀 **Circular Scrolling:** Place your finger directly onto the outermost **12% rim/edge** of the trackpad. Tracing your finger in a **clockwise** circle scrolls downwards; tracing an arc **counter-clockwise** scrolls upwards.

---

## 🚀 Flashing the Firmware

Once your GitHub Action finishes compiling successfully, download and flash the payload using these steps:
1. Extract the downloaded `firmware.zip` folder from your repository artifacts section to locate the **`xiao_ble-zmk.uf2`** asset.
2. Link your stacked XIAO BLE board to your PC using a standard USB-C data cable.
3. Use a tweezer tip or a small paperclip to press the tiny physical **Reset Button** on the side of the board twice rapidly.
4. The device will reboot and mount on your computer's filesystem as an external flash thumb drive named **`XIAO-SENSE`**.
5. Drag and drop your **`xiao_ble-zmk.uf2`** file directly into the drive directory window.
6. The board will swallow the payload, automatically flash its internal flash sector storage, and unmount. 
7. Turn on your hardware power switch, pair it via your computer's native Bluetooth settings panel, and your custom wireless trackpad is ready for action!
