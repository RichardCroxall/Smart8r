#pragma once
class gpioDevice
{
private:
	deviceType_t _deviceType;
	house_code_t _houseCode;
	device_code_t _deviceCode;
	device_selected_t _deviceSelected = selectOff;
	
protected:
public:
	gpioDevice(deviceType_t deviceType, house_code_t houseCode, device_code_t deviceCode);

	void Message(x10_message_t x10_message);
};

