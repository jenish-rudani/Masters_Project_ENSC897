#ifndef SENSORDATA_H
#define SENSORDATA_H

typedef struct sensorDataStruct
{
  float humidity;
  float temperature;
  float waterTemperature;
  int waterLevel;
} sensorDataStruct;

//Global data that will be defined in main.c
extern sensorDataStruct mySensorData;

#endif
