#ifndef MESERVO_H_
#define MESERVO_H_
#include <Arduino.h>
#include "MePort.h"
class MeServo : public MePort{
 public:
  MeServo();
  MeServo(uint8_t port);
  MeServo(uint8_t port,uint8_t slot);
  void reset(uint8_t port,uint8_t slot);
  void attach(uint8_t pin);
  void detach();
  boolean attached();
  void write(uint8_t a);
  void refresh(void);
 private:
  boolean isAttached;
  uint8_t servoPin, angle;
  uint16_t delayTime;
  unsigned long mTime;
  int pinState;
};
#endif
