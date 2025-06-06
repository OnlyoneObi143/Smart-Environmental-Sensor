# 🌎 Smart Environmental Sensor Node

This project is a low-power, Wi-Fi–enabled IoT sensor node that collects real-time environmental data(temperature, humidity, and pressure) and publishes it securely to AWS IoT Core using MQTT.

Built with the ESP32 Feather V2 and a BME280 sensor, the system transmits structured JSON payloads over a TLS-encrypted connection. The device also includes a local OLED display for instant feedback without needing cloud access.

## 🔧 What It Does

- Reads live environmental data from a BME280 sensor every 10 seconds
- Displays values locally on a 128x64 OLED screen via I2C
- Sends structured JSON payloads to AWS IoT Core using MQTT
- Uses secure X.509 certificate-based authentication to connect to AWS
- Helps visualize sensor readings remotely in real-time

## 🧠 Key Features

- ✅ Real-time telemetry every 10 seconds
- 🔐 Secure TLS connection to AWS using X.509 certificates
- 📡 Publishes to custom MQTT topics for structured cloud ingestion
- 📟 Built-in OLED display for local debugging or monitoring

## 📦 Hardware Used

- ESP32 Feather V2
- BME280 Sensor Module (I2C)
- 0.96" OLED Display (SSD1306, I2C)
- Micro-USB cable 

## 🛠️ Software / Libraries

- Arduino IDE
- Adafruit BME280 Library
- Adafruit SSD1306 and GFX Libraries
- PubSubClient (for MQTT)
- AWS IoT Core (free-tier eligible)

## 🔐 AWS IoT Setup Overview

1. Created AWS IoT “Thing” and downloaded:
   - Root CA
   - Device Certificate
   - Private Key
2. Attached policy to allow publish/subscribe to `env/sensor`
3. Connected via port `8883` using secure `WiFiClientSecure`
4. Data payload format:
```json
{
  "temperature": 21.26 °C,
  "humidity": 46.65 %,
  "pressure": 991.53 hPa
}
