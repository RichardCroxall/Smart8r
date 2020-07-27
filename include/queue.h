// *************************************************************************
// 16 devices, 17 brights are maximum, but also need INFRA red commands
#pragma once
#define MAX_QUEUE_SIZE (16 + 18 + 20)

#if DEBUG
#ifdef MAIN_PROGRAM
const char *szHouseCode[16] =
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

const char *szFunctionCode[32] =
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
     "?15",
     "UNIT14",
     "?17?",
     "UNIT6",
     "?19?",
     "UNIT4",
     "?21?",
     "UNIT12",
     "?23?",
     "UNIT16",
     "?25?",
     "UNIT8",
     "?27?",
     "UNIT2",
     "?29?",
     "UNIT10",
     "funPoll"
};
#else
extern const char *szHouseCode[];
extern const char *szFunctionCode[];
#endif
#endif
class CX10MessageQueue
{
    private:
        int mQueue[MAX_QUEUE_SIZE];
        int mQueueSize;
        int mQueueHead;
        bool mQueueContainsDimOrBright;
        bool mHouseCodeHasOtherTransmitters;
        void addQueue(int iMessage);

    protected:
    public:
        std::mutex mQueueMutex;
	
        void clearQueue();
        CX10MessageQueue();
        void restartQueue();
        int getQueueSize();
        int getFullQueueSize();
        void CheckForHouseCodeTransmitters(house_code_t houseCode);
        void addQueue(house_code_t houseCode, int deviceNumber);
        void addQueue(house_code_t houseCode, device_code_t devicecode);
        void addQueue(house_code_t houseCode, function_code_t functionCode);
        int nextMessage();
        bool getQueueContainsDimOrBright();
        bool getHouseCodeHasOtherTransmitters();
};


#ifdef MAIN_PROGRAM
CX10MessageQueue x10MessageQueue;
#else
extern CX10MessageQueue x10MessageQueue;
#endif


