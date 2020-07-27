
#pragma once
#include <list>

static int CalcBrightness(device_state_t deviceState);

class CX10HouseCode
{
    private:
        CX10NullDevice mNullDevice;
        CX10AbstractDevice *mDevice[16];
        actionNo_t mAllOffAction;
        actionNo_t mSomeOneHomeAction;
        house_code_t mHouseCode;
        std::list<HueLampDevice*> hue_lamp_devices;

        void mAutomaticHouseCodeAction(function_code_t functionCode, bool bExternal);
    protected:

    public:
        CX10HouseCode(house_code_t houseCode, actionNo_t allOffAction, actionNo_t allLightsOnAction);
        //~CX10HouseCode()                {};

        void mFunction(function_code_t functionCode, bool bExternal);
        void mSelect(device_code_t deviceCode);
        bool mUpdateDevices();
        bool mUpdateDevicesAllUnitsOff();
        bool mUpdateDevicesBrightUnknown();
        bool mUpdateDevicesAllLightsOn();
        bool mUpdateDevicesOn();
        bool mUpdateDevicesOff();
        bool mUpdateDevicesDim();
        bool mUpdateDevicesBright();
        void mLinkDevice(device_code_t deviceCode, CX10ControllableDevice* device);
        void mLinkDevice(HueLampDevice* device);

        void mSetAllDevicesUnknown();
        void printDeviceStates();
        void mCheckSwitchOffAndOnTimeout(time_t now);
        void mRefreshDevices();
        bool updateHueDevices();
};
