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
# 5. Arduino IDE Setup

## Step 1: Install Arduino IDE

Download and install the latest Arduino IDE 2.x.

---

## Step 2: Add ESP32 Board Manager URL

Open:

**File → Preferences**

In **Additional Boards Manager URLs**, add:

```text
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```

Click **OK**.

---

## Step 3: Install ESP32 Board Package

Go to:

```
Tools → Board → Boards Manager
```

Search for:

```
esp32
```

Install:

```
esp32 by Espressif Systems
Version: 2.0.11
```

> **Note**
>
> Avoid version **3.x**, as it has known Wi-Fi stability issues on the AI Thinker ESP32-CAM.

---

## Step 4: Select the Board

Go to:

```
Tools → Board → ESP32 Arduino
```

Select:

```
AI Thinker ESP32-CAM
```

---

## Step 5: Configure Board Settings

| Setting | Value |
|----------|-------|
| Upload Speed | 115200 |
| Flash Frequency | 40 MHz |
| Flash Mode | DIO |
| Partition Scheme | Huge APP (3MB No OTA / 1MB SPIFFS) |
| PSRAM | Enabled |
| Core Debug Level | None |

---

## Step 6: Select COM Port

Go to:

```
Tools → Port
```

Select the COM port corresponding to your USB-to-TTL adapter.

---

# 6. ESP32-CAM Code

Create a new Arduino sketch and paste the following code.

Before uploading, update these values:

```cpp
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

const char* PC_IP = "YOUR_PC_IP";

IPAddress local_IP(...);
IPAddress gateway(...);
```

Replace them with:

- Your Wi-Fi Name
- Your Wi-Fi Password
- Your PC's IPv4 Address
- Gateway Address
- Static IP for ESP32-CAM

---

## Required Libraries

```cpp
#include "esp_camera.h"
#include <WiFi.h>
#include "nvs_flash.h"
#include "camera_pins.h"
```

---

## Camera Model

```cpp
#define CAMERA_MODEL_AI_THINKER
```

---

## Wi-Fi Configuration

```cpp
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

const char* PC_IP = "10.189.70.10";
const int PC_PORT = 9000;
```

---

## Static IP Configuration

```cpp
IPAddress local_IP(10,189,70,50);
IPAddress gateway(10,189,70,227);
IPAddress subnet(255,255,255,0);
```

---

## Ultrasonic Sensor Pins

```cpp
#define TRIG_PIN 14
#define ECHO_PIN 13

#define DISTANCE_THRESHOLD_CM 100
```

---

## Global Variables

```cpp
bool motionDetected = false;

unsigned long lastMotionTime = 0;

WiFiClient client;
```

---

## Distance Measurement Function

The HC-SR04 measures the distance continuously.

```cpp
float getDistance()
{
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);

    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);

    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH, 30000);

    if(duration == 0)
        return 999;

    return (duration * 0.034) / 2.0;
}
```

---

## setup()

The `setup()` function performs the following tasks:

- Starts Serial Monitor
- Initializes Camera
- Initializes NVS Flash
- Configures Sensor Pins
- Sets Static IP
- Connects to Wi-Fi
- Prints ESP32 IP Address

```cpp
void setup()
{
    Serial.begin(115200);

    // Camera Initialization

    // NVS Initialization

    // Configure GPIO

    // Configure Static IP

    // Connect Wi-Fi

    // Print ESP32 IP
}
```

---

## loop()

The loop continuously:

1. Reads distance from HC-SR04
2. Detects motion
3. Starts streaming
4. Sends JPEG frames
5. Stops streaming after 5 seconds of no motion

Workflow:

```text
Measure Distance
        │
        ▼
Distance < 100 cm?
        │
   Yes──┴──No
        │
Start Streaming
        │
Capture JPEG
        │
Send Frame to PC
        │
Repeat
        │
No Motion for 5 sec
        │
Stop Streaming
```

---

## Frame Transmission

Each frame is transmitted as:

```
4 Bytes
(Frame Length)

↓

JPEG Image Bytes
```

The Python receiver first reads the frame length and then receives the JPEG image.

---

## Motion Detection Logic

```cpp
if(distance < DISTANCE_THRESHOLD_CM)
{
    motionDetected = true;
}
else
{
    if(no motion for 5 seconds)
    {
        Stop Streaming;
    }
}
```

---

## Streaming Process

```text
Motion Detected
       │
       ▼
Connect to Wi-Fi
       │
       ▼
Connect to PC
       │
       ▼
Capture Camera Frame
       │
       ▼
Send Frame
       │
       ▼
Repeat Until No Motion
```

---
