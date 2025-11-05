#include "WiFiManager.h"

WiFiManager::WiFiManager(const char* ssid, const char* password) 
    : ssid(ssid), password(password) {}

void WiFiManager::connect() {
    WiFi.begin(ssid, password);
    Serial.print("Conectando a WiFi");
    while (!isConnected()) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi conectado");
    Serial.print("Dirección IP: ");
    Serial.println(WiFi.localIP());
}

bool WiFiManager::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}