
#include "include/runtime.h"

//***************************************************************************************************
static const char *houseCode[16] =
{
     "M",
     "E",
     "C",
     "K",
     "O",
     "G",
     "A",
     "I",
     "N",
     "F",
     "D",
     "L",
     "P",
     "H",
     "B",
     "J",
};
//***************************************************************************************************

static const char *functionCode[32] =
{
     "UNIT13",
     "funAllUnitsOff",
     "UNIT5",
     "funAllLightsOn",
     "UNIT3",
     "funOn",
     "UNIT11",
     "funOff",
     "UNIT15",
     "funDim",
     "UNIT7",
     "funBright",
     "UNIT1",
     "funAllLightsOff",
     "UNIT9",
     "?",
     "UNIT14",
     "?",
     "UNIT6",
     "?",
     "UNIT4",
     "?",
     "UNIT12",
     "?",
     "UNIT16",
     "?",
     "UNIT8",
     "?",
     "UNIT2",
     "?",
     "UNIT10",
     "funPoll"
};
//***************************************************************************************************
char *messageDescription(x10_message_t x10_received_message)
{
    static char description[40 + 1];

                sprintf(description, "Mess %02x House %s %s", 
                                                x10_received_message,
                                                houseCode[((int)x10_received_message >> 5) & 0x0f],
                                                functionCode[(int)x10_received_message & 0x1f]);

                return description;
}
