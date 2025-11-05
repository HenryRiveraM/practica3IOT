#include "MQTTManager.h"

MQTTManager::MQTTManager(WiFiClient& wifiClient, const char* broker, int port, 
                        const char* clientId, const char* topic)
    : client(wifiClient), broker(broker), port(port), 
      clientId(clientId), topic(topic) {
    client.setServer(broker, port);
}

void MQTTManager::connect() {
    if (!client.connected()) {
        reconnect();
    }
}

void MQTTManager::reconnect() {
    while (!client.connected()) {
        Serial.print("Intentando conexión MQTT...");
        if (client.connect(clientId)) {
            Serial.println("Conectado");
            client.subscribe(topic);
        } else {
            Serial.print("Falló, rc=");
            Serial.print(client.state());
            Serial.println(" Intentando nuevamente en 5 segundos");
            delay(5000);
        }
    }
}

void MQTTManager::loop() {
    client.loop();
}

void MQTTManager::setCallback(void (*callback)(char*, byte*, unsigned int)) {
    client.setCallback(callback);
}

void MQTTManager::publish(const char* message) {
    client.publish(topic, message);
}

bool MQTTManager::isConnected() {
    return client.connected();
}