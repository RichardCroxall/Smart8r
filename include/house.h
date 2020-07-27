// This class looks after all X10 devices on all 16 house codes.
#pragma once
#include <forward_list>

class CX10House
{
    private:
        char mszName[MAXRUNTIMEIDENTIFIERLENGTH + 1];
        CX10HouseCode *mHouseCodes[16];
    protected:
    public:

        CX10House(const char *szName);
        //~CX10House()            {};

        void mMessage(int iMessage, bool bExternal);
        void mMessage(house_code_t houseCode, function_code_t functionCode, bool bExternal);
        void mMessage(house_code_t houseCode, device_code_t deviceCode);

        bool mUpdateX10Devices();
        bool mUpdateHueDevices();
        void mLinkDevice(house_code_t houseCode, device_code_t deviceCode, CX10ControllableDevice* device);
        void mLinkDevice(house_code_t houseCode, HueLampDevice* device);
        void mSetAllDevicesUnknown();
        void mLinkHouseCode(house_code_t houseCode, CX10HouseCode* housecodePtr);
        void printDeviceStates();
        void mCheckSwitchOffAndOnTimeout(time_t now);
        void mRefreshDevices();

};

#ifdef MAIN_PROGRAM
CX10House house("Barn End");
#else
extern CX10House house;
#endif

