#pragma once
#define _USE_32BIT_TIME_T

// header files provided by Compiler
#pragma warning(disable:4514) 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <errno.h>
#include <math.h>
#include <map>

#include <iostream>
#include <thread>

#ifndef _WIN32
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#else
#include <windows.h>
#include <direct.h>
#include <conio.h>
#endif
#pragma warning(default:4514) 
#include <mutex>
#include <queue>

#include "defines.h"
#include "logging.h"
#include "warning.h"
#include "common.h"
#include "../huehelper/include/HueMacAddressMap.h"
#include "../huehelper/include/HueQueue.h"
#include "../huehelper/include/HueRQueue.h"


// action and action-time classes cross refer. so give advance declarations.
class AbstractDevice;
class CActionItem;
class CAction;
class CX10ControllableDevice;
class CFlag;
class CTimer;
class CTimeout;


typedef struct
{
    AbstractDevice** device;
    CFlag** flag;
    CTimer** timer;
    CAction** action;
                CTimeout** timeout;
} loadMap_t;

#ifdef MAIN_PROGRAM
bool simulatingPast;
loadMap_t loadMap =
{
    nullptr, nullptr, nullptr, nullptr, nullptr
};
#else
extern loadMap_t loadMap;
extern bool simulatingPast;
#endif

#ifdef MAIN_PROGRAM
codeHeaderRecord_t codeHeaderRecord;
HueMacAddressMap macAddressDeviceNoMap;
HueQueue hueMessageQueue;
HueRQueue reverseHueMessageQueue;
#else
extern codeHeaderRecord_t codeHeaderRecord;
extern HueMacAddressMap macAddressDeviceNoMap;
extern HueQueue hueMessageQueue;
extern HueRQueue reverseHueMessageQueue;
#endif

typedef enum
{
    msg_none, msg_start, msg_data, msg_pause, msg_error, msg_suppress
} msg_state_t;

// lowermost 9 bits used to contain message
typedef int x10_message_t;
#define X10_MESSAGE_LENGTH 9

typedef long long nanoseconds_t;


#include "vm.h"
#include "clock.h"
#include "calendar.h"
#include "device.h"
#include "map.h"
#include "queue.h"
#include "tw523in.h"
#include "tw523out.h"
#include "tw523.h"
#ifdef _WIN32
#include "../Win/include/gpioDevice.h"
#include "../Win/include/gpioDevices.h"
#include "../Win/include/gpioReceive.h"
#include "../Win/include/gpioTransmit.h"
#include "../Win/include/gpio.h"
#endif
#include "flag.h"
#include "action.h"
#include "timer.h"
#include "timeout.h"
#include "message.h"
#include "scheduler.h"
#include "housecode.h"
#include "house.h"
#include "port.h"
#include "rqueue.h"
#include "readcodefile.h"

extern bool interactive;


