#include "MePort.h"

#if defined(__AVR_ATmega32U4__) //MeBaseBoard use ATmega32U4 as MCU

MePort_Sig mePort[11] = {{NC, NC}, {11, A8}, {13, A11}, {A10, A9}, {1, 0},
    {MISO, SCK}, {A0, A1}, {A2, A3}, {A4, A5}, {6, 7}, {5, 4}
};
#else // else ATmega328
MePort_Sig mePort[11] = {{NC, NC}, {11, 10}, {3, 9}, {12, 13}, {8, 2},
    {NC, NC}, {A2, A3}, {NC, A1}, {NC, A0}, {6, 7}, {5, 4}
};

#endif

union{
    byte b[4];
    float fVal;
    long lVal;
}u;

/*        Port       */
MePort::MePort(){
	s1 = mePort[0].s1;
    s2 = mePort[0].s2;
    _port = 0;
}
MePort::MePort(uint8_t port)
{
    s1 = mePort[port].s1;
    s2 = mePort[port].s2;
    _port = port;
	//The PWM frequency is 976 Hz
#if defined(__AVR_ATmega32U4__) //MeBaseBoard use ATmega32U4 as MCU

TCCR1A =  _BV(WGM10);
TCCR1B = _BV(CS11) | _BV(CS10) | _BV(WGM12);

TCCR3A = _BV(WGM30);
TCCR3B = _BV(CS31) | _BV(CS30) | _BV(WGM32);

TCCR4B = _BV(CS42) | _BV(CS41) | _BV(CS40);
TCCR4D = 0;

#else if defined(__AVR_ATmega328__) // else ATmega328

TCCR1A = _BV(WGM10);
TCCR1B = _BV(CS11) | _BV(CS10) | _BV(WGM12);

TCCR2A = _BV(WGM21) |_BV(WGM20);
TCCR2B = _BV(CS22);

#endif
}
MePort::MePort(uint8_t port,uint8_t slot)
{
    s1 = mePort[port].s1;
    s2 = mePort[port].s2;
    _port = port;
    _slot = slot;
	//The PWM frequency is 976 Hz
#if defined(__AVR_ATmega32U4__) //MeBaseBoard use ATmega32U4 as MCU

TCCR1A =  _BV(WGM10);
TCCR1B = _BV(CS11) | _BV(CS10) | _BV(WGM12);

TCCR3A = _BV(WGM30);
TCCR3B = _BV(CS31) | _BV(CS30) | _BV(WGM32);

TCCR4B = _BV(CS42) | _BV(CS41) | _BV(CS40);
TCCR4D = 0;

#else if defined(__AVR_ATmega328__) // else ATmega328

TCCR1A = _BV(WGM10);
TCCR1B = _BV(CS11) | _BV(CS10) | _BV(WGM12);

TCCR2A = _BV(WGM21) |_BV(WGM20);
TCCR2B = _BV(CS22);

#endif
}
uint8_t MePort::getPort(){
	return _port;
}
uint8_t MePort::getSlot(){
	return _slot;
}
bool MePort::Dread1()
{
    bool val;
    pinMode(s1, INPUT);
    val = digitalRead(s1);
    return val;
}

bool MePort::Dread2()
{
    bool val;
	pinMode(s2, INPUT);
    val = digitalRead(s2);
    return val;
}

void MePort::Dwrite1(bool value)
{
    pinMode(s1, OUTPUT);
    digitalWrite(s1, value);
}

void MePort::Dwrite2(bool value)
{
    pinMode(s2, OUTPUT);
    digitalWrite(s2, value);
}

int MePort::Aread1()
{
    int val;
    val = analogRead(s1);
    return val;
}

int MePort::Aread2()
{
    int val;
    val = analogRead(s2);
    return val;
}

void MePort::Awrite1(int value)
{   
    analogWrite(s1, value);  
}

void MePort::Awrite2(int value)
{
    analogWrite(s2, value); 
}
void MePort::reset(uint8_t port){
	s1 = mePort[port].s1;
    s2 = mePort[port].s2;
    _port = port;
}
void MePort::reset(uint8_t port,uint8_t slot){
	s1 = mePort[port].s1;
    s2 = mePort[port].s2;
    _port = port;
    _slot = slot;
}
uint8_t MePort::pin1(){
	return s1;
}
uint8_t MePort::pin2(){
	return s2;
}
