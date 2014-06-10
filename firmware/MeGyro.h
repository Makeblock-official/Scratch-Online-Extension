#ifndef MEGYRO_H_
#define MEGYRO_H_
#include "MeWire.h" 
#include <Arduino.h>
class MeGyro{
  public:
      static void init();
      static void setClockSource();
      static void setFullScaleGyroRange();
      static void setFullScaleAccelRange();
      static void setSleepEnabled();
      static uint8_t getDeviceID();
      static uint8_t getRate();
      static void setRate(uint8_t rate);
      static float getTemperature();
      static void getMotion6();
      static float getAngleX();
      static float getAngleY();
      static float getAngleZ();
};
#endif
