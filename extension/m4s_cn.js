// m4s.js
// yzj June 1st 2014
// Makeblock for Scratch Extension
//

(function(ext) {
    var device = null
    var rxbuff = null

    var slot1 = 1
    var slot2 = 2

    var axisX = 0
    var axisY = 1
    var axisZ = 2

    var port1 = 0x10
    var port2 = 0x20
    var port3 = 0x30
    var port4 = 0x40
    var port5 = 0x50
    var port6 = 0x60
    var port7 = 0x70
    var port8 = 0x80
    var m1 = 0x90
    var m2 = 0xA0
    var I2C =  0xB0
    var DIGIPORT = 0xC0
    var ALOGPORT = 0xD0

    var portEnum = {"接口1":port1,"接口2":port2,"接口3":port3,"接口4":port4,"接口5":port5,"接口6":port6,"接口7":port7,"接口8":port8,"电机接口1":m1,"电机接口2":m2,"I2C":I2C}
    var slotEnum = {"插口1":slot1,"插口2":slot2}
    var axisEnum = {"X轴":axisX,"Y轴":axisY,"Z轴":axisZ}
    var dpinEnum = {"D2":0,"D3":1,"D4":2,"D5":3,"D6":4,"D7":5,"D8":6,"D9":7,"D10":8,"D11":9,"D12":10,"D13":11}
    var apinEnum = {"A0":0,"A1":1,"A2":2,"A3":3,"A4":4,"A5":5}
    var pinmodeEnum = {"输入":1,"输出":0}
    var levelEnum = {"低电平":0,"高电平":1,"关":0,"开":1}
    
    var firmVersion = 0;

    var VERSION = 0
    var ULTRASONIC_SENSOR = 1
    var TEMPERATURE_SENSOR = 2
    var LIGHT_SENSOR = 3
    var POTENTIONMETER = 4
    var JOYSTICK = 5
    var GYRO = 6
    var RGBLED = 8
    var SEVSEG = 9
    var MOTOR = 10
    var SERVO = 11
    var ENCODER = 12
    var INFRARED = 16
    var LINEFOLLOWER = 17
    
    var DIGITAL_INPUT = 30
    var ANALOG_INPUT = 31
    var DIGITAL_OUTPUT = 32
    var ANALOG_OUTPUT = 33
    var PWM_OUTPUT = 34

	moduleList = [] //{port:port2,slot:slot1,module:module}

    function appendModule(module,portstr,slotstr,pin){
        var port = portEnum[portstr]
        var slot = slotEnum[slotstr]
        var value = 0;
        for(var i=0;i<moduleList.length;i++){
            mod = moduleList[i];
            if(mod.port == port && mod.slot == slot){
                // module at this port & slot changed
                if(module != mod.module){
                    mod.module = module
                    mod.value = [] // reset the value to 0
                }
                return i
            }
        }
        moduleList.push(constructModule(module,portstr,slotstr,pin,0));
        return moduleList.length-1
    }

    function constructModule(module,portstr,slotstr,pin,value){
        var port = portEnum[portstr]
        var slot = slotEnum[slotstr]
        return {port:port,slot:slot,module:module,pin:pin,value:[value]}
    }

    function sendModuleList(){
        if(!device) return;
        len = moduleList.length
        // ff 55 1 numdev dev1 port|slot 
        var buff = new Uint8Array(4+len*2)
        buff[0]=0xff
        buff[1]=0x55
        buff[2]=0x01
        buff[3]=len*2
        for(var i=0;i<moduleList.length;i++){
            mod = moduleList[i]
            buff[4+i*2] = mod.module
            buff[4+i*2+1] = mod.module>=DIGITAL_INPUT?mod.pin:(mod.port+mod.slot)
        }
        console.log("sendModuleList:",buff)
        device.send(buff.buffer);
    }

    function b2f(s,pos_start)
    {
        var d =new Uint8Array(s.subarray(pos_start,pos_start+4))
        var floatarray = new Float32Array(d.buffer, 0);
        return floatarray[0]
    }

    function appendBuffer( buffer1, buffer2 ) {
        var tmp = new Uint8Array( buffer1.byteLength + buffer2.byteLength );
        tmp.set( new Uint8Array( buffer1 ), 0 );
        tmp.set( new Uint8Array( buffer2 ), buffer1.byteLength );
        return tmp;
    }

    function parsePackage(s){
        console.log("rx",s);
        if(s[0]==0xff && s[1]==0x55){
            // ff 55 1 version[4] dev0[4] .... \r \n
            if(s[2]==0x01){
                var dataLen = (s.length-3-4-2)/4;
                firmVersion = b2f(s,3).toFixed(5)
                console.log("get cmd 1:",firmVersion,dataLen);
                var moduleIndex = 0
                for(var i=0;i<dataLen;i++){
                    // some special module may take multiple reply
                    if(moduleList[moduleIndex].module == JOYSTICK){
                        value = b2f(s,3+4+i*4)
                        i+=1
                        value2 = b2f(s,3+4+i*4)
                        moduleList[moduleIndex].value = [value,value2]
                    }else if(moduleList[moduleIndex].module == GYRO){
                        value = b2f(s,3+4+i*4)
                        i+=1
                        value2 = b2f(s,3+4+i*4)
                        i+=1
                        value3 = b2f(s,3+4+i*4)
                        moduleList[moduleIndex].value = [value,value2,value3]
                    }else{
                        value = b2f(s,3+4+i*4)
                        moduleList[moduleIndex].value = [value]
                    }
                    moduleIndex+=1;
                }
            }
        }
    }

    function deviceRun(mod){
        if(!device) return;
        // ff 55 2 dev port|slot value[4]
        var cc = new Uint8Array(10);
        cc[0]=0xff;
        cc[1]=0x55;
        cc[2]=0x02;
        cc[3]=0x06; // the len of one device description
        cc[4]=mod.module
        cc[5]=mod.module>=DIGITAL_INPUT?mod.pin:(mod.port+mod.slot)
        if(mod.value.length==1){
            var floatarray = new Float32Array(1)
            floatarray[0] = mod.value[0]
            var s = new Uint8Array(floatarray.buffer)
            cc.set(s,6)
        }else if(mod.value.length == 4){
            cc.set(mod.value,6)
        }
        console.log("run>",cc)
        device.send(cc.buffer);
    }


    ext.doMotorRun = function(port,speed) {
        mod=constructModule(MOTOR,port,"插口1",0,speed)
        deviceRun(mod)
    };

    ext.doServoRun = function(port,slot,speed){
        mod=constructModule(SERVO,port,slot,0,speed)
        deviceRun(mod)
    };

    ext.doUltrasonic = function(port){
        index = appendModule(ULTRASONIC_SENSOR,port,"插口1",0)
        sendModuleList()
        return moduleList[index].value[0]
    };

    ext.doLinefollow = function(port){
        index = appendModule(LINEFOLLOWER,port,"插口1",0)
        sendModuleList()
        return moduleList[index].value[0]
    };

    ext.doLimitSwitch = function(port){


    };

    ext.doTemperature = function(port,slot){
        index = appendModule(TEMPERATURE_SENSOR,port,slot,0)
        sendModuleList()
        return moduleList[index].value[0]
    };

    ext.doLightSensor = function(port){
        index = appendModule(LIGHT_SENSOR,port,"插口1",0)
        sendModuleList()
        return moduleList[index].value[0]
    };

    ext.doRunLightSensor = function(port,level){
        value = levelEnum[level]
        mod=constructModule(LIGHT_SENSOR,port,"插口1",0,value)
        deviceRun(mod)
    }

    ext.doSoundSensor = function(port){


    };

    ext.doJoystick = function(port,axis){
        index = appendModule(JOYSTICK,port,"插口1",0)
        sendModuleList()
        axis = axisEnum[axis]
        switch(axis){
            case axisX:
                return moduleList[index].value[0]
            case axisY:
                return moduleList[index].value[1]
        }
    };

    ext.doGyro = function(axis){
        index = appendModule(GYRO,"I2C","插口1",0)
        sendModuleList()
        axis = axisEnum[axis]
        switch(axis){
            case axisX:
                return moduleList[index].value[0]
            case axisY:
                return moduleList[index].value[1]
            case axisZ:
                return moduleList[index].value[2]
        }
    }

    ext.doPotentialMeter = function(port){
        index = appendModule(POTENTIONMETER,port,"插口2",0)
        sendModuleList()
        return moduleList[index].value[0]
    }

    ext.doInfrared = function(port){
        index = appendModule(INFRARED,port,"插口1",0)
        sendModuleList()
        return moduleList[index].value[0]
    }

    ext.doVersion = function(){
        return firmVersion;
    };

    ext.doButton = function(port){


    }

    ext.doRunSeg = function(port,num){
        mod=constructModule(SEVSEG,port,"插口1",0,num)
        deviceRun(mod)
    }

    ext.doRunRgb = function(port,pixal,r,g,b){
        mod=constructModule(RGBLED,port,"插口1",0,0)
        mod.value = [pixal,r,g,b]
        deviceRun(mod)
    }
    
    ext.doDWrite = function(pinstr,level){
        pin = dpinEnum[pinstr]
        value = levelEnum[level]
        mod = constructModule(DIGITAL_OUTPUT,"DIGIPORT","插口1",0,0)
        mod.pin = pin+2 // +2 be compatable to arduino code
        mod.value = [value]
        deviceRun(mod)
    }
    ext.doAWrite = function(pinstr,value){
        pin = dpinEnum[pinstr]
        mod = constructModule(ANALOG_OUTPUT,"DIGIPORT","插口1",0,value)
        mod.pin = pin+2 // +2 be compatable to arduino code
        mod.value = [value]
        deviceRun(mod)
    }

    ext.doDRead = function(pin){
        index = appendModule(DIGITAL_INPUT,"DIGIPORT","插口1",pin)
        sendModuleList()
        pinvalue = moduleList[index].value[0];
        return pinvalue
    }

    ext.doARead = function(pin){
        index = appendModule(ANALOG_INPUT,"ALOGPORT","插口1",pin);
        sendModuleList();
        pinvalue = moduleList[index].value[0];
        return pinvalue;
    }

    ext.resetAll = function(){
        console.log("resetAll")
        var cc = new Uint8Array(4);
        cc[0]=0xff;
        cc[1]=0x55;
        cc[2]=0x04;
        cc[3]=0x0; 
        device.send(cc.buffer);
    };

    ext._deviceConnected = function(dev) {
		console.log("_deviceConnected:",device);
        if(device) return;
        device = dev;
        var ser = prompt("check serial port",dev.id);
        device.id = ser
        device.open({ stopBits: 0, bitRate: 115200, ctsFlowControl: 0 });
        device.set_receive_handler(function(data) {
            if(!rxbuff){
                rxbuff = new Uint8Array(data)
            }else{
                var s = new Uint8Array(data)
                rxbuff = appendBuffer(rxbuff,s)
            }
            if(rxbuff[rxbuff.length-1]==0xA){
                parsePackage(rxbuff)
                rxbuff = null;
            }
        });
    };

    ext._shutdown = function() {
    	console.log("_shutdown");
        if(device) device.close();
        device = null;
    };

    ext._deviceRemoved = function(dev) {
    	console.log("_deviceRemoved");
        if(device != dev) return;
        device = null;
    };

    ext._getStatus = function() {
        //console.log("_getStatus:",device)
        if(!device) return {status: 1, msg: 'Makeblock disconnected'};
        return {status: 2, msg: 'Makeblock connected'};
    }


    var descriptor = {
        blocks:[
            ["r", "固件版本","doVersion"],
            ["", "设置电机%m.motorPort 转速%n", "doMotorRun", "电机接口1", 50],
            ["", "设置舵机%m.servoPort %m.slot 角度%n", "doServoRun", "接口1", "插口1", 90],
            ["", "设置数码管%m.normalPort 数字%n", "doRunSeg", "接口3", 100],
            ["", "设置LED%m.normalPort 第%n 盏 红色%n 绿色%n 蓝色%n", "doRunRgb", "接口3", 0, 0, 0, 0],
            ["r", "超声波%m.normalPort 距离", "doUltrasonic", "接口3"],
            ["r", "光线传感器%m.normalPort", "doLightSensor", "接口3"],
            ["", "设置光线传感器%m.normalPort LED状态为 %m.switch", "doRunLightSensor", "接口3", "开"],
            ["r", "巡线传感器%m.normalPort", "doLinefollow", "接口3"],
            ["r", "电位器%m.normalPort", "doPotentialMeter", "接口7"],
            ["r", "陀螺仪 %m.GyroAxis 角度", "doGyro", "X轴"],
            ["r", "红外接收器 %m.normalPort", "doInfrared", "接口6"],
            ["r", "当前温度 %m.normalPort%m.slot", "doTemperature", "接口3", "插口1"],
            ["r", "摇杆 %m.normalPort %m.Axis", "doJoystick", "接口3", "X轴"],
            ["b", "数字口 %n ", "doDRead", "13"],
            ["r", "模拟口 %n ", "doARead", "0"],
            ["", "设置 数字口 %n 输出为 %m.digital", "doDWrite", "13", "高电平"],
            ["", "设置 模拟口 %n 输出为 %n", "doAWrite", "0", 512]
        ],
        menus: {
			"normalPort":["接口3","接口4","接口5","接口6","接口7","接口8"],
			"digitalPin":["D2","D3","D4","D5","D6","D7","D8","D9","D10","D11","D12","D13"],
			"analogPin":["A0","A1","A2","A3","A4","A5"],
			"motorPort":["电机接口1","电机接口2","接口1","接口2"],
			"servoPort":["接口1","接口2","接口3","接口4","接口5","接口6","接口7","接口8"],
			"slot":["插口1","插口2"],
			"device":["Ultrasonic","Line Finder","Light Sensor","Sound Sensor","Joystick","Button"],
			"exdevice":["LimitSwitch","Temperature"],
			"mode":["输入","输出"],
			"type":["数字","模拟"],
			"Axis":["X轴","Y轴"],
			"GyroAxis":["X轴","Y轴","Z轴"],
			"digital":["低电平","高电平"],
			"switch":["关","开"]
        },
    url: 'http://www.makeblock.cc'
    };

    ScratchExtensions.register('makeblock', descriptor, ext, {type: 'serial'});
})({});