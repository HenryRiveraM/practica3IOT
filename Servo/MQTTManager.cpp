#include "MQTTManager.h"

MQTTManager::MQTTManager(const char* broker, int port, const char* clientId, const char* topicSub, ServoController& servoController) 
    : espClient(), client(espClient), broker(broker), port(port), clientId(clientId), topicSub(topicSub), servoController(servoController) {}

void MQTTManager::setupMQTT() {
    client.setServer(broker, port);
    client.setCallback([this](char* topic, byte* payload, unsigned int length) {
        this->callback(topic, payload, length);
    });
}

bool MQTTManager::isConnected() {
    return client.connected();
}

void MQTTManager::reconnectMQTT() {
    while (!client.connected()) {
        Serial.print("Intentando conexión MQTT...");
        if (client.connect(clientId)) {
            Serial.println("Conectado");
            client.subscribe(topicSub);
            Serial.print("Suscrito al topic: ");
            Serial.println(topicSub);
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

void MQTTManager::callback(char* topic, byte* payload, unsigned int length) {
    String mensaje;
    for (int i = 0; i < length; i++) {
        mensaje += (char)payload[i];
    }

    Serial.print("Mensaje recibido: ");
    Serial.println(mensaje);

    if (mensaje == "INTENTO_DE_APERTURA") {
        Serial.println("Reconociendo patrón de audio...");
        servoController.openLock();
        
     //   Serial.println("Cerrando de forma automática");
     }else{
        delay(2000);
        servoController.closeLock();
     }
}