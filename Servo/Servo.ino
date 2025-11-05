#include <Arduino.h>
#include "WiFiManager.h"
#include "MQTTManager.h"
#include "ServoController.h"

// Configuración
WiFiManager wifiManager("RIVERA WIFI 2.4", "2880203CB.");
ServoController servoController(27); // Pin para el servo
MQTTManager mqttManager("test.mosquitto.org", 1883, "ESP32_ACTUADOR", "ucb/lock/request", servoController);

void setup() {
    Serial.begin(115200);
    servoController.initialize();
    wifiManager.connectToWiFi();
    mqttManager.setupMQTT();
}

void loop() {
    if (!wifiManager.isConnected()) {
        wifiManager.connectToWiFi(); // Reconectar WiFi primero
        delay(1000); // Esperar antes de intentar MQTT
    }

    if (!mqttManager.isConnected() && wifiManager.isConnected()) { // Solo intentar MQTT si WiFi está activo
        mqttManager.reconnectMQTT();
    }
    mqttManager.loop();
}
