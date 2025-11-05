#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <PubSubClient.h>
#include <WiFiClient.h>

class MQTTManager {
private:
    PubSubClient client;
    const char* broker;
    int port;
    const char* clientId;
    const char* topic;
    
    void reconnect();
    
public:
    MQTTManager(WiFiClient& wifiClient, const char* broker, int port, 
               const char* clientId, const char* topic);
               
    void connect();
    void loop();
    void setCallback(void (*callback)(char*, byte*, unsigned int));
    void publish(const char* message);
    bool isConnected();
};

#endif