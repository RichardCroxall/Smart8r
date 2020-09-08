#include "include/runtime.h"
#include "include/huedevice.h"

#include "huehelper/include/HueMacAddressMap.h"
#include "huehelper/include/HueQueue.h"

extern CX10MessageQueue x10MessageQueue;
extern HueQueue hueMessageQueue;
extern HueRQueue reverseHueMessageQueue;
extern HueMacAddressMap macAddressLightNoMap;
extern HueMacAddressMap macAddressDeviceNoMap;

const int AllOffRepeatCount = 3;

CX10HouseCode::CX10HouseCode(house_code_t houseCode, actionNo_t allOffAction, actionNo_t allLightsOnAction)
{
    mAllOffAction = allOffAction;
    mSomeOneHomeAction = allLightsOnAction;
    mHouseCode = houseCode;

    for (int i = 0; i < 16; i++)
    {
        mDevice[i] =(class CX10AbstractDevice*) &mNullDevice;
    }
}

void CX10HouseCode::mLinkDevice(device_code_t deviceCode, CX10ControllableDevice* device)
{
    assert(device != NULL);
    mDevice[arrayMap->deviceCodeToInt[deviceCode]] = device;
}

void CX10HouseCode::mLinkDevice(HueLampDevice* device)
{
    assert(device != NULL);
    hue_lamp_devices.push_back(device);
}

void CX10HouseCode::mSetAllDevicesUnknown()
{
    for (int i = 0; i < 16; i++)
    {
        mDevice[i]->mSetState(stateUnknown);
    }
	
    for (HueLampDevice* device : hue_lamp_devices)
    {
        device->mSetState(stateUnknown);
    }
}

void CX10HouseCode::mAutomaticHouseCodeAction(function_code_t functionCode, bool bExternal)
{
    // if someone presses a button, then there must be someone in
    // except if they just pressed AllUnitsOff to go out.
    if (bExternal)
    {
        if (functionCode == funAllUnitsOff)
        {
            if (mAllOffAction != NULL_ACTION)
            {
                loadMap.action[mAllOffAction]->fire(true);
            }
        }
        else
        {
            if (mSomeOneHomeAction != NULL_ACTION)
            {
                loadMap.action[mSomeOneHomeAction]->fire(true);
            }
        }
    }
}

void CX10HouseCode::mFunction(function_code_t functionCode, bool bExternal)
{
    mAutomaticHouseCodeAction(functionCode, bExternal);

    for (int i = 0; i < 16; i++)
    {
        mDevice[i]->mFunction(functionCode, bExternal);
    }

    for (int i = 0; i < 16; i++)
    {
        mDevice[i]->FireDelayedActions();
    }

}

void CX10HouseCode::mSelect(device_code_t deviceCode)
{
    for (int i = 0; i < 16; i++)
    {
        mDevice[i]->mSelect(arrayMap->deviceCodeToInt[deviceCode] == i);
    }
}

bool CX10HouseCode::mUpdateDevices()
{
#ifdef DEBUG44
    std::cout << "waiting for mutex 2\n";
#endif
	
    // ensure other thread does not mess whilst we are filling the queue.
    std::lock_guard<std::mutex> guard(x10MessageQueue.mQueueMutex);
	
#ifdef DEBUG44
    std::cout << "waited for mutex 2\n";
#endif
	
    // try different codes one at a time until we generate an optimum message

    // clear queue of outgoing messages
    x10MessageQueue.clearQueue();

    if (!mUpdateDevicesAllUnitsOff())
    {
        if (!mUpdateDevicesBrightUnknown())
        {
            if (!mUpdateDevicesAllLightsOn())
            {
                if (!mUpdateDevicesOn())
                {
                    if (!mUpdateDevicesOff())
                    {
                        if (!mUpdateDevicesDim())
                        {
                            if (!mUpdateDevicesBright())
                            {
                                assert(x10MessageQueue.getQueueSize() == 0);
                                return false;
                            }
                        }
                    }
                }
            }
        }
    }
    assert(x10MessageQueue.getQueueSize() > 0);
    return true;
}

bool CX10HouseCode::mUpdateDevicesAllUnitsOff()
{
    // try for an all off code.
    {
        bool bAllDevicesOffWanted = true;
        int iDevicesNeedChange = 0;
        int iDevicesWithUnknownState = 0;

        for (int i = 0; i < 16; i++)
        {
            if (mDevice[i]->mStateChangeNeeded(stateOff))
            {
                iDevicesNeedChange++;
            }

            if (mDevice[i]->mGetState() == stateUnknown)
            {
                iDevicesWithUnknownState++;
            }

            if (!mDevice[i]->mStateChangePossible(stateOff))
            {
                bAllDevicesOffWanted = false;
            }
        }

        if (iDevicesNeedChange > 0 && 
            (bAllDevicesOffWanted || iDevicesWithUnknownState == 16))
        {
            // send a message and quit
            for (int i = 0; i < AllOffRepeatCount; i++)
            {
                x10MessageQueue.addQueue(mHouseCode, funAllUnitsOff);
            }

            logging.logInfo("mUpdateDevicesAllUnitsOff() returned true\n");
            return true;
        }
    }
    return false;
}

bool CX10HouseCode::mUpdateDevicesBrightUnknown()
{
    // Special for lights in an unknown state which should be on/dimmed.
    // They could already be dimmed, so brighten them up to full, so
    // at least we accurately know their current state.
    {
        bool bSomeLampsUnknown = false;

        for (int i = 0; i < 16; i++)
        {
            if (mDevice[i]->mGetLamp())
            {
                if (mDevice[i]->mGetWantedState() != stateOff &&
                    mDevice[i]->mGetWantedState() != stateUnknown &&
                    mDevice[i]->mGetState() == stateUnknown)
                {
                    //note what we are doing
                    bSomeLampsUnknown = true;

                     // select device device
                     x10MessageQueue.addQueue(mHouseCode, i);
                }
            }
        }

        if (bSomeLampsUnknown)
        {
            // send lots of bright messages to the selected lamps and quit
            for (int i = 0; i < 17; i++)
            {
                x10MessageQueue.addQueue(mHouseCode, funBright);
            }
            //logging.logInfo("mUpdateDevicesBrightUnknown() returned true\n");
            return true;
        }
    }
    return false;
}

bool CX10HouseCode::mUpdateDevicesAllLightsOn()
{
     // try for an all lights on code.
    {
        bool bAllLightsOnWanted = true;
        int iDevicesNeedChange = 0;

        for (int i = 0; i < 16; i++)
        {
             if (mDevice[i]->mGetLamp())
             {
                //if lamps are off and they want to be dimmed,
                //we send a funOn/funAllLightsOn first

                if (mDevice[i]->mStateLampOnChangeNeeded())
                {
                    iDevicesNeedChange++;
#if DEBUG
                    logging.logInfo("housecode %d device %d mStateLampOnChangeNeeded %d\n", mHouseCode, i,iDevicesNeedChange);
#endif
                }
                if (!mDevice[i]->mStateLampOnChangePossible())
                {
#if DEBUG
                    logging.logInfo("housecode %d device %d not mStateLampOnChangePossible\n", mHouseCode, i);
#endif
                     bAllLightsOnWanted = false;
                }
            }
        }

        if (iDevicesNeedChange > 0 && bAllLightsOnWanted)
        {
            // send a message and quit
            x10MessageQueue.addQueue(mHouseCode, funAllLightsOn);
            //logging.logInfo("mUpdateDevicesAllLightsOn() returned true\n");
            return true;
        }
    }
    return false;
}

bool CX10HouseCode::mUpdateDevicesOn()
{

    // try for selected devices on
    {
        bool bSomeDevicesOnWanted = false;

        for (int i = 0; i < 16; i++)
        {
            // Lamps that are already dimmed cannot be brought
            //to stateOn by an funOn function. They need to be brightened.

            if (mDevice[i]->mStateChangeNeeded(stateOn) &&
                !mDevice[i]->mStateDimmed())
            {
                //note what we are doing
                bSomeDevicesOnWanted = true;

                // select device 
                x10MessageQueue.addQueue(mHouseCode, i);
            }
        }

        if (bSomeDevicesOnWanted)
        {
            // send the on message to the selected devices and quit
            x10MessageQueue.addQueue(mHouseCode, funOn);
            //logging.logInfo("mUpdateDevicesOn() returned true\n");
            return true;
        }
    }

    return false;
}

bool CX10HouseCode::mUpdateDevicesOff()
{
     // try for selected devices off
    {
        bool bSomeDevicesOffWanted = false;

        for (int i = 0; i < 16; i++)
        {
            if (mDevice[i]->mGetReal())
            {
                if (mDevice[i]->mGetWantedState() == stateOff &&
                mDevice[i]->mGetState() != stateOff)
                {
                     //note what we are doing
                     bSomeDevicesOffWanted = true;

                     // select device 
                     x10MessageQueue.addQueue(mHouseCode, i);
                }
            }
        }

        if (bSomeDevicesOffWanted)
        {
            // send the off message to the selected devices and quit
            x10MessageQueue.addQueue(mHouseCode, funOff);
            //logging.logInfo("mUpdateDevicesOff() returned true\n");
            return true;
        }
    }
    return false;
}

bool CX10HouseCode::mUpdateDevicesDim()
{

     // now try for several lights dimmed by the same number of levels
    {
        int iMinimumLevels = 18;

        for (int i = 0; i < 16; i++)
        {
            const int iLevels = mDevice[i]->mDimIncrementLevelsWanted(); //DIM is positive

            if (iLevels > 0 && iLevels < iMinimumLevels)
            {
                 iMinimumLevels = iLevels;
            }
        }

        if (iMinimumLevels < 18)
        {
            assert(iMinimumLevels > 0);
             // logging.logInfo("dim Min %d\n", iMinimumLevels);

            for (int i = 0; i < 16; i++)
            {
                const int iLevels = mDevice[i]->mDimIncrementLevelsWanted(); //DIM is positive
                if (iLevels >= iMinimumLevels)
                {
                    // select all lamps needing same minimum (or more)
                    // number of levels dimming.
                    x10MessageQueue.addQueue(mHouseCode, i);
                }
            }

            while (iMinimumLevels > 0)
            {
                x10MessageQueue.addQueue(mHouseCode, funDim);
                iMinimumLevels--;
            }
            //logging.logInfo("mUpdateDevicesDim() returned true\n");
            return true;
        }
    }
    return false;
}

bool CX10HouseCode::mUpdateDevicesBright()
{
     // now try for several lights brightened by the same number of levels
    {
        int iMinimumLevels = 18;

        for (int i = 0; i < 16; i++)
        {
            const int iLevels = - mDevice[i]->mDimIncrementLevelsWanted(); //unary - makes BRIGHT positive

            if (iLevels > 0 && iLevels < iMinimumLevels)
            {
                 iMinimumLevels = iLevels;
            }
        }

        if (iMinimumLevels < 18)
        {
            assert(iMinimumLevels > 0);
             // logging.logInfo("bright Min %d\n", iMinimumLevels);

            for (int i = 0; i < 16; i++)
            {
                const int iLevels = - mDevice[i]->mDimIncrementLevelsWanted(); //unary - makes BRIGHT positive
                if (iLevels >= iMinimumLevels)
                {
                    // select all lamps needing same minimum
                    // number of levels brightening.
                    x10MessageQueue.addQueue(mHouseCode, i);
                }
            }
            while (iMinimumLevels > 0)
            {
                x10MessageQueue.addQueue(mHouseCode, funBright);
                iMinimumLevels--;
            }
            //logging.logInfo("mUpdateDevicesBright() returned true\n");
            return true;
        }
    }

     // no changes to devices needed.
     return false;
}

static int CalcBrightness(device_state_t deviceState)
{
    assert(deviceState >= stateOff && deviceState <= stateDim17);
	
    int brightness = 0;
	if (deviceState >= stateOn &&
        deviceState <= stateDim17)
	{
        brightness = MAX_LAMP_BRIGHTNESS - ((int)deviceState - (int)stateOn) * BRIGHTNESS_STEP;
	}
	
    return brightness;
}

HueLampCommand_t CreateCommandColourLoop(bool colourLoop)
{
    HueLampCommand_t hue_lamp_command;
    hue_lamp_command.lampCommand = LampSetColourLoop;
    hue_lamp_command.onion.onOff = colourLoop;
    return hue_lamp_command;
}

HueLampCommand_t CreateCommandColour(int colour)
{
    assert(colour >= 0 && colour <= 0xFFFFFF);
	
    HueLampCommand_t hue_lamp_command;
    hue_lamp_command.lampCommand = LampSetRGBColour;
    hue_lamp_command.onion.rgbColour = colour;
    return hue_lamp_command;
}

HueLampCommand_t CreateCommandSetState(device_state_t device_state)
{
    HueLampCommand_t hue_lamp_command;
    hue_lamp_command.lampCommand = LampSetState;
    hue_lamp_command.onion.brightness = CalcBrightness(device_state);
    return hue_lamp_command;
}

bool CX10HouseCode::updateHueDevices()
{
    bool changeNeeded = false;
	for (HueLampDevice* hueLampDevice : hue_lamp_devices)
	{
        if (hueLampDevice->mGetWantedState() != hueLampDevice->mGetState() ||
            hueLampDevice->mGetWantedColour() != hueLampDevice->mGetColour() ||
            hueLampDevice->mGetWantedColourLoop() != hueLampDevice->mGetColourLoop())
        {
            HueQueueMessage_t hue_queue_message;
            hue_queue_message.macAddress = hueLampDevice->mGetMacAddress();

            if (hueLampDevice->mGetWantedState() != hueLampDevice->mGetState())
            {
                hue_queue_message.commandList.push_back(CreateCommandSetState(hueLampDevice->mGetWantedState()));
            }
        	
            if (hueLampDevice->mGetWantedColour() != hueLampDevice->mGetColour())
            {
                hue_queue_message.commandList.push_back(CreateCommandColour(hueLampDevice->mGetWantedColour()));
            }

            if (hueLampDevice->mGetWantedColourLoop() != hueLampDevice->mGetColourLoop())
            {
                hue_queue_message.commandList.push_back(CreateCommandColourLoop(hueLampDevice->mGetWantedColourLoop()));
            }

            hueMessageQueue.pushMessage(hue_queue_message);
            changeNeeded = true;
        }
	}
    return changeNeeded;
}

void CX10HouseCode::printDeviceStates()
{
    for (int i = 0; i < 16; i++)
    {
         mDevice[i]->printDeviceState();
    }
	
    for (HueLampDevice* device : hue_lamp_devices)
    {
        device->printDeviceState();
    }
}

void CX10HouseCode::mCheckSwitchOffAndOnTimeout(time_t now)
{
    for (int i = 0; i < 16; i++)
    {
         mDevice[i]->mCheckSwitchOffAndOnTimeout(now);
    }
	
    for (HueLampDevice* device : hue_lamp_devices)
    {
        device->mCheckSwitchOffAndOnTimeout(now);
    }
}

void CX10HouseCode::mRefreshDevices()
{
    for (int i = 0; i < 16; i++)
    {
        mDevice[i]->mRefreshDevice();
    }
	
    for (HueLampDevice* device : hue_lamp_devices)
    {
        device->mRefreshDevice();
    }
}
