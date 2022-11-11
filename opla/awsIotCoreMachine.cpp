#include "awsIotCoreMachine.h"

/////// Enter your sensitive data in arduino_secrets.h
const char ssid[] = SECRET_SSID;
const char pass[] = SECRET_PASS;
const char broker[] = SECRET_BROKER;
const char *certificate = SECRET_CERTIFICATE;

WiFiClient wifiClient;               // Used for the TCP socket connection
BearSSLClient sslClient(wifiClient); // Used for SSL/TLS connection, integrates with ECC508
MqttClient mqttClient(sslClient);

unsigned long getTime()
{
  // get the current time from the WiFi module
  return WiFi.getTime();
}

void connectWiFi()
{
  Serial.print("Attempting to connect to SSID: ");
  Serial.print(ssid);
  Serial.print(" ");

  while (WiFi.begin(ssid, pass) != WL_CONNECTED)
  {
    // failed, retry
    Serial.print(".");
    delay(8000);
  }
  Serial.println();

  Serial.println("You're connected to the network");
  Serial.println();
}

void connectMQTT()
{
  Serial.print("Attempting to MQTT broker: ");
  Serial.print(broker);
  Serial.println(" ");

  while (!mqttClient.connect(broker, 8883))
  {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }
  Serial.println();

  Serial.println("You're connected to the MQTT broker");
  Serial.println();
}

void subscribeToMqttTopic(char *topic)
{
  // subscribe to a topic
  mqttClient.subscribe(topic);
}

void publishMessage()
{
  // Check for size here: https://arduinojson.org/v6/assistant/#/step4
  StaticJsonDocument<64> doc;

  doc["humidity"] = mySensorData.humidity;
  doc["temperature"] = mySensorData.temperature;
  doc["waterTemprature"] = mySensorData.waterTemperature;
  doc["waterLevel"] = mySensorData.waterLevel;

  char out[128];

  int b = serializeJson(doc, out);
  Serial.print("bytes = ");
  Serial.println(b, DEC);
  Serial.println("Publishing message");

  // send message, the Print interface can be used to set the message contents
  mqttClient.beginMessage("hydroponic/test");
  mqttClient.print(out);
  mqttClient.endMessage();
}

void onMessageReceived(int messageSize)
{
  // we received a message, print out the topic and contents
  Serial.print("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");

  // use the Stream interface to print the contents
  while (mqttClient.available())
  {
    Serial.print((char)mqttClient.read());
  }
  Serial.println();

  Serial.println();
}
