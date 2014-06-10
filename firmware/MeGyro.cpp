#include "MeGyro.h"
static uint8_t buffers[14];
static float angleX=0;
static float angleY=0;
static float angleZ=0;
void MeGyro::init(){ 
    Fastwire::setup(400,true);
    setClockSource();
    setFullScaleGyroRange();
    setFullScaleAccelRange();
    setSleepEnabled();
}
void MeGyro::setClockSource(){ 
    I2Cdev::writeBits(0x68, 0x6B, 2, 3, 0x01);
}
void MeGyro::setFullScaleGyroRange(){
    I2Cdev::writeBits(0x68, 0x1B, 4, 2, 0x00);
}
void MeGyro::setFullScaleAccelRange(){
    I2Cdev::writeBits(0x68, 0x1C, 4, 2, 0x00);
}
void MeGyro::setSleepEnabled(){
    I2Cdev::writeBit(0x68, 0x6B, 6, false);
  
}
uint8_t MeGyro::getDeviceID() {
    I2Cdev::readBits(0x68, 0x75, 6, 6, buffers);
    return buffers[0]==0x34;
}
uint8_t MeGyro::getRate() {
    I2Cdev::readByte(0x68, 0x19, buffers);
    return buffers[0];
}
void MeGyro::setRate(uint8_t rate) {
    I2Cdev::writeByte(0x68, 0x19, rate);
}
float MeGyro::getTemperature(){
  I2Cdev::readBytes(0x68, 0x41, 2, buffers);
  int16_t t = (((int16_t)buffers[0]) << 8) | buffers[1];
  return (12.0+((double) (t + 13200.0)) / 280.0)/1.8;
}
void MeGyro::getMotion6() {
    if(getDeviceID()==0){
        init();
    }
    I2Cdev::readBytes(0x68, 0x3B,14,buffers);
    double ax = ((((int16_t)buffers[0]) << 8) | buffers[1])/100.0;
    double ay = ((((int16_t)buffers[2]) << 8) | buffers[3])/100.0;
    double az = ((((int16_t)buffers[4]) << 8) | buffers[5])/100.0;
    double gx = (((((int16_t)buffers[8]) << 8) | buffers[9])+281.00)/65.5;
    double gy = (((((int16_t)buffers[10]) << 8) | buffers[11])-18.00)/65.5;
    double gz = (((((int16_t)buffers[12]) << 8) | buffers[13])+83.00)/65.5;  
    angleX = (atan(ax/az))*180/3.1415926;
    angleY = (atan(ay/az))*180/3.1415926;
    angleZ = gz/10.0;
}
float MeGyro::getAngleX(){
  return angleX;
}
float MeGyro::getAngleY(){
  return angleY;
}
float MeGyro::getAngleZ(){
  return angleZ;
}
