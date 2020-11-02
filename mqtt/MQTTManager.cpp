#ifndef mqtt_manager
#define mqtt_manager

#include <Arduino.h>
#include "ExecutionTicker.cpp"
//#include <ESP8266WiFi.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <functional>

// Caution. Setting this with a low value could cause that never reconnects to wifi.
// Looks like it is necessary to give "some time" to the client to process a try to connect action
#define WIFI_CONNECTION_VERIFIER_TIME 5000
#define MQTT_CONNECTION_VERIFIER_TIME 6000

class MQTTManager
{
private:
    const char *ssid, *ssidPassword, *mqttClientId, *mqttServer, *mqttUser, *mqttPassword;
    int mqttPort;
    ExecutionTicker *wifiConnectionVerifier;
    ExecutionTicker *mqttConnectionVerifier;

    WiFiClient wifiClient;
    PubSubClient mqttClient = PubSubClient(wifiClient);

    std::function<void(boolean connected)> wifiConnectionStateCallback = NULL;
    std::function<void(boolean connected)> mqttConnectionStateCallback = NULL;

    boolean previouslyWifiConnected = false;
    boolean previouslyMqttConnected = false;

    void init()
    {
        WiFi.begin(ssid, ssidPassword);
        wifiConnectionVerifier = new ExecutionTicker(WIFI_CONNECTION_VERIFIER_TIME,
                                                     std::bind(&MQTTManager::reconnectWifi, this));
        mqttConnectionVerifier = new ExecutionTicker(MQTT_CONNECTION_VERIFIER_TIME,
                                                     std::bind(&MQTTManager::reconnectMqtt, this));
        mqttClient.setServer(mqttServer, mqttPort);
    }

    void reconnectWifi()
    {
        Serial.print("WiFi: ");
        Serial.print(ssid);
        Serial.print(" ");
        auto status = WiFi.status();
        Serial.print(status);
        Serial.print(" ");
        if (status == WL_CONNECTED)
        {
            Serial.println("CONNECTED");
            if (previouslyWifiConnected == false && wifiConnectionStateCallback != NULL)
            {
                wifiConnectionStateCallback(true);
            }
            previouslyWifiConnected = true;
            return;
        }
        else
        {
            if (previouslyWifiConnected == true && wifiConnectionStateCallback != NULL)
            {
                wifiConnectionStateCallback(false);
            }
            previouslyWifiConnected = false;
            if (status == WL_IDLE_STATUS)
            {
                Serial.println("ATTEMPTING TO CONNECT");
                return;
            }
            Serial.println("DISCONNECTED. Trying to reconnect...");
            WiFi.begin(ssid, ssidPassword);
        }
    }

    void reconnectMqtt()
    {
        Serial.print("MQTT server ");
        Serial.print(mqttServer);
        Serial.print(":");
        Serial.print(mqttPort);
        Serial.print(" ");
        if (mqttClient.connected())
        {
            if (!previouslyMqttConnected && mqttConnectionStateCallback != NULL)
            {
                mqttConnectionStateCallback(true);
            }
            previouslyMqttConnected = true;
            Serial.println("CONNECTED");
        }
        else
        {
            if (previouslyMqttConnected && mqttConnectionStateCallback != NULL)
            {
                mqttConnectionStateCallback(false);
            }
            previouslyMqttConnected = false;
            Serial.println("DISCONNECTED. Trying to reconnect...");
            syncConnectToMqttServer();
        }
    }

    // This is a synchronous task, and takes a while, like 10segs (for the server not connected scenario)
    void syncConnectToMqttServer()
    {
        //WiFi.mode(WIFI_STA);
        Serial.print("address: ");
        Serial.print(WiFi.localIP());
        mqttClient.connect(mqttClientId);
    }

public:
    MQTTManager(
        const char *_ssid,
        const char *_ssidPassword,
        const char *_mqttServer,
        const int _mqttPort,
        const char *_mqttClientId,
        const char *_mqttUser = "",
        const char *_mqttPassword = "",
        std::function<void(boolean)> _wifiConnectionCallback = NULL,
        std::function<void(boolean)> _mqttConnectionCallback = NULL)
    {
        ssid = _ssid;
        ssidPassword = _ssidPassword;
        mqttServer = _mqttServer;
        mqttPort = _mqttPort;
        mqttUser = _mqttUser;
        mqttPassword = _mqttPassword;
        mqttClientId = _mqttClientId;
        wifiConnectionStateCallback = _wifiConnectionCallback;
        mqttConnectionStateCallback = _mqttConnectionCallback;
        init();
    }

    void tick()
    {
        wifiConnectionVerifier->tick();
        mqttConnectionVerifier->tick();
        mqttClient.loop();
    }

    void setListener(std::function<void(char *topic, byte *payload, long length)> topicCallback)
    {
        mqttClient.setCallback(topicCallback);
    }

    void sendMessage(const char *topic, const char *message)
    {
        mqttClient.publish(topic, message);
    }

    void setMqttCallback(std::function<void(boolean)> mqttStateCallback)
    {
        mqttConnectionStateCallback = mqttStateCallback;
    }

    void subscribeToTopic(const char *topic)
    {
        mqttClient.subscribe(topic);
    }
};

#endif
