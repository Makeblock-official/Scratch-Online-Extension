#include "MeDCMotor.h"
MeDCMotor::MeDCMotor(): MePort(0)
{

}
MeDCMotor::MeDCMotor(uint8_t port): MePort(port)
{

}
void MeDCMotor::run(int speed)
{
    speed = speed > 255 ? 255 : speed;
    speed = speed < -255 ? -255 : speed;

    if(speed >= 0) {
        MePort::Dwrite2(HIGH);
        MePort::Awrite1(speed);
    } else {
        MePort::Dwrite2(LOW);
        MePort::Awrite1(-speed);
    }
}
void MeDCMotor::stop()
{
    MeDCMotor::run(0);
}