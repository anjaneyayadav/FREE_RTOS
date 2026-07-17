#include "esp_camera.h"
#include <WiFi.h>
#include "nvs_flash.h"

#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"

const char* ssid     = "Lokesh";
const char* password = "Lokesh123";

// ← Your PC's IP address on the same WiFi network
const char* PC_IP = "10.59.130.10";
const int   PC_PORT  = 9000;

// ===== ADDED: static IP config (hotspot DHCP isn't assigning IPs) =====
IPAddress local_IP(10. 59. 130. 50);
IPAddress gateway(10. 59. 130. 225);
IPAddress subnet(255. 255. 255. 0);

#define TRIG_PIN 14
#define ECHO_PIN 13
#define DISTANCE_THRESHOLD_CM 100

bool motionDetected = false;
unsigned long lastMotionTime = 0;

WiFiClient client;

float getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return 999;
  return (duration * 0.034) / 2.0;
}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0       = Y2_GPIO_NUM;
  config.pin_d1       = Y3_GPIO_NUM;
  config.pin_d2       = Y4_GPIO_NUM;
  config.pin_d3       = Y5_GPIO_NUM;
  config.pin_d4       = Y6_GPIO_NUM;
  config.pin_d5       = Y7_GPIO_NUM;
  config.pin_d6       = Y8_GPIO_NUM;
  config.pin_d7       = Y9_GPIO_NUM;
  config.pin_xclk     = XCLK_GPIO_NUM;
  config.pin_pclk     = PCLK_GPIO_NUM;
  config.pin_vsync    = VSYNC_GPIO_NUM;
  config.pin_href     = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn     = PWDN_GPIO_NUM;
  config.pin_reset    = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode    = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location  = CAMERA_FB_IN_PSRAM;
  config.frame_size   = FRAMESIZE_QVGA;
  config.jpeg_quality = 12;
  config.fb_count     = 2;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed: 0x%x\n", err);
    return;
  }
  Serial.println("Camera init OK");

  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    nvs_flash_erase();
    ret = nvs_flash_init();
  }
  Serial.println("NVS OK");

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);

  // ===== ADDED: apply static IP before connecting =====
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("Static IP config failed!");
  }

  WiFi.begin(ssid, password);
  WiFi.setSleep(false);
  Serial.print("Connecting to WiFi");
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 40) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  Serial.println("\nWiFi connected!");

  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("Connecting to PC at ");
  Serial.println(PC_IP);
}

void loop() {
  float dist = getDistance();
  Serial.print("Distance: ");
  Serial.print(dist);
  Serial.println(" cm");

  if (dist > 0 && dist < DISTANCE_THRESHOLD_CM) {
    lastMotionTime = millis();
    if (!motionDetected) {
      Serial.println(">>> Motion detected! Starting stream...");
      motionDetected = true;
    }
  } else {
    if (motionDetected && millis() - lastMotionTime > 5000) {
      Serial.println(">>> No motion for 5s. Stopping stream.");
      motionDetected = false;
      client.stop();
    }
  }

  if (motionDetected) {
    if (!client.connected()) {
      Serial.println("Connecting to PC...");
      if (!client.connect(PC_IP, PC_PORT)) {
        Serial.println("Connection failed. Retrying...");
        delay(1000);
        return;
      }
      Serial.println("Connected to PC!");
    }

    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      delay(100);
      return;
    }

    uint32_t len = fb->len;
    client.write((uint8_t*)&len, 4);
    client.write(fb->buf, fb->len);
    esp_camera_fb_return(fb);
  }

  delay(100);
}