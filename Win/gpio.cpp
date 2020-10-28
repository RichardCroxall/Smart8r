#include "../include/runtime.h"

#ifdef _WIN32

generalPurposeInputOutput::generalPurposeInputOutput()
{
}

/// <summary>
/// timeIntoMainsCycleInMicroseconds - return number of microseconds into simulated mains cycle.
/// </summary>
/// <returns></returns>
long generalPurposeInputOutput::timeIntoMainsCycleInMicroseconds()
{
	const intmax_t ticks = (std::chrono::steady_clock::now() - beginning_of_time).count();

	const intmax_t ticks_into_mains_cycle = ticks % ticks_per_mains_cycle;
	return (long) (ticks_into_mains_cycle / ticks_per_microsecond);
}

long generalPurposeInputOutput::timeIntoHalfMainsCycleInMicroseconds()
{
    const intmax_t ticks = (std::chrono::steady_clock::now() - beginning_of_time).count();

    const intmax_t ticks_into_mains_cycle = ticks % ticks_per_half_mains_cycle;
    return (long)(ticks_into_mains_cycle / ticks_per_microsecond);
}

/// <summary>
/// zeroCrossing - return true if we are in the first half of the mains cycle (false for the 2nd half).
/// </summary>
/// <returns></returns>
bool generalPurposeInputOutput::zeroCrossing()
{
	bool zeroCrossingstate = false;

	const long time_into_mains_cycle_in_microseconds = timeIntoMainsCycleInMicroseconds();
	if (time_into_mains_cycle_in_microseconds < microSecondsPerMainsCycle / 2)
	{
		zeroCrossingstate = true;
	}
	return zeroCrossingstate;
}

bool generalPurposeInputOutput::receive(bool firstSample)
{
	return !gpio_transmit->receiveHalfBit(firstSample);
}

void generalPurposeInputOutput::transmit(bool receive_half_bit, bool transmit)
{
#ifdef TEST
	if (transmit)
	{
		const intmax_t ticks = (std::chrono::steady_clock::now() - beginning_of_time).count();
		tickTimeStruct_t tickTimeStruct{ ticks / 1000, receive_half_bit };
		times.emplace_back(tickTimeStruct);
		if (times.size() > 300)
		{
			bool stop = true;
		}
	}
#endif
	const long time_into_mains_cycle_in_microseconds = timeIntoHalfMainsCycleInMicroseconds();
	assert(time_into_mains_cycle_in_microseconds >= 0);
	if (transmit)
	{
		//assert(time_into_mains_cycle_in_microseconds <= 100);
	}
	else
	{
		//assert(time_into_mains_cycle_in_microseconds <= 1500);
    }

	if (time_into_mains_cycle_in_microseconds < 1000 && transmit)
	{
		x10_message_t resultX10Message = gpio_receive->processReceivedHalfBit(receive_half_bit);
		if (resultX10Message != 0)
		{
			gpio_transmit->setTransmitX10Message(resultX10Message);
		}
	}
    else
    {
		LogAssert(receive_half_bit == false);
    }
}


///
///for Windows simulation create virtual X-10 devices which will validate outgoing X-10 messages.
///
void generalPurposeInputOutput::createDevice(deviceType_t deviceType, house_code_t houseCode, device_code_t deviceCode)
{
	gpio_receive->createDevice(deviceType, houseCode, deviceCode);
}


#endif
