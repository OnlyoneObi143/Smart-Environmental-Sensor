#include <Wire.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_BME280.h>

// OLED settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// BME280 sensor
Adafruit_BME280 bme;

// WiFi credentials
const char* ssid = "*********";
const char* password = "*******";

// AWS IoT Core endpoint
const char* mqtt_server = "****************";

// Certificates
const char* root_ca = \
"-----BEGIN CERTIFICATE-----\n" \
"...root cert...\n" \
"-----END CERTIFICATE-----\n";

// Client Certificate
const char* client_cert = \
"-----BEGIN CERTIFICATE-----\n" \
"...client cert...\n" \
"-----END CERTIFICATE-----\n";

// Private Key
const char* client_key = \
"-----BEGIN RSA PRIVATE KEY-----\n" \
"...private key...\n" \
"-----END RSA PRIVATE KEY-----\n";

// WiFi + MQTT
WiFiClientSecure net;
PubSubClient client(net);

// Connects to your WiFi network
void setup_wifi() {
  delay(10);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

// Reconnects to AWS IoT Core if MQTT connection is lost
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("Connected to AWS");
      client.publish("env/sensor", "Test message from ESP32");
    } else {
      Serial.print("Failed. Code=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // OLED init
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED not found");
    while (true);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // BME280 init
  if (!bme.begin(0x76)) {
    Serial.println("BME280 not found");
    display.println("Sensor error");
    display.display();
    while (true);
  }

  setup_wifi();

  
  // Set secure certs for AWS IoT
  net.setCACert(root_ca);
  net.setCertificate(client_cert);
  net.setPrivateKey(client_key);
  client.setServer(mqtt_server, 8883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  static unsigned long lastSend = 0;
  if (millis() - lastSend > 10000) {
    lastSend = millis();

    float temp = bme.readTemperature();
    float hum = bme.readHumidity();
    float pres = bme.readPressure() / 100.0F;

    // Display on OLED
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Env Sensor Readings:");
    display.print("Temp: "); display.print(temp); display.println(" °C");
    display.print("Hum:  "); display.print(hum); display.println(" %");
    display.print("Pres: "); display.print(pres); display.println(" hPa");
    display.display();

    // Format JSON payload
    String payload = "{";
    payload += "\"temperature\":\"" + String(temp, 2) + " °C\",";
    payload += "\"humidity\":\"" + String(hum, 2) + " %\",";
    payload += "\"pressure\":\"" + String(pres, 2) + " hPa\"";
    payload += "}";

    client.publish("env/sensor", payload.c_str());
    Serial.println("Sent: " + payload);
  }
}
