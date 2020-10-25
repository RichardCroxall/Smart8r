smart : action.o calendar.o clock.o device.o flag.o housecode.o house.o huedevice.o logging.o map.o message.o queue.o runtime.o readcodefile.o rqueue.o scheduler.o smart8r.o timer.o timeout.o tw523.o tw523in.o tw523out.o port.o vm.o
smart: HueConnection.o HueMacAddressMap.o HuePersistance.o HueQueue.o HueRQueue.o HueThread.o
smart: APICache.o BaseHttpHandler.o BridgeConfig.o ColorUnits.o ExtendedColorHueStrategy.o ExtendedColorTemperatureStrategy.o Group.o Hue.o
smart: HueCommandAPI.o HueDeviceTypes.o HueException.o HueLight.o LinHttpHandler.o ModelPictures.o Scene.o Schedule.o SimpleBrightnessStrategy.o
smart: SimpleColorHueStrategy.o SimpleColorTemperatureStrategy.o StateTransaction.o TimePattern.o UPnP.o Utils.o
	g++ action.o calendar.o clock.o device.o flag.o housecode.o house.o huedevice.o logging.o map.o message.o queue.o runtime.o readcodefile.o rqueue.o scheduler.o smart8r.o timer.o timeout.o tw523.o tw523in.o tw523out.o port.o vm.o \
		HueConnection.o HueMacAddressMap.o HuePersistance.o HueQueue.o HueRQueue.o HueThread.o \
		APICache.o BaseHttpHandler.o BridgeConfig.o ColorUnits.o ExtendedColorHueStrategy.o ExtendedColorTemperatureStrategy.o Group.o Hue.o \
		HueCommandAPI.o HueDeviceTypes.o HueException.o HueLight.o LinHttpHandler.o ModelPictures.o Scene.o Schedule.o SimpleBrightnessStrategy.o \
		SimpleColorHueStrategy.o SimpleColorTemperatureStrategy.o StateTransaction.o TimePattern.o UPnP.o Utils.o -pthread -lpthread -lrt -o smart

include/runtime.h : include/action.h include/calendar.h include/clock.h include/common.h include/defines.h include/device.h include/flag.h include/house.h include/housecode.h include/logging.h 
include/runtime.h: include/map.h include/message.h include/queue.h include/rqueue.h include/scheduler.h include/timer.h include/timeout.h include/tw523.h include/tw523in.h
include/runtime.h: include/tw523out.h include/port.h include/vm.h
	touch include/runtime.h

action.o : action.cpp include/runtime.h
	g++ -c action.cpp -pthread

calendar.o : calendar.cpp include/runtime.h
	g++ -c calendar.cpp -pthread

clock.o : clock.cpp include/runtime.h
	g++ -c clock.cpp -pthread

device.o : device.cpp include/runtime.h
	g++ -c device.cpp -pthread

flag.o : flag.cpp include/runtime.h
	g++ -c flag.cpp -pthread

housecode.o : housecode.cpp include/runtime.h
	g++ -c housecode.cpp -pthread

house.o : house.cpp include/runtime.h
	g++ -c house.cpp -pthread

huedevice.o : huedevice.cpp include/runtime.h
	g++ -c huedevice.cpp -pthread

logging.o : logging.cpp include/runtime.h
	g++ -c logging.cpp -pthread

map.o : map.cpp include/runtime.h
	g++ -c map.cpp -pthread

message.o : message.cpp include/runtime.h
	g++ -c message.cpp -pthread

port.o : port.cpp include/runtime.h
	g++ -c port.cpp -pthread

queue.o : queue.cpp include/runtime.h
	g++ -c queue.cpp -pthread

readcodefile.o : readcodefile.cpp include/runtime.h
	g++ -c readcodefile.cpp -pthread

runtime.o : runtime.cpp include/runtime.h
	g++ -c runtime.cpp -pthread

rqueue.o : rqueue.cpp include/runtime.h
	g++ -c rqueue.cpp -pthread

scheduler.o : scheduler.cpp include/runtime.h
	g++ -c scheduler.cpp -pthread

smart8r.o : smart8r.cpp include/runtime.h
	g++ -c smart8r.cpp -pthread

timer.o : timer.cpp include/runtime.h
	g++ -c timer.cpp -pthread

timeout.o : timeout.cpp include/runtime.h
	g++ -c timeout.cpp -pthread

tw523.o : tw523.cpp include/runtime.h
	g++ -c tw523.cpp -pthread

tw523in.o : tw523in.cpp include/runtime.h
	g++ -c tw523in.cpp -pthread

tw523out.o : tw523out.cpp include/runtime.h
	g++ -c tw523out.cpp -pthread

vm.o : vm.cpp include/runtime.h
	g++ -c vm.cpp -pthread


huehelper/include/HueCommon.h : huehelper/include/HueConnection.h huehelper/include/HueMacAddressMap.h huehelper/include/HuePersistance.h huehelper/include/HueQueue.h huehelper/include/HueRQueue.h huehelper/include/HueThread.h
huehelper/include/HueCommon.h : hueplusplus/include/APICache.h hueplusplus/include/BaseHttpHandler.h hueplusplus/include/BridgeConfig.h hueplusplus/include/BrightnessStrategy.h hueplusplus/include/ColorHueStrategy.h 
huehelper/include/HueCommon.h : hueplusplus/include/ColorTemperatureStrategy.h hueplusplus/include/ColorUnits.h 
huehelper/include/HueCommon.h : hueplusplus/include/ExtendedColorHueStrategy.h hueplusplus/include/ExtendedColorTemperatureStrategy.h hueplusplus/include/Group.h hueplusplus/include/Hue.h hueplusplus/include/HueCommandAPI.h
huehelper/include/HueCommon.h : hueplusplus/include/HueConfig.h hueplusplus/include/HueDeviceTypes.h hueplusplus/include/HueException.h hueplusplus/include/HueExceptionMacro.h hueplusplus/include/HueLight.h 
huehelper/include/HueCommon.h : hueplusplus/include/IHttpHandler.h
huehelper/include/HueCommon.h : hueplusplus/include/LinHttpHandler.h hueplusplus/include/ModelPictures.h hueplusplus/include/Scene.h hueplusplus/include/Schedule.h hueplusplus/include/SimpleBrightnessStrategy.h 
huehelper/include/HueCommon.h : hueplusplus/include/SimpleColorHueStrategy.h hueplusplus/include/SimpleColorTemperatureStrategy.h hueplusplus/include/StateTransaction.h
huehelper/include/HueCommon.h : hueplusplus/include/TimePattern.h hueplusplus/include/Units.h hueplusplus/include/UPnP.h hueplusplus/include/Utils.h hueplusplus/include/WinHttpHandler.h
	touch huehelper/include/HueCommon.h


HueConnection.o: huehelper/HueConnection.cpp huehelper/include/HueCommon.h
	g++ -c -Wno-psabi huehelper/HueConnection.cpp -pthread

HueMacAddressMap.o: huehelper/HueMacAddressMap.cpp huehelper/include/HueCommon.h
	g++ -c -Wno-psabi huehelper/HueMacAddressMap.cpp -pthread

HuePersistance.o: huehelper/HuePersistance.cpp huehelper/include/HueCommon.h
	g++ -c -Wno-psabi huehelper/HuePersistance.cpp -pthread

HueQueue.o: huehelper/HueQueue.cpp huehelper/include/HueCommon.h
	g++ -c -Wno-psabi huehelper/HueQueue.cpp -pthread

HueRQueue.o: huehelper/HueRQueue.cpp huehelper/include/HueCommon.h
	g++ -c -Wno-psabi huehelper/HueRQueue.cpp -pthread

HueThread.o: huehelper/HueThread.cpp huehelper/include/HueCommon.h
	g++ -c -Wno-psabi huehelper/HueThread.cpp -pthread


APICache.o: hueplusplus/src/APICache.cpp huehelper/include/HueCommon.h
	g++ -c -Wno-psabi hueplusplus/src/APICache.cpp -pthread

BaseHttpHandler.o: hueplusplus/src/BaseHttpHandler.cpp huehelper/include/HueCommon.h
	g++ -c -Wno-psabi hueplusplus/src/BaseHttpHandler.cpp -pthread

BridgeConfig.o: hueplusplus/src/BridgeConfig.cpp huehelper/include/HueCommon.h
	g++ -c -Wno-psabi hueplusplus/src/BridgeConfig.cpp -pthread

ColorUnits.o: hueplusplus/src/ColorUnits.cpp huehelper/include/HueCommon.h
	g++ -c -Wno-psabi hueplusplus/src/ColorUnits.cpp -pthread

ExtendedColorHueStrategy.o: hueplusplus/src/ExtendedColorHueStrategy.cpp huehelper/include/HueCommon.h
	g++ -c -Wno-psabi hueplusplus/src/ExtendedColorHueStrategy.cpp -pthread

ExtendedColorTemperatureStrategy.o: hueplusplus/src/ExtendedColorTemperatureStrategy.cpp huehelper/include/HueCommon.h
	g++ -c -Wno-psabi hueplusplus/src/ExtendedColorTemperatureStrategy.cpp -pthread

Group.o: hueplusplus/src/Group.cpp huehelper/include/HueCommon.h
	g++ -c -Wno-psabi hueplusplus/src/Group.cpp -pthread

Hue.o: hueplusplus/src/Hue.cpp huehelper/include/HueCommon.h
	g++ -c -Wno-psabi hueplusplus/src/Hue.cpp -pthread

HueCommandAPI.o: hueplusplus/src/HueCommandAPI.cpp huehelper/include/HueCommon.h
	g++ -c -Wno-psabi hueplusplus/src/HueCommandAPI.cpp -pthread

HueDeviceTypes.o: hueplusplus/src/HueDeviceTypes.cpp huehelper/include/HueCommon.h
	g++ -c -Wno-psabi hueplusplus/src/HueDeviceTypes.cpp -pthread

HueException.o: hueplusplus/src/HueException.cpp huehelper/include/HueCommon.h
	g++ -c -Wno-psabi hueplusplus/src/HueException.cpp -pthread

HueLight.o: hueplusplus/src/HueLight.cpp huehelper/include/HueCommon.h
	g++ -c -Wno-psabi hueplusplus/src/HueLight.cpp -pthread

LinHttpHandler.o: hueplusplus/src/LinHttpHandler.cpp huehelper/include/HueCommon.h
	g++ -c -Wno-psabi hueplusplus/src/LinHttpHandler.cpp -pthread

ModelPictures.o: hueplusplus/src/ModelPictures.cpp huehelper/include/HueCommon.h
	g++ -c -Wno-psabi hueplusplus/src/ModelPictures.cpp -pthread

Scene.o: hueplusplus/src/Scene.cpp huehelper/include/HueCommon.h
	g++ -c -Wno-psabi hueplusplus/src/Scene.cpp -pthread

Schedule.o: hueplusplus/src/Schedule.cpp huehelper/include/HueCommon.h
	g++ -c -Wno-psabi hueplusplus/src/Schedule.cpp -pthread

SimpleBrightnessStrategy.o: hueplusplus/src/SimpleBrightnessStrategy.cpp huehelper/include/HueCommon.h
	g++ -c -Wno-psabi hueplusplus/src/SimpleBrightnessStrategy.cpp -pthread

SimpleColorHueStrategy.o: hueplusplus/src/SimpleColorHueStrategy.cpp huehelper/include/HueCommon.h
	g++ -c -Wno-psabi hueplusplus/src/SimpleColorHueStrategy.cpp -pthread

SimpleColorTemperatureStrategy.o: hueplusplus/src/SimpleColorTemperatureStrategy.cpp huehelper/include/HueCommon.h
	g++ -c -Wno-psabi hueplusplus/src/SimpleColorTemperatureStrategy.cpp -pthread

StateTransaction.o: hueplusplus/src/StateTransaction.cpp huehelper/include/HueCommon.h
	g++ -c -Wno-psabi hueplusplus/src/StateTransaction.cpp -pthread

TimePattern.o: hueplusplus/src/TimePattern.cpp huehelper/include/HueCommon.h
	g++ -c -Wno-psabi hueplusplus/src/TimePattern.cpp -pthread

UPnP.o: hueplusplus/src/UPnP.cpp huehelper/include/HueCommon.h
	g++ -c -Wno-psabi hueplusplus/src/UPnP.cpp -pthread

Utils.o: hueplusplus/src/Utils.cpp huehelper/include/HueCommon.h
	g++ -c -Wno-psabi hueplusplus/src/Utils.cpp -pthread



