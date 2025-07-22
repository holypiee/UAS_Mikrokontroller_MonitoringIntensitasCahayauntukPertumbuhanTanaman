#include <DHT.h>
#include <WiFi.h>
#include <HTTPClient.h>

// --- WiFi ---
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASS ""

// --- ThingSpeak ---
const char* thingspeak_api_key = "SPQ617BT4LBSHHXP";
const char* thingspeak_server = "http://api.thingspeak.com/update";

// --- Sensor Pins ---
#define DHT_PIN 2
#define DHT_TYPE DHT22
#define LDR_PIN 32
#define POT_PIN 33

DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected.");
}

void loop() {
  // Baca data dari sensor
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  int ldrValue = analogRead(LDR_PIN);
  int lightIntensity = map(ldrValue, 0, 4095, 1000, 0);  // Semakin terang, nilai semakin tinggi

  int soilMoisture = analogRead(POT_PIN);
  int soilMoisturePercent = map(soilMoisture, 0, 4095, 0, 100);

  // Tampilkan di Serial Monitor
  Serial.println("Temperature: " + String(temperature));
  Serial.println("Humidity: " + String(humidity));
  Serial.println("Light Intensity: " + String(lightIntensity));
  Serial.println("Soil Moisture: " + String(soilMoisturePercent));

  // Kirim ke ThingSpeak
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(thingspeak_server) + "?api_key=" + thingspeak_api_key +
                 "&field1=" + String(temperature) +
                 "&field2=" + String(humidity) +
                 "&field3=" + String(lightIntensity) +
                 "&field4=" + String(soilMoisturePercent);

    http.begin(url);
    int httpCode = http.GET();

    if (httpCode > 0) {
      Serial.println("Data sent to ThingSpeak. Code: " + String(httpCode));
    } else {
      Serial.println("Error sending data: " + http.errorToString(httpCode));
    }
    http.end();
  }

  delay(15000);  // Minimal delay 15 detik sesuai aturan ThingSpeak
}
