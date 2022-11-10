#include "soilMouistureLevelSensor.h"

#include <Arduino_MKRIoTCarrier.h>

int moistureLevelSensorPin = A5;

int readMoistureLevels()
{
  // read raw moisture value
  int raw_moisture = analogRead(moistureLevelSensorPin);

  // map raw moisture to a scale of 0 - 100
  return map(raw_moisture, 0, 1023, 100, 0);
}
