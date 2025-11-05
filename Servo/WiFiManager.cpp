#include "WiFiManager.h"

WiFiManager::WiFiManager(const char* ssid, const char* password, int maxAttempts)
    : ssid(ssid), password(password), maxAttempts(maxAttempts) {}

void WiFiManager::connectToWiFi() {
    Serial.println("Conectando a WiFi...");
    WiFi.begin(ssid, password);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < maxAttempts) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nConectado a WiFi");
        Serial.print("Dirección IP: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nError al conectar a WiFi");
    }
}

bool WiFiManager::isConnected() const {
    return WiFi.status() == WL_CONNECTED;
}