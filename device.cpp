#include "include/runtime.h"

extern CX10MessageQueue x10MessageQueue;

// *************************************************************************

AbstractDevice::AbstractDevice(deviceType_t deviceTypeParam, const char* mszNameParam)
{
    deviceType = deviceTypeParam;
    strncpy(mszName, mszNameParam, sizeof(mszName));
}

void AbstractDevice::mSetWantedState(device_state_t wantedState, int colour, bool colourLoop, time_t switchOnTimeout, time_t switchOffTimeout)
{
    logging.logDebug("controllable device %s mSetWantedState %d\n", mszName, wantedState);
    assert(false);
};

void AbstractDevice::mSetState(device_state_t state)
{
    logging.logDebug("controllable device %s mSetState %d\n", mszName, state);
    assert(false);
};

void AbstractDevice::mSetColour(int colour)
{
    logging.logDebug("controllable device %s mSetWantedcolour %d\n", mszName, colour);
    assert(false);
};

void AbstractDevice::mSetColourLoop(bool colourLoop)
{
    logging.logDebug("controllable device %s ColourLoop %d\n", mszName, colourLoop);
    assert(false);
};


bool CX10AbstractDevice::allX10DevicesInWantedState = false;

// *************************************************************************

void CX10AbstractDevice::mFunction(function_code_t functionCode, bool bExternal)
{
    switch (functionCode)
    {
        case funAllUnitsOff:
            mFunctionAllUnitsOff(bExternal);
            break;

        case funAllLightsOn:
            mFunctionAllLightsOn(bExternal);
            break;

        case funOn:
            mFunctionOn(bExternal);
            break;

        case funOff:
            mFunctionOff(bExternal);
            break;

        case funBright:
            mFunctionBright(bExternal);
            break;

        case funDim:
            mFunctionDim(bExternal);
            break;

        default:
            mFunctionOther(functionCode, bExternal);
            break;
    }
}

// *************************************************************************
CX10ControllableDevice::CX10ControllableDevice(house_code_t houseCode, device_code_t deviceCode, char *szName)
{
    mHouseCode = houseCode;
    mDeviceCode = deviceCode;
    assert(strlen(szName) < sizeof(mszName));
    strcpy(mszName, szName);

    mSelected = selectOff;
    miState = stateUnknown;
};

// *************************************************************************
void CX10ControllableDevice::mSelect(bool bSelectYou)
{
    if (bSelectYou)
    {
        mSelected = selectOn;
    }
    else
    {
        if (mSelected == selectOnUsed)
        {
            mSelected = selectOff;
#if DEBUG22
            logging.logInfo("deselected %s\n", mszName);
#endif
        }
    }
}

// *************************************************************************
char CX10ControllableDevice::mGetStatus()
{
    const char szDeviceStatus[] = "01ABCDEFGHIJKLMNOPQhtf+_-678912345?";

    return szDeviceStatus[miState];
};

// *************************************************************************
// This class is the common ancestor of all devices with actions
// It is an abstract class which should never be instanciated
CX10ActionDevice::CX10ActionDevice(house_code_t houseCode, device_code_t deviceCode, char *szName,
         actionNo_t offAction, actionNo_t onAction) :
                    CX10ControllableDevice(houseCode, deviceCode, szName)
{
    mDelayedAction = delayedActionNone;
    mOffAction = offAction;
    mOnAction = onAction;
};
// *************************************************************************
void CX10ActionDevice::FireDelayedActions()
{
    switch(mDelayedAction)
    {
        case delayedActionOff:
            if (mOffAction != NULL_ACTION)
            {
                loadMap.action[mOffAction]->fire(true);
            }
            break;

        case delayedActionOn:
            if (mOnAction != NULL_ACTION)
            {
                loadMap.action[mOnAction]->fire(true);
            }
            break;

        default:
            //no action required
            break;
    }
    mDelayedAction = delayedActionNone;
}

// *************************************************************************
// This class is the common ancestor of all *REAL* X10 devices
// It is an abstract class which should never be instanciated

CX10RealDevice::CX10RealDevice(house_code_t houseCode, device_code_t deviceCode, char *szName,
         actionNo_t offAction, actionNo_t onAction) :
                    CX10ActionDevice(houseCode, deviceCode, szName, offAction, onAction)
{
    miWantedState = stateOff;
    mSwitchOnTimeout = THE_END_OF_TIME;
    mSwitchOffTimeout = THE_END_OF_TIME;
};

// *************************************************************************
void CX10RealDevice::mCheckSwitchOnTimeout(time_t now)
{
    if (now >= mSwitchOnTimeout)
    {
#if DEBUG
        logging.logInfo("Switch On Timeout %s now=%ld mSwitchOnTimeout=%ld mSwitchOffTimeout=%ld\n", mszName, now, mSwitchOnTimeout, mSwitchOffTimeout);
#endif

        mSwitchOnTimeout = THE_END_OF_TIME;
        miWantedState = stateOn;
        if (miState != miWantedState)
        {
            allX10DevicesInWantedState = false;
        }
    }
}
// *************************************************************************
void CX10RealDevice::mCheckSwitchOffTimeout(time_t now)
{
    if (now >= mSwitchOffTimeout)
    {
#if DEBUG
        logging.logInfo("Switch Off Timeout %s now=%ld mSwitchOnTimeout=%ld mSwitchOffTimeout=%ld\n", mszName, now, mSwitchOnTimeout, mSwitchOffTimeout);
#endif
        mSwitchOffTimeout = THE_END_OF_TIME;
        assert(mSwitchOnTimeout == THE_END_OF_TIME);
        mSwitchOnTimeout = THE_END_OF_TIME;
        miWantedState = stateOff;
        if (miState != miWantedState)
        {
            allX10DevicesInWantedState = false;
        }
    }
}
// *************************************************************************
void CX10RealDevice::mSetWantedState(device_state_t wantedState, int colour, bool colourLoop, time_t switchOnTimeout, time_t switchOffTimeout)
{
#ifdef DEBUG
    logging.logInfo("device %s wanted state = %d switchontime %d switchofftime %d\n", mszName,  wantedState, switchOnTimeout, switchOffTimeout);
#endif
    assert(switchOnTimeout == THE_END_OF_TIME || switchOnTimeout != switchOffTimeout);
    assert(wantedState != stateUnknown);

    mSwitchOnTimeout = switchOnTimeout;
    mSwitchOffTimeout = switchOffTimeout;

    //if wanted state takes place immediately, action immediately and reset timer to avoid it firing again
    if ((wantedState != stateOff && switchOnTimeout <= tickingClock.getTime()))
    {
        miWantedState = wantedState;
        if (miState != miWantedState)
        {
            allX10DevicesInWantedState = false;
        }
        mSwitchOnTimeout = THE_END_OF_TIME;
    }

    //if wanted state takes place immediately, action immediately and reset timer to avoid it firing again
    if ((wantedState == stateOff && switchOffTimeout <= tickingClock.getTime()))
    {
        miWantedState = wantedState;
        if (miState != miWantedState)
        {
            allX10DevicesInWantedState = false;
        }
        mSwitchOffTimeout = THE_END_OF_TIME;
    }
};
// *************************************************************************

bool CX10RealDevice::mStateChangeNeeded(device_state_t consideredDeviceState)
{
    bool needed = false;

    if (miState != consideredDeviceState &&
        miWantedState  == consideredDeviceState)
    {
        needed = true;
    }
    return needed;
}
// *************************************************************************
bool CX10RealDevice::mStateChangePossible(device_state_t consideredDeviceState)
{
    bool possible = false;

    if (miWantedState == consideredDeviceState)
    {
        possible = true;
    }
    return possible;
}
// *************************************************************************
void CX10RealDevice::mFunctionAllLightsOn(bool bExternal)
{
    //This default is used for appliance devices. They ignore all lights on, so do nothing
};
// *************************************************************************
void CX10RealDevice::mFunctionAllUnitsOff(bool bExternal)
{
#if DEBUG
    if (miState != stateOff)
    {
        logging.logDebug("%s off\n", mszName);
    }
#endif

    mSelected = selectOff;
    miState = stateOff;
    if (bExternal)
    {
        miWantedState = miState;
        mDelayedAction = delayedActionOff;
        mSwitchOffTimeout = THE_END_OF_TIME;
        mSwitchOnTimeout = THE_END_OF_TIME;
    }
};
// *************************************************************************
void CX10RealDevice::mFunctionOn(bool bExternal)
{
    if (mSelected == selectOn || mSelected == selectOnUsed)
    {
#if DEBUG
        if (miState != stateOn)
        {
            logging.logDebug("%s on\n", mszName);
        }
#endif

        miState = stateOn;
        if (bExternal)
        {
            miWantedState = miState;
            mDelayedAction = delayedActionOn;
            mSwitchOffTimeout = tickingClock.getTime() + 12 * 3600;
        }
        mSelected = selectOnUsed;
    }
};
// *************************************************************************
void CX10RealDevice::mFunctionOff(bool bExternal)
{
    if (mSelected == selectOn || mSelected == selectOnUsed)
    {
#if DEBUG
        if (miState != stateOff)
        {
            logging.logDebug("%s off\n", mszName);
        }
#endif
        miState = stateOff;
        if (bExternal)
        {
            miWantedState = miState;
            mDelayedAction = delayedActionOff;
            mSwitchOffTimeout = THE_END_OF_TIME;
            mSwitchOnTimeout = THE_END_OF_TIME;
        }
        mSelected = selectOnUsed;
    }
};
// *************************************************************************
void CX10RealDevice::mFunctionDim(bool bExternal)
{
    if (mSelected == selectOn || mSelected == selectOnUsed)
    {
#if DEBUG
        logging.logDebug("%s can't be dimmed or brightened\n", mszName);
#endif
        mSelected = selectOnUsed;
    }
};
// *************************************************************************
void CX10RealDevice::mFunctionBright(bool bExternal)
{
    if (mSelected == selectOn || mSelected == selectOnUsed)
    {
#if DEBUG
        logging.logInfo("%s can't be dimmed or brightened\n", mszName);
#endif
        mSelected = selectOnUsed;
    }
};
// *************************************************************************
void CX10RealDevice::mFunctionOther(function_code_t functionCode, bool bExternal)
{
#if DEBUG
    logging.logError("%s impossible function: %d ext: %d\n", mszName, functionCode, bExternal);
#endif
};
// *************************************************************************
void CX10RealDevice::printDeviceState()
{
    printf("%30s %5s ------ - (%5s ------ -) On %s", mszName, deviceStateDescription[miState], deviceStateDescription[miWantedState], formatTime(mSwitchOnTimeout));
    printf(" Off %s\n", formatTime(mSwitchOffTimeout)); //print 2nd time separately to avoid corrupting static variable in formatTime
}

//This class is the device used for appliance X10 devices
CX10ApplianceDevice::CX10ApplianceDevice(house_code_t houseCode, device_code_t deviceCode, char *szName,
         actionNo_t offAction, actionNo_t onAction) :
                 CX10RealDevice(houseCode, deviceCode, szName, offAction, onAction)
{
}


// *************************************************************************
//This class is the device used for lamp X10 devices.
//This class supports dimming.
CX10LampDevice::CX10LampDevice(house_code_t houseCode, device_code_t deviceCode,
     char *szName) : CX10RealDevice(houseCode, deviceCode, szName)
{
}

// *************************************************************************
bool CX10LampDevice::mStateDimmed()
{
    return miState >= stateDim1 && miState <= stateDim17;
}
// *************************************************************************
int CX10LampDevice::mDimIncrementLevelsWanted()
{
    int incrementLevels  = 0;
    if (miWantedState != stateOff &&
        (miState != stateUnknown && miState != stateRefreshWanted))
    {
        if (miState == stateOff)
        {
            //special case. We now reckon that going from OFF state with a single DIM command takes you to ON. (rather than DIM1)
            incrementLevels = miWantedState - stateOff; //positive = DIM, negative = Bright
        }
        else
        {
            incrementLevels = miWantedState - miState; //positive = DIM, negative = Bright
        }
    }
    return incrementLevels;
}

// *************************************************************************
bool CX10LampDevice::mStateLampOnChangeNeeded()
{
    bool needed = false;

    if ((miState == stateOff || miState == stateUnknown || miState == stateRefreshWanted || miState == stateRefreshWanted) &&
        miWantedState >= stateOn &&
        miWantedState <= stateDim17)
    {
        needed = true;
    }
    return needed;

}

// *************************************************************************
bool CX10LampDevice::mStateLampOnChangePossible()
{
    bool possible = false;

    if (miWantedState >= stateOn &&
        miWantedState <= stateDim17)
    {
        possible = true;
    }
    return possible;
}

// *************************************************************************
void CX10LampDevice::mFunctionAllLightsOn(bool bExternal)
{
    if (miState == stateOff || miState == stateUnknown || miState == stateRefreshWanted)
    {
#if DEBUG
        logging.logInfo("%s on\n", mszName);
#endif
        miState = stateOn;
        if (bExternal)
        {
            mSwitchOffTimeout = tickingClock.getTime() + 12 * 3600; // 12 hours
            miWantedState = miState;
            mDelayedAction = delayedActionOn;
        }
    }
};
// *************************************************************************
void CX10LampDevice::mFunctionDim(bool bExternal)
{
    if (mSelected == selectOn || mSelected == selectOnUsed)
    {
        mSelected = selectOnUsed;
        if (miState == stateOff || miState == stateUnknown)
        {
#if DEBUG
            logging.logDebug("%s on\n", mszName);
#endif
            miState = stateOn;
            if (bExternal)
            {
                miWantedState = miState;
                mSwitchOffTimeout = tickingClock.getTime() + 12 * 3600; // 12 hours
            }
        }
        else if (miState >= stateOn && miState < stateDim17)
        {
            miState = (device_state_t) ((int) miState + 1);
            if (bExternal)
            {
                miWantedState = miState;
                mSwitchOffTimeout = tickingClock.getTime() + 12 * 3600; // 12 hours
            }
#if DEBUG9
            logging.logDebug("%s dimmed %d\n", mszName, miState);
#endif
        }
        else if (miState == stateRefreshWanted)
        {
            //it is too disruptive to take lights up to full and back down to dim again, so just assume it is ok.
            miState = miWantedState;
            logging.logDebug("DIM refresh of dimmed light ignored %s\n", mszName);
        }
    }
};
// *************************************************************************
void CX10LampDevice::mFunctionBright(bool bExternal)
{
    if (mSelected == selectOn || mSelected == selectOnUsed)
    {
        mSelected = selectOnUsed;
        if (miState == stateOff || miState == stateUnknown)
        {
#if DEBUG
            logging.logDebug("%s on\n", mszName);
#endif
            miState = stateOn;
            if (bExternal)
            {
                miWantedState = miState;
                mSwitchOffTimeout = tickingClock.getTime() + 12 * 3600; //12 hours
            }
        }
        else if (miState > stateOn && miState <= stateDim17)
        {
            miState = (device_state_t) ((int) miState - 1);
            if (bExternal)
            {
                miWantedState = miState;
                mSwitchOffTimeout = tickingClock.getTime() + 12 * 3600; //12 hours
            }
#if DEBUG9
            logging.logDebug("%s brightened %d\n", mszName, miState);
#endif
        }
        else if (miState == stateRefreshWanted)
        {
            //it is too disruptive to take lights up to full and back down to dim again, so just assume it is ok.
            miState = miWantedState;
            logging.logDebug("BRIGHT refresh of dimmed light ignored %s\n", mszName);
        }
    }
};
// *************************************************************************
// *************************************************************************
//This class is the device used for appliance lamp X10 devices.
//This class supports dimming.
CX10ApplianceLampDevice::CX10ApplianceLampDevice(house_code_t houseCode, device_code_t deviceCode,
 char *szName) : CX10RealDevice(houseCode, deviceCode, szName)
{
}

// *************************************************************************
void CX10ApplianceLampDevice::mFunctionAllLightsOn(bool bExternal)
{
    if (miState == stateOff || miState == stateUnknown || miState == stateRefreshWanted)
    {
        if (bExternal)
        {
            allX10DevicesInWantedState = false;
            miWantedState = stateOn;
            mSwitchOffTimeout = tickingClock.getTime() + 12 * 3600; //12 hours
            logging.logInfo("appliancelamp %s wanted on\n", mszName);
        }
    }
};

// *************************************************************************
//This class is the device used for Passive Infra Red X10 devices.
CX10PIRSensorDevice::CX10PIRSensorDevice(house_code_t houseCode, device_code_t deviceCode, char *szName,
         actionNo_t offAction, actionNo_t onAction) : CX10ActionDevice(houseCode, deviceCode, szName, offAction, onAction)
{
}

// *************************************************************************
void CX10PIRSensorDevice::mFunctionAllUnitsOff(bool bExternal)
{
    warning(bExternal);
    logging.logError("impossible AllUnitsOff external %d\n", bExternal);
};
// *************************************************************************
void CX10PIRSensorDevice::mFunctionAllLightsOn(bool bExternal)
{
#if DEBUG
    logging.logError("%s impossible function: all lights on ext: %d\n", mszName, bExternal);
#endif
};
// *************************************************************************
void CX10PIRSensorDevice::mFunctionOn(bool bExternal)
{
    warning(bExternal);
    if (mSelected == selectOn || mSelected == selectOnUsed)
    {
#if DEBUG
        if (miState != stateOn)
        {
            logging.logDebug("%s received on\n", mszName);
        }
#endif
        if (bExternal)
        {
            mDelayedAction = delayedActionOn;
        }
        mSelected = selectOnUsed;
    }
};
// *************************************************************************
void CX10PIRSensorDevice::mFunctionOff(bool bExternal)
{
    warning(bExternal);
            if (mSelected == selectOn || mSelected == selectOnUsed)
            {
#if DEBUG
                if (miState != stateOff)
                {
                    logging.logDebug("%s received off\n", mszName);
                }
#endif
                miState = stateOff;
                if (bExternal)
                {
                    mDelayedAction = delayedActionOff;
                }
                mSelected = selectOnUsed;
            }
};
// *************************************************************************
void CX10PIRSensorDevice::mFunctionDim(bool bExternal)
{
#if DEBUG
    logging.logError("%s impossible function: Dim ext: %d\n", mszName, bExternal);
#endif
};
// *************************************************************************
void CX10PIRSensorDevice::mFunctionBright(bool bExternal)
{
#if DEBUG
    logging.logError("%s impossible function: Bright ext: %d\n", mszName, bExternal);
#endif
};
// *************************************************************************
void CX10PIRSensorDevice::mFunctionOther(function_code_t functionCode, bool bExternal)
{
#if DEBUG
    logging.logError("%s impossible function: %d ext: %d\n", mszName, functionCode, bExternal);
#endif
};

// *************************************************************************
//This class is the device used for IR Remote control X10 devices.
//This class does not support dimming.
CX10IRRemoteDevice::CX10IRRemoteDevice(house_code_t houseCode, device_code_t deviceCode,
     char *szName) : CX10ControllableDevice(houseCode, deviceCode, szName)
{
}

// *************************************************************************
void CX10IRRemoteDevice::mFunctionAllUnitsOff(bool bExternal)
{
    if (bExternal)
    {
        logging.logError("impossible AllUnitsOff external %d\n", bExternal);
    }
    warning(bExternal);
};
// *************************************************************************
void CX10IRRemoteDevice::mFunctionAllLightsOn(bool bExternal)
{
    if (bExternal)
    {
        logging.logError("impossible AllLightsOn external %d\n", bExternal);
    }
    warning(bExternal);
};
// *************************************************************************
void CX10IRRemoteDevice::mFunctionOn(bool bExternal)
{
    if (bExternal)
    {
        logging.logInfo("Remotecontroldevice functioncode On external %d\n",  bExternal);
    }
};
// *************************************************************************
void CX10IRRemoteDevice::mFunctionOff(bool bExternal)
{
    if (bExternal)
    {
        logging.logInfo("Remotecontroldevice functioncode Off external %d\n",  bExternal);
    }
};
// *************************************************************************
void CX10IRRemoteDevice::mFunctionDim(bool bExternal)
{
    if (bExternal)
    {
        logging.logInfo("Remotecontroldevice functioncode Dim external %d\n",  bExternal);
    }
    warning(bExternal);
};
// *************************************************************************
void CX10IRRemoteDevice::mFunctionBright(bool bExternal)
{
    if (bExternal)
    {
        logging.logInfo("Remotecontroldevice functioncode Bright external %d\n",  bExternal);
    }
    warning(bExternal);
};
// *************************************************************************
void CX10IRRemoteDevice::mFunctionOther(function_code_t functionCode, bool bExternal)
{
    warning(bExternal);
    logging.logError("impossible Other external %d\n", bExternal);
};
// *************************************************************************
void CX10IRRemoteDevice::mSetWantedState(device_state_t wantedState, int colour, bool colourLoop, time_t switchOnTimeout, time_t switchOffTimeout)
{
#if DEBUG4
    logging.logDebug("IRRemote device %s housecode %d deviceCode %d mSetWantedState %d\n", mszName, mHouseCode, mDeviceCode, wantedState);
#endif
    x10MessageQueue.addQueue(mHouseCode, mDeviceCode);
    switch (wantedState)
    {
        case stateOff:
            x10MessageQueue.addQueue(mHouseCode, funOff);
            break;

        case stateOn:
            x10MessageQueue.addQueue(mHouseCode, funOn);
            break;

        default:
#if DEBUG
            logging.logInfo("wantedState %d\n", wantedState);
#endif
            assert(false);
            break;
     }
};

