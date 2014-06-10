#include "MeServo.h"

MeServo::MeServo() {
  isAttached = false;
  servoPin = 255;
  angle = 90;
  mTime = micros();
  pinState = false;
  
}
MeServo::MeServo(uint8_t port): MePort(port)
{
  attach(s1);
}
MeServo::MeServo(uint8_t port,uint8_t slot): MePort(port,slot)
{
  attach(slot==SLOT_1?s1:s2);
}
void MeServo::reset(uint8_t port,uint8_t slot){
        detach();
        _port = port;
        _slot = slot;
	s2 = mePort[port].s2;
	s1 = mePort[port].s1;
        attach(slot==SLOT_1?s1:s2);
}
void MeServo::attach(uint8_t pin) {
  servoPin = pin;
  angle = 90;
  isAttached = true;
  pinMode(servoPin, OUTPUT);
}

void MeServo::detach(void) {
  isAttached = false;
  pinMode(servoPin, INPUT);
}

boolean  MeServo::attached(void) {
  return isAttached;
}

void MeServo::write(uint8_t a) {
  angle = a;

  if (! isAttached) return;
  delayTime = map(a, 0, 180, 1000, 2000);  
}

void MeServo::refresh(void) {
  if(isAttached){
    digitalWrite(servoPin, HIGH); 
    delayMicroseconds(delayTime);
    digitalWrite(servoPin, LOW);
  }
}
