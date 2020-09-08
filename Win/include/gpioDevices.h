#pragma once

class gpioDevices
{
private:
	gpioDevice *gpio_device[16][16];
	
protected:
public:
	gpioDevices();
	void createDevice(deviceType_t deviceType, house_code_t houseCode, device_code_t deviceCode);
	void Message(x10_message_t x10Message);
};

