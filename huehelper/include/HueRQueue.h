#pragma once
#include <assert.h>

#include <list>
#include <mutex>
#include <queue>


enum class HueCommandSuccessEnum
{
	ok,
	hubDown,
	lampNotKnown,
	lampFailed,
};

typedef struct
{
	LampCommandEnum lampCommand;
	union
	{
		int rgbColour;
		//device_state_t state;
		int brightness;
		bool onOff;
	} onion;
	HueCommandSuccessEnum hueCommandSuccess;
} HueLampCommandSuccess_t;

typedef struct
{
	std::string macAddress;
	std::list<HueLampCommandSuccess_t> commandList;
} HueReverseQueueMessageSuccess_t;


class HueRQueue
{
private:
	//queue of messages for the Philips Hue hub
	std::queue< HueReverseQueueMessageSuccess_t> hueRQueue;
	std::mutex rQueueMutex;

	int fastQueueSize = 0;
protected:
public:
	void pushMessage(HueReverseQueueMessageSuccess_t hueLampCommandSuccess)
	{
		std::lock_guard<std::mutex> guard(rQueueMutex);
		hueRQueue.push(hueLampCommandSuccess);
		fastQueueSize++;
	}

	int QueueSize()
	{
		return fastQueueSize;
	}

	HueReverseQueueMessageSuccess_t popMessage()
	{
		std::lock_guard<std::mutex> guard(rQueueMutex);

		fastQueueSize--;
		assert(fastQueueSize >= 0);
		const HueReverseQueueMessageSuccess_t result = hueRQueue.front();
		hueRQueue.pop();

		return result;
	}

};


