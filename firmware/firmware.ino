/*************************************************************************
* File Name          : Firmware.ino
* Author             : Ander
* Updated            : Ander
* Version            : V1.1.0
* Date               : 03/06/2014
* Description        : Firmware for Makeblock Electronic modules with Scratch.  
* License            : CC-BY-SA 3.0
* Copyright (C) 2013 - 2014 Maker Works Technology Co., Ltd. All right reserved.
* http://www.makeblock.cc/
**************************************************************************/
#include <Servo.h>
#include "MePort.h"
#include "MeServo.h" 
#include "MeDCMotor.h" 
#include "MeUltrasonic.h" 
#include "MeGyro.h"
#include "Me7SegmentDisplay.h"
#include "MeTemperature.h"
#include "MeRGBLed.h"
#include "MeInfraredReceiver.h"
MeServo servo;  
MeDCMotor dc;
MeTemperature ts;
MeRGBLed led;
MeUltrasonic us;
Me7SegmentDisplay seg;
MePort generalDevice;
MeInfraredReceiver ir;
typedef struct MeModule
{
    int device;
    int port;
    int slot;
    int pin;
    int index;
    float values[3];
} MeModule;

union{
    byte byteVal[4];
    float floatVal;
    long longVal;
}val;

MeModule modules[12];
float mVersion = 1.0611;
boolean isAvailable = false;
boolean isBluetooth = false;
void setup(){
  
#if defined(__AVR_ATmega32U4__) 
  Serial1.begin(9600);
  delay(2000);
  Serial1.println("AT+BAUD8");
  delay(1000);
  Serial1.begin(115200); 
#else
  Serial.begin(9600);
  delay(2000);
  Serial.println("AT+BAUD8");
  delay(1000);
#endif
  Serial.begin(115200);

  buzzerOn();
  delay(100);
  buzzerOff();
  
}
int len = 52;
char buffer[52];
char bufferBt[52];
byte index = 0;
byte dataLen;
byte modulesLen=0;
boolean isStart = false;
unsigned char irRead;
char serialRead;
#define VERSION 0
#define ULTRASONIC_SENSOR 1
#define TEMPERATURE_SENSOR 2
#define LIGHT_SENSOR 3
#define POTENTIONMETER 4
#define JOYSTICK 5
#define GYRO 6
#define RGBLED 8
#define SEVSEG 9
#define MOTOR 10
#define SERVO 11
#define ENCODER 12
#define INFRARED 16
#define LINEFOLLOWER 17
#define DIGITAL_INPUT 30
#define ANALOG_INPUT 31
#define DIGITAL_OUTPUT 32
#define ANALOG_OUTPUT 33
#define PWM_OUTPUT 34

float angleServo = 90.0;

unsigned char prevc=0;

void loop(){
  if(ir.buttonState()==1){ 
    if(ir.available()>0){
      irRead = ir.read();
    }
  }else{
    irRead = 0;
  }
  readSerial();
  if(isAvailable){
    unsigned char c = serialRead&0xff;
    if(c==0x55&&isStart==false){
     if(prevc==0xff){
      index=1; 
      isStart = true;
     }
    }else{
      prevc = c;
      if(isStart){
        if(index==3){
         dataLen = c; 
        }else if(index>3){
          dataLen--;
        }
         writeBuffer(index,c);
      }
    }
     index++;
     if(index>51){
      index=0; 
      isStart=false;
     }
     if(isStart&&dataLen==0&&index>3){ 
        isStart = false;
       parseData(); 
       index=0;
     }
  }
}
unsigned char readBuffer(int index){
 return isBluetooth?bufferBt[index]:buffer[index]; 
}
void writeBuffer(int index,unsigned char c){
 if(isBluetooth){
  bufferBt[index]=c;
 }else{
  buffer[index]=c;
 } 
}
void writeEnd(){
  
#if defined(__AVR_ATmega32U4__) 
 isBluetooth?Serial1.println():Serial.println(); 
#else
 Serial.println(); 
#endif
}
void writeSerial(unsigned char c){
  #if defined(__AVR_ATmega32U4__) 
 isBluetooth?Serial1.write(c):Serial.write(c); 
 #else
 Serial.write(c);
 #endif
}
void readSerial(){
  isAvailable = false;
  if(Serial.available()>0){
    isAvailable = true;
    isBluetooth = false;
    serialRead = Serial.read();
  }
    #if defined(__AVR_ATmega32U4__) 
  else if(Serial1.available()>0){
    isAvailable = true;
    isBluetooth = true;
    serialRead = Serial1.read();
  }
  #endif
}
void parseData(){
  isStart = false;
  switch(readBuffer(2)){
    case 0x1:{
      if(readBuffer(3)>0){
        int l = readBuffer(3)/2;
        for(int i=0;i<l;i++){
          int device = readBuffer(i*2+4);
          int port = (readBuffer(i*2+5)&0xf0)>>4;
          int slot = readBuffer(i*2+5)&0xf;
          modules[i].device = device;
          modules[i].port = port;
          modules[i].slot = slot;
          modules[i].pin = readBuffer(i*2+5);
        }
        modulesLen = l;
      }else{
        modulesLen = 0;
      }
        readModules();
     }
     break;
     case 0x2:{
        int l = readBuffer(3);
        int dataIndex = 4;
        while(l>dataIndex-4){
          int device = readBuffer(dataIndex);
          dataIndex++;
          int pin = readBuffer(dataIndex);
          int port = (pin&0xf0)>>4;
          int slot = pin&0xf;
          dataIndex++;
          MeModule module;
          module.device = device;
          module.port = port;
          module.slot = slot;
          module.pin = pin;
          if(device==RGBLED){
            module.index = readBuffer(dataIndex++);
            module.values[0]=readBuffer(dataIndex++);
            module.values[1]=readBuffer(dataIndex++);
            module.values[2]=readBuffer(dataIndex++);
            if(led.getPort()!=port){
              led.reset(port);
            }
            if(module.index>0){
              led.setColorAt(module.index-1,module.values[0],module.values[1],module.values[2]);
            }else{
              for(int t=0;t<led.getNumber();t++){
                led.setColorAt(t,module.values[0],module.values[1],module.values[2]);
              }
            }
            led.show();
            callOK();
          }else if(device==MOTOR){
            val.byteVal[0]=readBuffer(dataIndex++);
            val.byteVal[1]=readBuffer(dataIndex++);
            val.byteVal[2]=readBuffer(dataIndex++);
            val.byteVal[3]=readBuffer(dataIndex++);
            module.values[0]=val.floatVal;
            dc.reset(module.port);
            dc.run(module.values[0]);
            callOK();
          }else if(device==SEVSEG){
            val.byteVal[0]=readBuffer(dataIndex++);
            val.byteVal[1]=readBuffer(dataIndex++);
            val.byteVal[2]=readBuffer(dataIndex++);
            val.byteVal[3]=readBuffer(dataIndex++);
            module.values[0]=val.floatVal;
            if(seg.getPort()!=port){
               seg.reset(port);
            }
            seg.display(module.values[0]);
            callOK();
          }else if(device==SERVO){
            
            val.byteVal[0]=readBuffer(dataIndex++);
            val.byteVal[1]=readBuffer(dataIndex++);
            val.byteVal[2]=readBuffer(dataIndex++);
            val.byteVal[3]=readBuffer(dataIndex++);
            module.values[0]=val.floatVal;
            angleServo=module.values[0];
            if(servo.getPort()!=port||servo.getSlot()!=slot){
              servo.reset(module.port,module.slot);
            }
            servo.write(abs(angleServo));
            callOK();
          }else if(device==LIGHT_SENSOR){
            val.byteVal[0]=readBuffer(dataIndex++);
            val.byteVal[1]=readBuffer(dataIndex++);
            val.byteVal[2]=readBuffer(dataIndex++);
            val.byteVal[3]=readBuffer(dataIndex++);
            module.values[0]=val.floatVal;
            if(generalDevice.getPort()!=port){
              generalDevice.reset(module.port);
            }
            generalDevice.Dwrite1(val.floatVal>=1?HIGH:LOW);
            callOK();
          }else if(device==DIGITAL_OUTPUT){
              val.byteVal[0]=readBuffer(dataIndex++);
              val.byteVal[1]=readBuffer(dataIndex++);
              val.byteVal[2]=readBuffer(dataIndex++);
              val.byteVal[3]=readBuffer(dataIndex++);
              pinMode(module.pin,OUTPUT);
              analogWrite(module.pin,val.floatVal==1?1023:val.floatVal);         
          }else if(device==ANALOG_OUTPUT||device==PWM_OUTPUT){
              val.byteVal[0]=readBuffer(dataIndex++);
              val.byteVal[1]=readBuffer(dataIndex++);
              val.byteVal[2]=readBuffer(dataIndex++);
              val.byteVal[3]=readBuffer(dataIndex++);
              pinMode(module.pin,OUTPUT);
              analogWrite(module.pin,val.floatVal);
              callOK();
          }
        }
      }
      break;
      case 0x4:{
        //reset
        dc.reset(M1);
        dc.run(0);
        dc.reset(M2);
        dc.run(0);
        dc.reset(PORT_1);
        dc.run(0);
        dc.reset(PORT_2);
        dc.run(0);
        callOK();
      }
     break;
  }
}
void callOK(){
    writeSerial(0xff);
    writeSerial(0x55);
    writeEnd();
}
void readModules(){
    
    writeSerial(0xff);
    writeSerial(0x55);
    writeSerial(0x1);
    if(modulesLen>0){
      for(int i=0;i<modulesLen;i++){
        MeModule module = modules[i];
        readSensor(module.device,module.port,module.slot,module.pin);
      }
    }
    writeEnd();
}
void sendValue(float value){ 
     val.floatVal = value;
     writeSerial(val.byteVal[0]);
     writeSerial(val.byteVal[1]);
     writeSerial(val.byteVal[2]);
     writeSerial(val.byteVal[3]);
}
void readSensor(int device,int port,int slot,int pin){
  float value=0.0;
  switch(device){
   case  ULTRASONIC_SENSOR:{
     if(us.getPort()!=port){
       us.reset(port);
     }
     value = us.distanceCm();
     sendValue(value);
   }
   break;
   case  TEMPERATURE_SENSOR:{
     if(ts.getPort()!=port||ts.getSlot()!=slot){
       ts.reset(port,slot);
     }
     value = ts.temperature();
     sendValue(value);
   }
   break;
   case  LIGHT_SENSOR:{
     if(generalDevice.getPort()!=port){
       generalDevice.reset(port);
     }
     value = generalDevice.Aread2();
     sendValue(value);
   }
   break;
   case  POTENTIONMETER:{
     if(generalDevice.getPort()!=port){
       generalDevice.reset(port);
     }
     value = generalDevice.Aread2();
     sendValue(value);
   }
   break;
   case  JOYSTICK:{
     if(generalDevice.getPort()!=port){
       generalDevice.reset(port);
     }
     value = generalDevice.Aread1();
     sendValue(value);
     value = generalDevice.Aread2();
     sendValue(value);
   }
   break;
   case  INFRARED:{
     if(ir.getPort()!=port){
       ir.reset(port);
     }
     sendValue(irRead);
   }
   break;
   case  LINEFOLLOWER:{
     if(generalDevice.getPort()!=port){
       generalDevice.reset(port);
     }
     value = generalDevice.Dread1()*2+generalDevice.Dread2();
     sendValue(value);
   }
   break;
   case  GYRO:{
       MeGyro::getMotion6();
       value = MeGyro::getAngleX();
       sendValue(value);
       value = MeGyro::getAngleY();
       sendValue(value);
       value = MeGyro::getAngleZ();
       sendValue(value);
   }
   break;
   case  VERSION:{
     sendValue(mVersion);
   }
   break;
   case  DIGITAL_INPUT:{
     pinMode(pin,INPUT);
     sendValue(digitalRead(pin));
   }
   break;
   case  ANALOG_INPUT:{
     pinMode(pin,INPUT);
     sendValue(analogRead(pin));
   }
   break;
  }
}
