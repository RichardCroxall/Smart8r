#include "include/runtime.h"
#include "include/huedevice.h"

// ************************************************************************* Hue ******************************************************
bool HueLampDevice::allHueDevicesInWantedState = false;

// ******************************************************
HueLampDevice::HueLampDevice(house_code_t houseCode, char* mszMacAddressParam, char* nameParam) : HueAbstractDevice(deviceHueLamp, mszMacAddressParam, nameParam)
{
#ifndef DEBUG4
    logging.logDebug("HueLampDevice device %s mszMacAddress %s %s\n", mszMacAddressParam, nameParam);
#endif
    mHouseCode = houseCode;
    miState = stateOff;
    miColour = 0x000000;
    mbColourLoop = false;

    miWantedState = stateOff;
    miWantedColour = 0x000000;
    mbWantedColourLoop = false;

    mSwitchOnTimeout = THE_END_OF_TIME;
    mSwitchOffTimeout = THE_END_OF_TIME;
    mDelayedAction = delayedActionNone;
}


// *************************************************************************
void HueLampDevice::mSetWantedState(device_state_t wantedState, int colour, bool colourLoop, time_t switchOnTimeout, time_t switchOffTimeout)
{
#ifdef DEBUG
    logging.logInfo("device %s wanted state = %d switchontime %d switchofftime %d\n", mszName, wantedState, switchOnTimeout, switchOffTimeout);
#endif
    assert(switchOnTimeout == THE_END_OF_TIME || switchOnTimeout != switchOffTimeout);
    assert(wantedState != stateUnknown);

    mSwitchOnTimeout = switchOnTimeout;
    mSwitchOffTimeout = switchOffTimeout;

    //if wanted state takes place immediately
    if ((wantedState == stateOn && switchOnTimeout <= tickingClock.getTime()) ||
        (wantedState == stateOff && switchOffTimeout <= tickingClock.getTime()) ||
        (wantedState != stateOn && wantedState != stateOff))
    {
        //if wanted state takes place immediately, action immediately and reset timer to avoid it firing again
        if ((wantedState != stateOff && switchOnTimeout <= tickingClock.getTime()))
        {
            miWantedState = wantedState;
            miWantedColour = colour;
            mbWantedColourLoop = colourLoop;
            mSwitchOnTimeout = THE_END_OF_TIME;
            allHueDevicesInWantedState = false;
        }
    }

    //if wanted state takes place immediately, action immediately and reset timer to avoid it firing again
    if ((wantedState == stateOff && switchOffTimeout <= tickingClock.getTime()))
    {
        miWantedState = wantedState;
        miWantedColour = colour;
        mbWantedColourLoop = colourLoop;
        mSwitchOffTimeout = THE_END_OF_TIME;
        allHueDevicesInWantedState = false;
    }
};

void HueLampDevice::printDeviceState()
{
    printf("%30s %5s %06x %1s (%5s %06x %1s) On %s", mszName, deviceStateDescription[miState], miColour, mbColourLoop ? "T":"F",
        deviceStateDescription[miWantedState], miWantedColour, mbWantedColourLoop ? "T" : "F",
        formatTime(mSwitchOnTimeout));
    printf(" Off %s\n", formatTime(mSwitchOffTimeout)); //print 2nd time separately to avoid corrupting static variable in formatTime
}

void HueLampDevice::mRefreshDevice()
{
#if DEBUG
    logging.logInfo("Refresh device %s\n", mszName);
#endif

    miState = stateRefreshWanted;
    miColour = -1;
    mbColourLoop = false;
    allHueDevicesInWantedState = false;
};


void HueLampDevice::mCheckSwitchOffAndOnTimeout(time_t now)
{
    mCheckSwitchOnTimeout(now);
    mCheckSwitchOffTimeout(now);
};

void HueLampDevice::mCheckSwitchOnTimeout(time_t now)
{
    if (now >= mSwitchOnTimeout)
    {
#if DEBUG
        logging.logInfo("Switch On Timeout %s now=%ld mSwitchOnTimeout=%ld mSwitchOffTimeout=%ld\n", mszName, now, mSwitchOnTimeout, mSwitchOffTimeout);
#endif

        mSwitchOnTimeout = THE_END_OF_TIME;
        miWantedState = stateOn;
        if (miState != miWantedState ||
            miColour != miWantedColour ||
            mbColourLoop != mbWantedColourLoop)
        {
            allHueDevicesInWantedState = false;
        }
    }
}
void HueLampDevice::mCheckSwitchOffTimeout(time_t now)
{
    if (now >= mSwitchOffTimeout)
    {
#if DEBUG
        logging.logInfo("Switch Off Timeout %s now=%ld mSwitchOnTimeout=%ld mSwitchOffTimeout=%ld\n", mszName, now, mSwitchOnTimeout, mSwitchOffTimeout);
#endif
        mSwitchOffTimeout = THE_END_OF_TIME;
        LogAssert(mSwitchOnTimeout == THE_END_OF_TIME);
        mSwitchOnTimeout = THE_END_OF_TIME;
        miWantedState = stateOff;
        if (miState != miWantedState ||
            miColour != miWantedColour ||
            mbColourLoop != mbWantedColourLoop)
        {
            allHueDevicesInWantedState = false;
        }
    }
}


