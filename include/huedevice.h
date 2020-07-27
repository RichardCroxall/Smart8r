#pragma once
// *************************************************************************
//
// 
//Hue Devices.
//***********************************************
class HueAbstractDevice : public AbstractDevice
{
private:
protected:
    char mszMacAddress[MAXRUNTIMEIDENTIFIERLENGTH + 1];

public:
    HueAbstractDevice(deviceType_t deviceTypeParam, const char* mszMacAddressParam, const char* mszNameParam) : AbstractDevice(deviceTypeParam, mszNameParam)
    {
        strncpy(mszMacAddress, mszMacAddressParam, sizeof(mszMacAddress));
    }
};

//***********************************************
//static device_state_t CalcDeviceState(int brightness);

//***********************************************
class HueLampDevice : public HueAbstractDevice
{
private:
    void mCheckSwitchOnTimeout(time_t now);
    void mCheckSwitchOffTimeout(time_t now);

protected:
    static bool allHueDevicesInWantedState;

    house_code_t mHouseCode;
    delayedAction_t mDelayedAction;
    device_state_t miWantedState;
    device_state_t miState;
    int miWantedColour;
    int miColour;
    bool mbWantedColourLoop;
    bool mbColourLoop;

    time_t mSwitchOnTimeout;
    time_t mSwitchOffTimeout;

    void mFunctionAllLightsOn(bool bExternal);
    void mFunctionAllUnitsOff(bool bExternal);

public:
    static bool mGetAllDevicesInWantedState() { return allHueDevicesInWantedState; };
    static void mSetAllDevicesNowInWantedState() { allHueDevicesInWantedState = true; };
	
    HueLampDevice(house_code_t houseCode, char* mszMacAddressParam, char* nameParam);
    bool mGetLamp() { return true; };
    bool mStateLampOnChangeNeeded();
    bool mStateLampOnChangePossible() const;
    int mDimIncrementLevelsWanted();
    void mSetWantedState(device_state_t wantedState, int colour, bool colourLoop, time_t switchOnTimeout, time_t switchOffTimeout);
    void mSetState(device_state_t state) { assert(state >= stateOff && state <= stateRefreshWanted);  miState = state; };
    void mSetColour(int colour) { assert(colour >= 0 && colour <= 0xFFFFFF);  miColour = colour; };
    void mSetColourLoop(bool colourLoop) { assert(colourLoop == false || colourLoop == true); mbColourLoop = colourLoop; };
    void printDeviceState();
    void mRefreshDevice();
    void mCheckSwitchOffAndOnTimeout(time_t now);
    device_state_t mGetWantedState() { return miWantedState; };
    device_state_t mGetState() { return miState; };
    int mGetWantedColour() { return miWantedColour; }
    int mGetColour() { return miColour; }
    bool mGetWantedColourLoop() { return mbWantedColourLoop; }
    bool mGetColourLoop() { return mbColourLoop; }
    //void applyChanges(std::forward_list<HueLampCommandSuccess_t>commandList);
    std::string mGetMacAddress() { return mszMacAddress; }
};




