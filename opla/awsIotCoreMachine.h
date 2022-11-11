#pragma once

#include <ArduinoBearSSL.h>
#include <ArduinoECCX08.h>
#include <ArduinoMqttClient.h>
#include <WiFiNINA.h> // change to #include <WiFi101.h> for MKR1000
#include <ArduinoJson.h>

#include "arduino_secrets.h"

#include "sensorData.h"

unsigned long getTime(void);
void connectWiFi();
void connectMQTT();
void publishMessage();
void onMessageReceived(int messageSize);
void subscribeToMqttTopic(char *topic);
