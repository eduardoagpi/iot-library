#include <Arduino.h>
#include <MQTTManager.cpp>

// ----------------- Function Prototypes--------------------------------------
void setupMqtt();
void mqttConnectionStatusChanged(boolean connected);
void subscribeToCommandChannels();
void mqttMessageReceived(char *topic, byte *mqttmessage, unsigned int length);
//-----------------------------------------------------------------------------

MQTTManager mqttManager = MQTTManager("SSID", "password", "192.168.1.76", 1883, "device_id");

void setup()
{
  Serial.begin(9600);
  setupMqtt();
}

void loop()
{
  mqttManager.tick();
}

void setupMqtt()
{
  mqttManager.setMqttCallback(std::bind(&mqttConnectionStatusChanged, std::placeholders::_1));
  mqttManager.setListener(std::bind(&mqttMessageReceived, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void mqttConnectionStatusChanged(boolean connected)
{
  if (connected)
  {
    Serial.print("Connected to MQTT");
    subscribeToCommandChannels();
  }
}

void subscribeToCommandChannels()
{
  mqttManager.subscribeToTopic("/#");
}

void mqttMessageReceived(char *topic, byte *mqttmessage, unsigned int length)
{
  Serial.println("Message arrived to channel: ");
  Serial.println(topic);

  char message[length + 1];
  memcpy(message, mqttmessage, length);
  message[length] = '\0';

  Serial.println("Payload: ");
  Serial.print("<<");
  Serial.print(message);
  Serial.println(">>");
}
