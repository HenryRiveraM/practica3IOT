#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <PubSubClient.h>
#include <WiFi.h>
#include "ServoController.h"

class MQTTManager {
private:
    WiFiClient espClient;  // Cliente WiFi como miembro
    PubSubClient client;
    const char* broker;
    const int port;
    const char* clientId;
    const char* topicSub;
    ServoController& servoController;

    static void callbackWrapper(char* topic, byte* payload, unsigned int length, void* userdata);
    void callback(char* topic, byte* payload, unsigned int length);

public:
    MQTTManager(const char* broker, int port, const char* clientId, const char* topicSub, ServoController& servoController);
    void setupMQTT();
    bool isConnected();
    void reconnectMQTT();
    void loop();
};

#endif