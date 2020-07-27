#include "include/runtime.h"


void CX10House::mMessage(int iMessage, bool bExternal)
{
    const int iHouseCode = iMessage >> 5;
    int houseMessage = iMessage & 0x1F;

    assert(iHouseCode < 16);

    if ((iMessage & 1) != 0)
    {
        mHouseCodes[iHouseCode]->mFunction((function_code_t)houseMessage, bExternal);
    }
    else
    {
        mHouseCodes[iHouseCode]->mSelect((device_code_t)houseMessage);
    }
}

void CX10House::mMessage(house_code_t houseCode, function_code_t functionCode, bool bExternal)
{
    mHouseCodes[houseCode]->mFunction(functionCode, bExternal);
}

void CX10House::mMessage(house_code_t houseCode, device_code_t deviceCode)
{
    mHouseCodes[houseCode]->mSelect(deviceCode);
}


void CX10House::mLinkDevice(house_code_t houseCode, device_code_t deviceCode, CX10ControllableDevice* device)
{
	assert((int)houseCode < 16);
	mHouseCodes[(int)houseCode]->mLinkDevice(deviceCode, device);
}

void CX10House::mLinkDevice(house_code_t houseCode, HueLampDevice* device)
{
    mHouseCodes[(int)houseCode]->mLinkDevice(device);
}

void CX10House::mLinkHouseCode(house_code_t houseCode, CX10HouseCode* housecodePtr)
{
    assert((int)houseCode < 16);
    mHouseCodes[(int)houseCode]= housecodePtr;
}

CX10House::CX10House(const char *szName)
{
    assert(strlen(szName) < sizeof(mszName));
    strcpy(mszName, szName);

    for (int i = 0; i < 16; i++)
    {
        mHouseCodes[i] = new CX10HouseCode((house_code_t) i, NULL_ACTION, NULL_ACTION);
    }
}

bool CX10House::mUpdateX10Devices()
{
    bool bSuccess = false;

    int i = 0;
    while (i < 16 && !bSuccess)
    {
        bSuccess = mHouseCodes[i]->mUpdateDevices();
        i++;
    }
    return bSuccess;
}

bool CX10House::mUpdateHueDevices()
{
    bool bSuccess = false;
    int i = 0;
	while (i < 16 && !bSuccess)
	{
        bSuccess = mHouseCodes[i]->updateHueDevices();
        i++;
	}
    return bSuccess;
}

void CX10House::mSetAllDevicesUnknown()
{
    for (int i = 0; i < 16; i++)
    {
        mHouseCodes[i]->mSetAllDevicesUnknown();
    }
}

void CX10House::printDeviceStates()
{
    for (int i = 0; i < 16; i++)
    {
        mHouseCodes[i]->printDeviceStates();
    }
}

void CX10House::mCheckSwitchOffAndOnTimeout(time_t now)
{
    for (int i = 0; i < 16; i++)
    {
        mHouseCodes[i]->mCheckSwitchOffAndOnTimeout(now);
    }
}

void CX10House::mRefreshDevices()
{
    logging.logDebug("RefreshDevices\n");

    for (int i = 0; i < 16; i++)
    {
        mHouseCodes[i]->mRefreshDevices();
    }
}

