#pragma once
/// <summary>
/// Simulate Reception of transmission from Raspberry Pi GPIO port.
/// </summary>

#if _WIN32

class gpioReceive
{
private:
	x10_message_t x10_first_received_message;
	x10_message_t x10_received_message;
	bool x10_message_received;

	int receive_counter;
	msg_state_t receive_state;
	unsigned long centiseconds_since_last_data_received;
	input_error_enum last_input_error;

	bool receive_data;
	bool first_time;

	gpioDevices* gpio_devices = new gpioDevices();
	
protected:
public:
	gpioReceive();
	x10_message_t processReceivedHalfBit(bool receive_half_bit);
	void createDevice(deviceType_t deviceType, house_code_t houseCode, device_code_t deviceCode);

};
#endif

