#include "../include/runtime.h"


gpioDevices::gpioDevices()
{
	for (int houseCode = 0; houseCode < 16; houseCode++)
	{
		for (int deviceCode = 0; deviceCode < 16; deviceCode++)
		{
			gpio_device[houseCode][deviceCode] = nullptr;
		}
	}
}
void gpioDevices::createDevice(deviceType_t deviceType, house_code_t houseCode, device_code_t deviceCode)
{
	gpio_device[arrayMap->houseCodeToInt[houseCode]][arrayMap->deviceCodeToInt[deviceCode]] = new gpioDevice(deviceType, houseCode, deviceCode);
}

void gpioDevices::Message(x10_message_t x10Message)
{
	const house_code_t houseCode = arrayMap->x10ToHouseCode(x10Message);
	//const house_code_t houseCode = (house_code_t) (x10Message >> 5);
	
	for (int iDeviceCode = 0; iDeviceCode < 16; iDeviceCode++)
	{
		device_code_t deviceCode = arrayMap->intToDeviceCode[iDeviceCode];
		
		if (gpio_device[arrayMap->houseCodeToInt[houseCode]][iDeviceCode] != nullptr)
		{
			gpio_device[arrayMap->houseCodeToInt[houseCode]][iDeviceCode]->Message(x10Message);
		}
	}
}

