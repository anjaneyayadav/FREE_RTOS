**ESP32-CAM Motion-Triggered Wi-Fi Security Camera**

*A Complete Beginner\'s Build Guide*

# 1. Project Objective

To design and build a low-cost, motion-triggered Wi-Fi camera system using an ESP32-CAM and an ultrasonic sensor, which automatically detects nearby motion, streams live video to a computer over Wi-Fi, and saves each detected event as a video clip --- without needing continuous recording or constant human monitoring.

Specific goals:

-   Detect motion reliably using an ultrasonic distance sensor, rather than relying on continuous video analysis.

-   Conserve power and bandwidth by activating the camera and Wi-Fi streaming only when motion is detected.

-   Stream live video wirelessly from the ESP32-CAM to a PC in real time.

-   Automatically save each motion event as a timestamped video clip.

-   Use only low-cost, easily available hardware --- no proprietary security camera hardware or cloud subscriptions.

# 2. What You\'re Building

This project builds a small Wi-Fi security camera. An ultrasonic distance sensor watches for something coming close. When it does, the ESP32-CAM automatically wakes up its camera and streams live video over Wi-Fi to a program running on your PC, which displays the video on screen and saves each event as a video clip.

How it works, in order:

-   The ultrasonic sensor (HC-SR04) continuously measures distance.

-   When an object comes within 100 cm, the ESP32-CAM treats this as "motion detected."

-   The ESP32-CAM connects to your PC over WiFi and starts sending live camera frames.

-   A Python program on your PC receives those frames, shows them in a live window, and saves them as an .mp4 clip.

-   When nothing is detected for 5 seconds, the ESP32-CAM stops streaming and disconnects until the next motion event.

# 3. **Required Hardware Components**

-   AI Thinker ESP32-CAM board

-   HC-SR04 ultrasonic distance sensor

-   USB-to-TTL (FTDI / CP2102 / CH340) adapter, for programming and power

-   Jumper wires

-   Breadboard

-   A PC with Arduino IDE and Python installed

-   Both the ESP32-CAM and the PC must be on the same Wi-Fi network

> *Note: Do not power the ESP32-CAM from a Raspberry Pi or similar single-board computer\'s 5V pin. It usually cannot supply enough current for the camera, WiFi radio, and sensor running together, and will cause random crashes ("brownout" resets).*

# ![Uploading image.png…]()



# 4. Connections

## 4.1 HC-SR04 to ESP32-CAM

  -----------------------------------------------------------------------
  **HC-SR04 Pin**                     **ESP32-CAM Pin**
  ----------------------------------- -----------------------------------
  VCC                                 5V

  GND                                 GND

  TRIG                                GPIO14

  ECHO                                GPIO13
  -----------------------------------------------------------------------

> *Note: GPIO14 and GPIO13 are used deliberately. GPIO16 must never be used for the sensor on this board --- it is internally used by the camera\'s PSRAM chip, and reconfiguring it as a sensor pin corrupts memory and crashes the board the moment the camera and WiFi are both active.*

## 

## 4.2 USB-TTL Adapter to ESP32-CAM (power + programming)

  -----------------------------------------------------------------------
  **USB-TTL Pin**                     **ESP32-CAM Pin**
  ----------------------------------- -----------------------------------
  5V                                  Don't connect 5V, Leave this Pin

  GND                                 GND

  TX                                  U0R (RX)

  RX                                  U0T (TX)
  -----------------------------------------------------------------------

> *Note: Use the USB-TTL adapter\'s 5V pin, not its 3.3V pin. Do not use its 3.3V line --- the ESP32-CAM needs true 5V on its 5V pin.*

## 4.3 Flashing Mode Jumper (only while uploading code)

Connect GPIO0 to GND only while uploading new code. Remove this wire immediately after uploading finishes --- leaving it connected will prevent the board from running normally.

## 4.4 Common Ground 

If you power the sensor from a separate external 5V supply instead of the USB-TTL adapter, make sure every ground (GND) in the circuit is tied together, otherwise readings and connections will be unreliable:

-   ESP32-CAM GND

-   HC-SR04 GND

-   External power supply GND

-   USB-TTL adapter GND

## 4.4 Common Power supply (5V)

-   ESP32-CAM 5V

-   HC-SR04 5V

-   External power supply 5V

# 5. Arduino IDE Setup

1.  Install Arduino IDE 2.x from the official site: [arduino.cc/en/software.](https://www.arduino.cc/en/software)

2.  Go to File → Preferences, and under "Additional Boards Manager URLs" add:

> [**https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json**](https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json)

3.  Go to Tools → Board → Boards Manager, search "esp32", and install esp32 by Espressif Systems, version 2.0.11 (newer 3.x versions have known Wi-Fi stability bugs on this board).

4.  Select Tools → Board → ESP32 Arduino → AI Thinker ESP32-CAM.

5.  Set the remaining board options as shown below.

6.  Set Tools → Port to the COM port your USB-TTL adapter shows up as.

## 

## 

## 5.1 Board Settings

  ------------------------------------------------------------------------
  **Setting**                         **Value**
  ----------------------------------- ------------------------------------
  Upload Speed                        115200

  Flash Frequency                     40 MHz

  Flash Mode                          DIO

  Partition Scheme                    Huge APP (3MB No OTA / 1MB SPIFFS)

  PSRAM                               Enabled

  Core Debug Level                    None
  ------------------------------------------------------------------------

# 6. ESP32-CAM Code

Create a new sketch in Arduino IDE and paste in the following code. Before uploading, update these three lines to match your own network (see Section 8 for how to find these values):

-   ssid and password --- your Wi-Fi network name and password

-   PC_IP --- your PC\'s current IP address

-   local_IP and gateway --- a free static IP for the ESP32-CAM, and your network\'s gateway address

*(Arduino code omitted)*

# 7. Flashing the Code

1.  Disconnect the HC-SR04 sensor completely before flashing.

2.  Connect GPIO0 to GND (flashing mode).

3.  Power off the board, then click Upload in Arduino IDE.

4.  When the IDE shows "Connecting\...", power the board back on.

![](media/image2.png){width="6.268055555555556in" height="1.2666666666666666in"}

5.  Wait for the upload to finish successfully.

![](media/image3.png){width="6.268055555555556in" height="1.7194444444444446in"}

6.  Remove the GPIO0-to-GND wire.

7.  Open the Serial Monitor.

8.  Set the baud rate to "115200".

9.  Power cycle the board (unplug and re-plug power).

10. The Serial Monitor will begin displaying the boot messages and program logs.

![](media/image4.png){width="6.270049212598425in" height="1.5206430446194226in"}

11. Reconnect the HC-SR04 sensor now that flashing is complete.

# 8. Finding Your Network Settings

On your PC (Windows), open Command Prompt and run:

ipconfig /all

Look under your active WiFi adapter for these three values:

-   IPv4 Address --- this is your PC\'s IP. Use it as PC_IP in the ESP32 code.

-   Default Gateway --- use this as the gateway value in the ESP32 code.

-   Subnet Mask --- normally 255.255.255.0, matches the subnet value already in the code.

For local_IP, pick any unused address on the same subnet as your PC (for example, if your PC is 10.189.70.10, you can use 10.189.70.50 for the ESP32-CAM).

> *Note: If you ever reconnect to Wi-Fi and the connection stops working, your PC\'s IP address may have changed. Re-run ipconfig /all, and update PC_IP, local_IP, and gateway in the ESP32 code to match, then re-upload.*

# 8. Python Receiver (PC side) 

Install the required Python packages once, from Command Prompt:

pip install opencv-python numpy

Create a file named receiver.py and paste in the following code:

*(Python code omitted)*

To run it, open Command Prompt in the folder containing this file and run:

python receiver.py

It will print "Waiting for ESP32-CAM to connect\..." and wait. Saved video clips will appear automatically in a folder named clips, created next to the script.

# 10. Running the Full System

1.  On your PC, run: python receiver.py, and leave it running.

2.  Power on the ESP32-CAM (with the sensor connected).

3.  Open Arduino IDE\'s Serial Monitor at 115200 baud to watch its status.

4.  Wait for Serial Monitor to show Wi-Fi connected! and an IP address.

5.  Wave your hand within about 100 cm of the HC-SR04 sensor.

6.  Serial Monitor should show "Motion detected! Starting stream\..." and "Connected to PC!"

7.  A window titled "ESP32-CAM Motion Stream" should appear on your PC showing live video.

8.  After you move away and 5 seconds pass with no motion, streaming stops automatically and the clip is saved.

9.  Press q with the video window focused, or Ctrl+C in the terminal, to stop the receiver program safely.

# 11. Troubleshooting Guide

+----------------------------------------------------------------------------------------+-----------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------------+
| **Symptom**                                                                            | **Likely Cause**                                                                        | **Fix**                                                                                         |
+========================================================================================+=========================================================================================+=================================================================================================+
| Board keeps rebooting / crashes right after boot                                       | Wrong GPIO used for the sensor, or camera initialized before power stabilized           | Use GPIO14 for TRIG (never GPIO16), and make sure the camera init code runs early and cleanly   |
+----------------------------------------------------------------------------------------+-----------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------------+
| "Guru Meditation Error" right when Wi-Fi starts                                        | Wi-Fi\'s internal storage (NVS) is missing or corrupted, often after a flash erase      | Make sure the nvs_flash_init() block is present near the top of setup ()                        |
+----------------------------------------------------------------------------------------+-----------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------------+
| Crash only happens when camera + Wi-Fi + sensor all run together, but each works alone | GPIO16 is used internally by the camera\'s PSRAM chip and conflicts with sensor use     | Move the sensor\'s TRIG wire to GPIO14 instead of GPIO16                                        |
+----------------------------------------------------------------------------------------+-----------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------------+
| Wi-Fi shows "connected" but IP address is 0.0.0.0                                      | The Wi-Fi network\'s DHCP server isn\'t assigning an address (common on phone hotspots) | Assign a static IP in code using                                                                |
|                                                                                        |                                                                                         |                                                                                                 |
|                                                                                        |                                                                                         | Wi-Fi. Config (), instead of relying on DHCP                                                    |
+----------------------------------------------------------------------------------------+-----------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------------+
| "Brownout detector was triggered" when the sensor is connected                         | Not enough stable power for                                                             | Power the ESP32-CAM and sensor from the USB-TTL adapter\'s 5V line instead                      |
|                                                                                        |                                                                                         |                                                                                                 |
|                                                                                        | camera + Wi-Fi + sensor together, often from a shared board like a Raspberry Pi         |                                                                                                 |
+----------------------------------------------------------------------------------------+-----------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------------+
| "Connection failed, retrying\..." when trying to stream                                | The PC\'s IP address changed, or receiver.py isn\'t running                             | Re-check ipconfig on the PC, update PC_IP in the ESP32 code, and confirm receiver.py is running |
+----------------------------------------------------------------------------------------+-----------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------------+
| Saved video clip won\'t open / play                                                    | The Python script was force-closed before it could finalize the file                    | Always stop the script with q or Ctrl+C so the video file gets closed properly                  |
+----------------------------------------------------------------------------------------+-----------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------------+

# 12. Tips for Going Further

-   Improve image quality by lowering jpeg_quality in the ESP32 code (lower number = higher quality, range 0--63).

-   Adjust the FPS value in receiver.py if saved clips look sped up or slowed down compared to real time.

-   Change DISTANCE_THRESHOLD_CM in the ESP32 code to make the sensor more or less sensitive to distance.

-   Consider giving your PC a static IP on your router/hotspot, so PC_IP in the ESP32 code never needs updating again.
