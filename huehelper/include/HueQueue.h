#pragma once
#include <assert.h>
#include <list>
#include <mutex>
#include <queue>

enum LampCommandEnum
{
	LampSetRGBColour,
	LampSetColourLoop,
	LampSetState,
};

typedef struct
{
	LampCommandEnum lampCommand;
	union
	{
		int rgbColour;
		int brightness;
		bool onOff;
	} onion;
} HueLampCommand_t;

typedef struct
{
	std::string macAddress;
	std::list<HueLampCommand_t> commandList;
} HueQueueMessage_t;


class HueQueue
{
private:
	//queue of messages for the Philips Hue hub
	std::queue< HueQueueMessage_t> hueQueue;
	std::mutex queueMutex;

	int fastQueueSize = 0;
	
protected:
	
public:
	void pushMessage(HueQueueMessage_t hueQueueMessage)
	{
		std::lock_guard<std::mutex> guard(queueMutex);
		hueQueue.push(hueQueueMessage);
		fastQueueSize++;
	}

	int getQueueSize()
	{
		return fastQueueSize;
	}

	HueQueueMessage_t popMessage()
	{
		std::lock_guard<std::mutex> guard(queueMutex);

		fastQueueSize--;
		assert(fastQueueSize >= 0);
		const HueQueueMessage_t result = hueQueue.front();
		hueQueue.pop();

		return result;
	}

};

