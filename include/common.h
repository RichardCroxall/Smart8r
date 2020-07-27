#pragma once

// This file contains all constants and simple datatypes common between
// compile time and runtime of the smart house suite.
//

//the end of time
//#define THE_END_OF_TIME ((time_t) 0x7FFFFFFF)
  #define THE_END_OF_TIME ((time_t) 0x7F000000)

// calendar specifications
#define BACKTRACK_DAYS 1
#define MAX_RUNTIME 366
#define TOTAL_DAY_ATTRIBUTE_DAYS (BACKTRACK_DAYS + 1 + MAX_RUNTIME*2)
#define MAXRUNTIMEIDENTIFIERLENGTH 30
#define MAC_ADDRESS_LENGTH 26

// special times for sunrise & sunset events
#define SUNRISE_TIME (-1)
#define SUNSET_TIME (-2)

#define SECONDSPERDAY (3600L*24L)
// maximum number of seconds which can be held in a long
//day specifications, used in sets of days

#define DAY_SUN (1<<0)
#define DAY_MON (1<<1)
#define DAY_TUE (1<<2)
#define DAY_WED (1<<3)
#define DAY_THU (1<<4)
#define DAY_FRI (1<<5)
#define DAY_SAT (1<<6)
#define DAY_WORK (1<<7)
#define DAY_NONWORK (1<<8)
#define DAY_FIRSTWORK (1<<9)

// BST & GMT are mutually exclusive
#define DAY_BST (1<<10)
#define DAY_GMT (1<<11)

#ifdef MAIN_PROGRAM
const char* calendarAttributeDescriptions[] =
{
    "SUN ",
    "MON ",
    "TUE ",
    "WED ",
    "THU ",
    "FRI ",
    "SAT ",
    "WORK ",
    "NONWORK ",
    "FIRSTWORK ",
    "BST ",
    "GMT ",
    "notused 12",
    "notused 13",
    "notused 14",
    "notused 15",
};
#else
extern const char* calendarAttributeDescriptions[];
#endif
// device related codes
typedef enum
{
     UNIT1 = 0x0C,
     UNIT2 = 0x1C,
     UNIT3 = 0x04,
     UNIT4 = 0x14,
     UNIT5 = 0x02,
     UNIT6 = 0x12,
     UNIT7 = 0x0A,
     UNIT8 = 0x1A,
     UNIT9 = 0x0E,
     UNIT10 = 0x1E,
     UNIT11 = 0x06,
     UNIT12 = 0x16,
     UNIT13 = 0x00,
     UNIT14 = 0x10,
     UNIT15 = 0x08,
     UNIT16 = 0x18,
} device_code_t; // used to address an X-10 device within a house code
#define MIN_DEVICE_CODE UNIT13
#define MAX_DEVICE_CODE UNIT10
#define VALIDATE_DEVICE_CODE(device_code) assert((device_code) >= MIN_DEVICE_CODE && (device_code) <= MAX_DEVICE_CODE)


#ifdef MAIN_PROGRAM
const device_code_t unitNo[16] = 
{
     UNIT1,
     UNIT2,
     UNIT3,
     UNIT4,
     UNIT5,
     UNIT6,
     UNIT7,
     UNIT8,
     UNIT9,
     UNIT10,
     UNIT11,
     UNIT12,
     UNIT13,
     UNIT14,
     UNIT15,
     UNIT16,
};
#else
extern const device_code_t unitNo[];
#endif



typedef enum
{
     stateOff,
     stateOn,
     stateDim1,
     stateDim2,
     stateDim3,
     stateDim4,
     stateDim5,
     stateDim6,
     stateDim7,
     stateDim8,
     stateDim9,
     stateDim10,
     stateDim11,
     stateDim12,
     stateDim13,
     stateDim14,
     stateDim15,
     stateDim16,
     stateDim17,
     stateUnknown,
     stateRefreshWanted,
} device_state_t; // used for X10 device state
// *************************************************************************

#ifdef MAIN_PROGRAM
const char * deviceStateDescription[21] = 
{
     "Off",
     "On",
     "Dim1",
     "Dim2",
     "Dim3",
     "Dim4",
     "Dim5",
     "Dim6",
     "Dim7",
     "Dim8",
     "Dim9",
     "Dim10",
     "Dim11",
     "Dim12",
     "Dim13",
     "Dim14",
     "Dim15",
     "Dim16",
     "Dim17",
     "Unknown",
     "RefreshWanted",
};
#else
extern const char * deviceStateDescription[];
#endif

const int MAX_LAMP_BRIGHTNESS = 254;
const int BRIGHTNESS_STEP = (MAX_LAMP_BRIGHTNESS / 18);


#define MIN_DEVICE_STATE_CODE UNIT5
#define MAX_DEVICE_STATE_CODE UNIT10
#define VALIDATE_DEVICE_STATE_CODE(device_code) assert((device_code) >= stateOff && (device_code) <= stateDim17)

#define NO_DEVICES_PER_HOUSECODE (16)

typedef enum
{
     HouseA = 0x06,
     HouseB = 0x0E,
     HouseC = 0x02,
     HouseD = 0x0A,
     HouseE = 0x01,
     HouseF = 0x09,
     HouseG = 0x05,
     HouseH = 0x0D,
     HouseI = 0x07,
     HouseJ = 0x0F,
     HouseK = 0x03,
     HouseL = 0x0B,
     HouseM = 0x00,
     HouseN = 0x08,
     HouseO = 0x04,
     HouseP = 0x0C,
} house_code_t; // used to address particular house codes
#define MIN_HOUSE_CODE HouseM
#define MAX_HOUSE_CODE HouseJ
#define VALIDATE_HOUSE_CODE(house_code) assert((house_code) >= MIN_HOUSE_CODE && (house_code) <= MAX_HOUSE_CODE)
#define VALIDATE_MAC_ADDRESS(macAddress) assert(strlen(macAddress) == MAC_ADDRESS_LENGTH)

// structures of the code file written at compile time and read at runtime
// by the smart house suite.

typedef int fileTime_t;

typedef int deviceNo_t;
typedef int flagNo_t;
typedef int timerNo_t;
typedef int actionNo_t;
typedef int timeoutNo_t;
#define NULL_ACTION (-1)
typedef int actionItemOffset_t;

//1st record contains counts of other record types
#define MAGIC_CODE "SmartCode"
typedef struct
{
    char magicCode[9 + 1]; //file sanity check
    int noCalendarEntries;
    int noHouseCodeEntries;
    int noDeviceEntries;
    int noFlagEntries;
    int noTimerEntries;
    int noActionEntries;
    int noTimeoutEntries;
    int noTotalEntries; //file sanity check
} codeHeaderRecord_t;

typedef struct
{
    char end_marker[16 + 1];
} markerRecord_t;

typedef struct
{
    fileTime_t mMidnightStartDay;
    unsigned int muDayAttributes;
    fileTime_t mtSunRise;
    fileTime_t mtSunSet;
} codeCalendarRecord_t;

typedef enum
{
    deviceAppliance,
    deviceLamp,
    deviceApplianceLamp,
    deviceHueLamp, //Philips Hue Lamp connected via Philips hub.
    devicePIRSensor,
    deviceIRRemote,
	deviceTypeNone,
}  deviceType_t;

typedef struct
{
    house_code_t houseCode;
    char mszName[MAXRUNTIMEIDENTIFIERLENGTH + 1];
    int offActionNo;
    int onActionNo;
}  codeHouseCodeRecord_t;

typedef struct
{
    house_code_t houseCode;
    device_code_t deviceCode;
    deviceType_t deviceType;
    char macAddress[MAC_ADDRESS_LENGTH + 1];
    char mszName[MAXRUNTIMEIDENTIFIERLENGTH + 1];
    int offActionNo;
    int onActionNo;
}  codeDeviceRecord_t;


typedef struct
{
    char mszIdentifier[MAXRUNTIMEIDENTIFIERLENGTH + 1];
    int initialValue;
}  codeFlagRecord_t;

// Timer events comprise 3 levels of records (1) Timer, (2) Sequence, (3) Event
typedef struct
{
    char mszName[MAXRUNTIMEIDENTIFIERLENGTH + 1];
    int noSequencesInTimer;
}  codeTimerRecord_t;

typedef struct
{
    char mszName[MAXRUNTIMEIDENTIFIERLENGTH + 1];
    fileTime_t mSequenceFireTime; //local time that sequence fires (BST/GMT)
    unsigned int muDaysToFire; // types of days on which the sequence fires
    int noEventsInSequence;
}  codeSequenceRecord_t;

typedef struct
{
    fileTime_t mTimerFireTime; //relative time compared to sequence
    int fireActionNumber; // number of action (body) for event.
}  codeEventRecord_t;

typedef struct
{
    char mszName[MAXRUNTIMEIDENTIFIERLENGTH + 1];
    int noActionItemsInAction;
    int noProgramEntriesInAction;
}  codeActionRecord_t;


typedef struct
{
    time_t defaultDuration;
    char mszName[MAXRUNTIMEIDENTIFIERLENGTH + 1];
    int offActionNo;
}  codeTimeoutRecord_t;


#ifndef _WIN32
#define RUNTIME_FILENAME "/home/pi/smart8r/smart.smt"
#else
#define RUNTIME_FILENAME "D:\\usr\\richard\\projects\\Smart8r\\Smart8r\\smart.smt"
#endif



