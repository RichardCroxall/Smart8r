#pragma once
#ifdef _WIN32
#include <chrono>
class generalPurposeInputOutput
{
	
private:
	const int mains_frequency = 50; //Hz in UK, Europe.
	
	const intmax_t ticks_per_mains_cycle = std::chrono::steady_clock::period::den / (mains_frequency * std::chrono::steady_clock::period::num);
	const intmax_t ticks_per_half_mains_cycle = std::chrono::steady_clock::period::den / (2 * mains_frequency * std::chrono::steady_clock::period::num);
	const intmax_t ticks_per_microsecond = std::chrono::steady_clock::period::den / (1000000 * std::chrono::steady_clock::period::num);
	const intmax_t microSecondsPerMainsCycle = 1000000 / mains_frequency;

	const std::chrono::steady_clock::time_point beginning_of_time = std::chrono::steady_clock::now();
	
	long timeIntoMainsCycleInMicroseconds();
	long timeIntoHalfMainsCycleInMicroseconds();
	
	gpioReceive* gpio_receive = new gpioReceive();
	gpioTransmit* gpio_transmit = new gpioTransmit();

protected:
public:
	generalPurposeInputOutput();
	
	bool zeroCrossing();
	bool receive(bool firstSample);
	void transmit(bool receive_half_bit, bool transmit);
	void createDevice(deviceType_t deviceType, house_code_t houseCode, device_code_t deviceCode);

};
#endif
