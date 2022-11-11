#include <Arduino_MKRIoTCarrier.h>
#include "visuals.h"
#include "sensorData.h"
#include "waterTemperatureMachine.h"
#include "awsIotCoreMachine.h"

MKRIoTCarrier carrier;
extern WiFiClient wifiClient;   // Used for the TCP socket connection
extern BearSSLClient sslClient; // Used for SSL/TLS connection, integrates with ECC508
extern MqttClient mqttClient;
extern const char *certificate;
sensorDataStruct mySensorData;

// Declare pages
void printHumidity(void);
void printTemprature(void);
void printWaterTemperatureLevel(void);
void printInitialMessage(void);
void printWiFiMessage();

void setup()
{
  CARRIER_CASE = false;
  Serial.begin(921600);
  delay(2500);

  carrier.begin();
  carrier.display.setRotation(0);

  if (!isWaterTemperatureLevelSensorConnected())
  {
    Serial.println("ERROR: NO DS18B20 Sensor.");
    while (1)
      ;
  }

  if (!ECCX08.begin())
  {
    Serial.println("ERROR: No ECCX08 present!");
    while (1)
      ;
  }

  // Set a callback to get the current time
  // used to validate the servers certificate
  ArduinoBearSSL.onGetTime(getTime);

  // Set the ECCX08 slot to use for the private key
  // and the accompanying public certificate for it
  sslClient.setEccSlot(0, certificate);

  // Optional, set the client id used for MQTT,
  // each device that is connected to the broker
  // must have a unique client id. The MQTTClient will generate
  // a client id for you based on the millis() value if not set
  mqttClient.setId("hydroponicSystem");

  // Set the message callback, this function is
  // called when the MQTTClient receives a message
  mqttClient.onMessage(onMessageReceived);
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    printWiFiMessage();
    connectWiFi();
  }

  if (!mqttClient.connected())
  {
    // MQTT client is disconnected, connect
    connectMQTT();
  }

  // // poll for new MQTT messages and send keep alives
  // mqttClient.poll();
  carrier.display.enableDisplay(true);
  printInitialMessage();

  readSensors();

  printHumidity();
  delay(2000);
  printTemprature();
  delay(2000);
  printWaterTemperatureLevel();
  delay(2000);
  carrier.display.fillScreen(0x0000);
  carrier.display.enableDisplay(false);
  publishMessage();
  delay(5000);
}

void readSensors()
{
  mySensorData.humidity = carrier.Env.readHumidity();       // storing humidity value in a variable
  mySensorData.temperature = carrier.Env.readTemperature(); // storing temprature value in a variable
  mySensorData.waterTemperature = readWaterTemperatureLevels();
}

void printWiFiMessage()
{

  carrier.display.fillScreen(0x0000);
  carrier.display.setCursor(25, 50);
  carrier.display.setTextSize(2);
  carrier.display.println("Connecting to");
  carrier.display.println("WiFi");
  carrier.display.drawBitmap(50, 90, wifiLogo, 128, 128, 0x070A);
  // Printing a three dots animation
  for (int i = 0; i < 3; i++)
  {
    carrier.display.print(".");
    delay(1000);
  }
}

void printInitialMessage()
{

  carrier.display.fillScreen(0x0000);
  carrier.display.setCursor(25, 50);
  carrier.display.setTextSize(2);
  carrier.display.print("Reading Sensors");
  carrier.display.drawBitmap(65, 85, hydroPonicBitMap, 115, 150, 0x070A);
  // Printing a three dots animation
  for (int i = 0; i < 3; i++)
  {
    carrier.display.print(".");
    delay(1000);
  }
}

void printHumidity()
{

  carrier.display.fillScreen(0x0000);
  carrier.display.setCursor(54, 40);
  carrier.display.setTextSize(3);
  carrier.display.print("Humidity");
  carrier.display.drawBitmap(70, 70, humidity_logo, 100, 100, 0x0D14); // 0x0CA1A6); //draw a humidity figure on the MKR IoT carrier's display
  carrier.display.setCursor(60, 180);
  carrier.display.print(mySensorData.humidity);
  carrier.display.print(" %");
}

void printTemprature()
{

  carrier.display.fillScreen(0x0000);
  carrier.display.setCursor(25, 60);
  carrier.display.setTextSize(3);
  carrier.display.print("Temperature");
  carrier.display.drawBitmap(80, 80, temperature_logo, 100, 100, 0xDAC9);
  carrier.display.setCursor(60, 180);
  carrier.display.print(mySensorData.temperature);
  carrier.display.print(" C");
}

void printWaterTemperatureLevel()
{

  carrier.display.fillScreen(0x0000);
  carrier.display.setCursor(25, 60);
  carrier.display.setTextSize(2);
  carrier.display.print("Water Temprature");
  carrier.display.drawBitmap(60, 80, waterTemperatureLogo, 100, 108, 0xFD4C); // 0xDA5B4A); //draw a thermometer on the MKR IoT carrier's display
  carrier.display.setTextSize(2);
  carrier.display.setCursor(80, 200);
  carrier.display.print(mySensorData.waterTemperature); // display the temperature on the screen
  carrier.display.print(" C");
}
