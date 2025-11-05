#include "WiFiManager.h"
#include "MQTTManager.h"
#include "SoundDetector.h"

// Configuración WiFi
const char* WIFI_SSID = "RIVERA WIFI 2.4";
const char* WIFI_PASS = "2880203CB.";

// Configuración MQTT
const char* MQTT_BROKER = "test.mosquitto.org";
const int MQTT_PORT = 1883;
const char* MQTT_CLIENT_ID = "ESP32_SENSOR";
const char* MQTT_TOPIC = "ucb/lock/request";

// Configuración del sensor de sonido
const int SOUND_SENSOR_PIN = 35;
const int SOUND_THRESHOLD = 2300;
const int MAX_TIME_BETWEEN_PEAKS = 800;
const int REQUIRED_PEAKS = 3;

// Instancias de las clases
WiFiManager wifiManager(WIFI_SSID, WIFI_PASS);
WiFiClient wifiClient;
MQTTManager mqttManager(wifiClient, MQTT_BROKER, MQTT_PORT, MQTT_CLIENT_ID, MQTT_TOPIC);
SoundDetector soundDetector(SOUND_SENSOR_PIN, SOUND_THRESHOLD, 
                          MAX_TIME_BETWEEN_PEAKS, REQUIRED_PEAKS, 
                          mqttManager);

void setup() {
  Serial.begin(115200);
  delay(300);

  analogSetPinAttenuation(SOUND_SENSOR_PIN, ADC_11db);
  pinMode(SOUND_SENSOR_PIN, INPUT);

  wifiManager.connect();
  mqttManager.connect();
  Serial.println("Sistema de detección de sonido inicializado");
}

void loop() {
    if (!wifiManager.isConnected()) {
        wifiManager.connect();
    }
    mqttManager.loop();
    soundDetector.monitor();
}