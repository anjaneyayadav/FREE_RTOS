# ESP32-CAM Motion-Triggered Wi-Fi Security Camera

*A Complete Beginner's Build Guide*

---

# 1. Project Objective

Design and build a low-cost, motion-triggered Wi-Fi camera system using an **ESP32-CAM** and an **HC-SR04 ultrasonic sensor**.

The system automatically:

- Detects nearby motion
- Streams live video over Wi-Fi
- Saves each motion event as a video clip
- Eliminates the need for continuous recording

## Goals

- Detect motion using an ultrasonic sensor.
- Activate Wi-Fi and camera only when motion is detected.
- Stream live video to a PC.
- Save each event as an MP4 video.
- Use inexpensive, readily available hardware.

---

# 2. What You're Building

This project creates a Wi-Fi security camera.

Workflow:

1. HC-SR04 continuously measures distance.
2. Object detected within **100 cm**.
3. ESP32-CAM connects to Wi-Fi.
4. Camera starts streaming.
5. Python program receives video.
6. Video is displayed live.
7. Video is saved as an MP4 clip.
8. If no motion is detected for 5 seconds, streaming stops automatically.

---

# 3. Required Hardware

| Component | Quantity |
|-----------|----------|
| AI Thinker ESP32-CAM | 1 |
| HC-SR04 Ultrasonic Sensor | 1 |
| USB-to-TTL Adapter (FTDI/CP2102/CH340) | 1 |
| Jumper Wires | Several |
| Breadboard | 1 |
| 5V Power Supply | 1 |
| Windows PC | 1 |

> **Important**
>
> Both the PC and ESP32-CAM must be connected to the same Wi-Fi network.

> **Warning**
>
> Do **not** power the ESP32-CAM from a Raspberry Pi 5V pin.
> Use a stable external 5V supply or the USB-TTL adapter.

---

# 4. Connections

## HC-SR04 → ESP32-CAM

| HC-SR04 | ESP32-CAM |
|----------|-----------|
| VCC | 5V |
| GND | GND |
| TRIG | GPIO14 |
| ECHO | GPIO13 |

> Never use **GPIO16** for the ultrasonic sensor.
>
> GPIO16 is reserved for the ESP32-CAM PSRAM.

---

## USB-TTL → ESP32-CAM

| USB-TTL | ESP32-CAM |
|----------|-----------|
| 5V | Leave Disconnected |
| GND | GND |
| TX | U0R (RX) |
| RX | U0T (TX) |

Use the adapter's **5V output**, not **3.3V**.

---

## Flash Mode

During code upload:

GPIO0 → GND

After upload:

Disconnect GPIO0 from GND.

---

## Common Ground

If using an external power supply, connect all grounds together.

- ESP32-CAM GND
- HC-SR04 GND
- USB-TTL GND
- External Power Supply GND

---

## Common 5V Supply

Connect 5V to:

- ESP32-CAM
- HC-SR04
- External Power Supply

---
