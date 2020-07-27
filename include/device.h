// *************************************************************************
// *************************************************************************
#pragma once

typedef enum
{
     selectOff,
     selectOn,
     selectOnUsed
} device_selected_t;
// *************************************************************************
typedef enum
{
    funAllUnitsOff = 0x01,
    funAllLightsOn = 0x03,
    funOn = 0x05,
    funOff = 0x07,
    funDim = 0x09,
    funBright = 0x0B
    //funAllLightsOff = 0x0D,
    //funPoll = 0x1f
} function_code_t;
// *************************************************************************
typedef enum
{
    delayedActionNone,
    delayedActionOff,
    delayedActionOn
} delayedAction_t;

//This class is the common ancestor of all Devices (X10 or Hue)
//It is an abstract class which should never be instantiated
class AbstractDevice
{
	private:
	protected:
	~AbstractDevice() = default;
	char mszName[MAXRUNTIMEIDENTIFIERLENGTH + 1];
        deviceType_t deviceType;

	public:
        AbstractDevice(deviceType_t deviceTypeParam, const char* mszNameParam);
        virtual void mSetWantedState(device_state_t wantedState, int colour, bool colourLoop, time_t switchOnTimeout, time_t switchOffTimeout);
        virtual void mSetState(device_state_t state);
        virtual void mSetColour(int colour);
        virtual void mSetColourLoop(bool colourLoop);
        deviceType_t GetDeviceType() { return deviceType; }
};

// *************************************************************************
// This class is the common ancestor of all X10 devices
// It is an abstract class which should never be instantiated
class CX10AbstractDevice : public AbstractDevice
{
    private:
    protected:
	~CX10AbstractDevice() = default;
	static bool allX10DevicesInWantedState;

        virtual void mFunctionAllUnitsOff(bool bExternal) = 0;
        virtual void mFunctionAllLightsOn(bool bExternal) = 0;
        virtual void mFunctionOn(bool bExternal) = 0;
        virtual void mFunctionOff(bool bExternal) = 0;
        virtual void mFunctionDim(bool bExternal) = 0;
        virtual void mFunctionBright(bool bExternal) = 0;
        virtual void mFunctionOther(function_code_t functionCode, bool bExternal) = 0;

    public:
        CX10AbstractDevice(): AbstractDevice(deviceTypeNone,"name")
        {
        };
        //virtual ~CX10AbstractDevice() {};

        void mFunction(function_code_t functionCode, bool bExternal);


        virtual void mSelect(bool bSelectYou) = 0;
        virtual device_selected_t mGetSelected() = 0;
        virtual char mGetStatus() = 0;
        virtual bool mGetReal()    {return false;};
        virtual bool mGetLamp()    {return false;};
        virtual device_state_t mGetWantedState()    {return stateUnknown;};
        virtual device_state_t mGetState()        {return stateUnknown;};
        virtual int mDimIncrementLevelsWanted()        {return 0;};
        virtual bool mStateChangeNeeded(device_state_t deviceState)        {return false;};
        virtual bool mStateChangePossible(device_state_t deviceState)        {return true;};
        virtual bool mStateDimmed()        {return false;};
        virtual bool mStateLampOnChangeNeeded()        {return false;};
        virtual bool mStateLampOnChangePossible()        {return true;};
        void mSetState(device_state_t iState)    {};
        static bool mGetAllDevicesInWantedState() {return allX10DevicesInWantedState;};
        static void mSetAllDevicesNowInWantedState() {allX10DevicesInWantedState = true;};
        virtual void printDeviceState() = 0;
        virtual void mCheckSwitchOffAndOnTimeout(time_t now) {};
        virtual void FireDelayedActions() {};
        virtual void mRefreshDevice() {};
};

// *************************************************************************
//This class is the device used for all slots which do not have an
//X10 device connected.
//Since this device does not exist, it does not hold any state.
//It ignores any message sent to it.
//Its accessor functions (mGetXXX) return constants.
class CX10NullDevice : public CX10AbstractDevice
{
    private:
    protected:
	virtual void mFunctionAllUnitsOff(bool bExternal) {};
        virtual void mFunctionAllLightsOn(bool bExternal) {};
        virtual void mFunctionOn(bool bExternal) {};
        virtual void mFunctionOff(bool bExternal) {};
        virtual void mFunctionDim(bool bExternal) {};
        virtual void mFunctionBright(bool bExternal) {};
        virtual void mFunctionOther(function_code_t functionCode, bool bExternal) {};

     public:
        CX10NullDevice()        {};
        ~CX10NullDevice() = default;

        void mSelect(bool bSelectYou)        {};
        device_selected_t mGetSelected()    {return selectOff;};
        char mGetStatus()        {return 'X';};
        device_state_t mGetState()    {return stateUnknown;};
        void printDeviceState() {};
};

// *************************************************************************
// It is an abstract class which should never be instanciated
class CX10ControllableDevice : public CX10AbstractDevice
{
    private:

    protected:
	~CX10ControllableDevice() = default;
	house_code_t mHouseCode;
        device_code_t mDeviceCode;
        device_state_t miState;
        device_selected_t mSelected;

    public:
        CX10ControllableDevice(house_code_t houseCode, device_code_t deviceCode, char *szName);
        device_selected_t mGetSelected()        {return mSelected;};
        char mGetStatus();
        device_state_t mGetState()    {return miState;};
        void mSetState(device_state_t iState)    {miState = iState;};
        void mSelect(bool bSelectYou);
};
// *************************************************************************
// This class is the common ancestor of all X10 devices with off/on actions
// It is an abstract class which should never be instanciated
class CX10ActionDevice : public CX10ControllableDevice
{
    private:
        actionNo_t mOffAction;
        actionNo_t mOnAction;

    protected:
        ~CX10ActionDevice() = default;
        delayedAction_t mDelayedAction;

    public:
        CX10ActionDevice(house_code_t houseCode, device_code_t deviceCode, char* szName,
             actionNo_t offAction = -1, actionNo_t onAction = -1);
        void FireDelayedActions();
};



// *************************************************************************
// This class is the common ancestor of all *REAL* X10 devices
// It is an abstract class which should never be instanciated
class CX10RealDevice : public CX10ActionDevice 
{
    private:
        void mCheckSwitchOnTimeout(time_t now);
        void mCheckSwitchOffTimeout(time_t now);

    protected:
        ~CX10RealDevice() = default;
        device_state_t miWantedState;
        time_t mSwitchOnTimeout;
        time_t mSwitchOffTimeout;

        virtual void mFunctionAllUnitsOff(bool bExternal);
        virtual void mFunctionAllLightsOn(bool bExternal);
        virtual void mFunctionOn(bool bExternal);
        virtual void mFunctionOff(bool bExternal);
        virtual void mFunctionDim(bool bExternal);
        virtual void mFunctionBright(bool bExternal);

        virtual void mFunctionOther(function_code_t functionCode, bool bExternal);

    public:
        CX10RealDevice(house_code_t houseCode, device_code_t deviceCode, char* szName,
             actionNo_t offAction = -1, actionNo_t onAction = -1);

        void mSetWantedState(device_state_t wantedState, int colour, bool colourLoop, time_t switchOnTimeout, time_t switchOffTimeout);
        device_state_t mGetWantedState()    {return miWantedState;};
        bool mGetReal()    {return true;};
        bool mStateChangeNeeded(device_state_t deviceState);
        bool mStateChangePossible(device_state_t deviceState);
        void mCheckSwitchOffAndOnTimeout(time_t now)
        {
            mCheckSwitchOnTimeout(now);
            mCheckSwitchOffTimeout(now);
        };
        void printDeviceState();
        void mRefreshDevice()
        {
#if DEBUG
            logging.logInfo("Refresh device %s\n", mszName);
#endif

            miState = stateRefreshWanted;
            allX10DevicesInWantedState = false;
        };
};


// *************************************************************************
//This class is the device used for appliance X10 devices
class CX10ApplianceDevice : public CX10RealDevice
{
protected:
	~CX10ApplianceDevice() = default;
public:
        CX10ApplianceDevice(house_code_t houseCode, device_code_t deviceCode, char *szName,
             actionNo_t offAction = -1, actionNo_t onAction = -1);
};

// *************************************************************************
//This class is the device used for lamp X10 devices.
//This class supports dimming.
class CX10LampDevice : public CX10RealDevice
{
    protected:
	~CX10LampDevice() = default;
	virtual void mFunctionAllLightsOn(bool bExternal);
        virtual void mFunctionDim(bool bExternal);
        virtual void mFunctionBright(bool bExternal);

    public:
        CX10LampDevice(house_code_t houseCode, device_code_t deviceCode, char *szName);
        bool mGetLamp()    {return true;};
        bool mStateDimmed();
        bool mStateLampOnChangeNeeded();
        bool mStateLampOnChangePossible();
        int mDimIncrementLevelsWanted();
};



// *************************************************************************
class CX10ApplianceLampDevice : public CX10RealDevice
{
    protected:
	~CX10ApplianceLampDevice() = default;
	virtual void mFunctionAllLightsOn(bool bExternal);

    public:
        CX10ApplianceLampDevice(house_code_t houseCode, device_code_t deviceCode, char *szName);
        bool mGetLamp()    {return false;};
};

// *************************************************************************

class CX10PIRSensorDevice : public CX10ActionDevice 
{
    protected:
		~CX10PIRSensorDevice() = default;
		virtual void mFunctionAllUnitsOff(bool bExternal);
        virtual void mFunctionAllLightsOn(bool bExternal);
        virtual void mFunctionOn(bool bExternal);
        virtual void mFunctionOff(bool bExternal);
        virtual void mFunctionDim(bool bExternal);
        virtual void mFunctionBright(bool bExternal);
        virtual void mFunctionOther(function_code_t functionCode, bool bExternal);

    public:
        CX10PIRSensorDevice(house_code_t houseCode, device_code_t deviceCode, char *szName,
             actionNo_t offAction = -1, actionNo_t onAction = -1);
        void printDeviceState() {}; //no state to print for PIR
};

// *************************************************************************

class CX10IRRemoteDevice : public CX10ControllableDevice
{
    protected:
	~CX10IRRemoteDevice() = default;
	virtual void mFunctionAllUnitsOff(bool bExternal);
        virtual void mFunctionAllLightsOn(bool bExternal);
        virtual void mFunctionOn(bool bExternal);
        virtual void mFunctionOff(bool bExternal);
        virtual void mFunctionDim(bool bExternal);
        virtual void mFunctionBright(bool bExternal);
        virtual void mFunctionOther(function_code_t functionCode, bool bExternal);

    public:
        CX10IRRemoteDevice(house_code_t houseCode, device_code_t deviceCode, char *szName);
        void mSetWantedState(device_state_t wantedState, int colour, bool colourLoop, time_t switchOnTimeout, time_t switchOffTimeout);
        void printDeviceState() {}; //no state to print for Infra Red transmitter.
};

class HueAbstractDevice;
class HueLampDevice;
