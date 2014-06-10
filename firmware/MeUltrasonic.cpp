#include "MeUltrasonic.h"
/*           UltrasonicSenser                 */
MeUltrasonic::MeUltrasonic(): MePort(0)
{
}
MeUltrasonic::MeUltrasonic(uint8_t port): MePort(port)
{
}
long MeUltrasonic::distanceCm()
{
    long distance = measure();
    return ((distance / 29) >> 1);
}
long MeUltrasonic::measure()
{
    long duration;
    MePort::Dwrite2(LOW);
    delayMicroseconds(2);
    MePort::Dwrite2(HIGH);	
    delayMicroseconds(10);
    MePort::Dwrite2(LOW);
    pinMode(s2, INPUT);
    duration = pulseIn(s2, HIGH); 
    return duration;
}
