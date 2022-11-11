#include "waterTemperatureMachine.h"
#include <OneWire.h>

int waterTemperatureSensorPin = A5;
OneWire ds18b20(waterTemperatureSensorPin); // on pin 10 (a 4.7K resistor is necessary)
byte addr[8];

void initiateWaterTempratureSensor()
{
  ds18b20.reset(); // sensor is reset
  ds18b20.select(addr);
  ds18b20.write(0x44, 1); // start conversion, with parasite power on at the end
}

float readWaterTemperatureLevels()
{
  initiateWaterTempratureSensor();
  delay(1000);

  byte data[9];
  ds18b20.reset(); // sensor is reset
  ds18b20.select(addr);
  ds18b20.write(0xBE); // transfer data from EEROM to Scratchpad

  for (int i = 0; i < 9; i++)
  {                           // we need 9 bytes
    data[i] = ds18b20.read(); // read Scratchpad Memory of ds18b20
  }

  uint8_t myCrc = OneWire::crc8(data, 8); // calculate crc

  if (myCrc == data[8]) // compare : calculate with received
  {
    // Convert the data to actual temperature
    // because the result is a 16 bit signed integer, it should
    // be stored to an "int16_t" type, which is always 16 bits
    // even when compiled on a 32 bit processor.
    int16_t raw = (data[1] << 8) | data[0];
    return (float)raw / 16.0; // default is 12 bit resolution, 750 ms conversion time
  }
  else
  {
    return -1.0;
  }
}

bool isWaterTemperatureLevelSensorConnected()
{
  if (!ds18b20.search(addr)) // collect 64-bit (8-byte) ROM/Addres code Sensor (ds18b20)
  {
    ds18b20.reset_search();
    return false;
  }
  Serial.println("DS18B20 ROM Address =");
  for (int i = 0; i < 8; i++)
  {
    Serial.write(' ');
    Serial.print(addr[i], HEX);
  }
  return true;
}
