#include "include/HueMacAddressMap.h"

int HueMacAddressMap::findHueLight(const std::string macAddress)
{
    int lampNumber = -1;

    if (macAddressMap.count(macAddress) > 0)
    {
        lampNumber = macAddressMap[macAddress];
    }
    return lampNumber;
}

void HueMacAddressMap::AddHueLight(const std::string macAddress, int lampNumber)
{
    macAddressMap[macAddress] = lampNumber;
}

