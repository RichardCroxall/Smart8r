#include <assert.h>
#include <algorithm>    // std::min

#include "../hueplusplus/include/Hue.h"

#ifdef _WIN32
#include "../hueplusplus/include/WinHttpHandler.h"
#else
#include "../hueplusplus/include/LinHttpHandler.h"
#endif

#include "include/HueConnection.h"
#include "include/HueQueue.h"
#include "include/HueRQueue.h"
#include "include/HueMacAddressMap.h"
#include "include/HueThread.h"


#include "../hueplusplus/include/ColorUnits.h"

#include "../include/defines.h"
#include "../include/clock.h"
#include "../include/logging.h"

extern HueQueue hueMessageQueue;
extern HueRQueue reverseHueMessageQueue;
HueMacAddressMap macAddressLightNoMap;


HueThread::HueThread()
{
}

void HueThread::ConstructFailureMessage(const HueQueueMessage_t hueMessage, HueReverseQueueMessageSuccess_t hueReverseQueueMessage, HueCommandSuccessEnum hue_command_success)
{
	for (HueLampCommand_t hueLampCommand : hueMessage.commandList)
	{
		HueLampCommandSuccess_t hue_lamp_command_success;
		hue_lamp_command_success.lampCommand = hueLampCommand.lampCommand;
		hue_lamp_command_success.hueCommandSuccess = hue_command_success;
		hueReverseQueueMessage.commandList.push_back(hue_lamp_command_success);
	}
}

void HueThread::processMessage(const HueQueueMessage_t& hueMessage)
{
	HueReverseQueueMessageSuccess_t hueReverseQueueMessage;
	hueReverseQueueMessage.macAddress = hueMessage.macAddress;
	
	int lampNumber = macAddressLightNoMap.findHueLight(hueMessage.macAddress);
	
	if (hueConnection != nullptr && hueConnection->connectedToHub)
    {
		if (lampNumber >= 0)
		{
			hueplusplus::HueLight hueLight = hueConnection->hueBridge->lights().get(lampNumber);
			hueplusplus::StateTransaction stateTransaction = hueLight.transaction();

		    for (HueLampCommand_t hueLampCommand : hueMessage.commandList)
		    {
		        HueLampCommandSuccess_t hue_lamp_command_success;
		        hue_lamp_command_success.lampCommand = hueLampCommand.lampCommand;

		        switch (hueLampCommand.lampCommand)
		        {
			        case LampSetState:
						assert(hueLampCommand.onion.brightness >= 0 && hueLampCommand.onion.brightness <= 254);
			            hue_lamp_command_success.onion.brightness = hueLampCommand.onion.brightness;
		        		//TODO simplify
						if (hueLampCommand.onion.brightness == 0)
						{
							stateTransaction.setColorLoop(false);
							stateTransaction.setOn(false);

						}
						else
						{
							stateTransaction.setOn(true);
							stateTransaction.setBrightness(hueLampCommand.onion.brightness);
						}
#ifdef DEBUG
						logging.logDebug("Hue Lamp %d %d\n", lampNumber, hueLampCommand.onion.brightness);
#endif
						break;

			        case LampSetColourLoop:
						assert(hueLampCommand.onion.onOff == false || hueLampCommand.onion.onOff == true);
						stateTransaction.setColorLoop(hueLampCommand.onion.onOff);
			            hue_lamp_command_success.onion.onOff = hueLampCommand.onion.onOff;
#ifdef DEBUG
						logging.logDebug("Hue Lamp %d colour loop %d\n", lampNumber, hueLampCommand.onion.onOff);
#endif
						break;
		        	
			        case LampSetRGBColour:
			        {
						assert(hueLampCommand.onion.rgbColour >= 0 && hueLampCommand.onion.rgbColour <= 0xFFFFFF);
						hueplusplus::RGB rgb{ (uint8_t)((hueLampCommand.onion.rgbColour >> 16) & 0xFF),
											 (uint8_t)((hueLampCommand.onion.rgbColour >> 8) & 0xFF),
											 (uint8_t)(hueLampCommand.onion.rgbColour & 0xFF) };
						stateTransaction.setColor(rgb.toXY());
			            hue_lamp_command_success.onion.rgbColour = hueLampCommand.onion.rgbColour;
#ifdef DEBUG
						logging.logDebug("Hue Lamp %d colour r%d g%d b%d\n", lampNumber, rgb.r, rgb.g, rgb.b);
#endif
						break;
			        }

			        default:
			            assert(false);
			            break;
			    }
		        hue_lamp_command_success.hueCommandSuccess = HueCommandSuccessEnum::ok;
		        hueReverseQueueMessage.commandList.push_back(hue_lamp_command_success);
		    }

			//send message to lamp
			bool success = stateTransaction.commit();
			//apply success or otherwise to all the command just send as a single JSON message.
			for (HueLampCommandSuccess_t hue_lamp_command_success : hueReverseQueueMessage.commandList)
			{
				hue_lamp_command_success.hueCommandSuccess = success ? HueCommandSuccessEnum::ok : HueCommandSuccessEnum::lampFailed;
			}
		}
		else
		{
			ConstructFailureMessage(hueMessage, hueReverseQueueMessage, HueCommandSuccessEnum::lampNotKnown);
		}
	}
	else
	{
		ConstructFailureMessage(hueMessage, hueReverseQueueMessage, HueCommandSuccessEnum::hubDown);
	}
	reverseHueMessageQueue.pushMessage(hueReverseQueueMessage);
}

void HueThread::processQueue()
{
    while (hueMessageQueue.getQueueSize() > 0)
    {
        const HueQueueMessage_t hue_queue_message = hueMessageQueue.popMessage();
        processMessage(hue_queue_message);
    }
}

void HueThread::connectToHub()
{
	hueConnection = new HueConnection();

	
    if (hueConnection->connectedToHub)
    {
        lights = hueConnection->hueBridge->lights().getAll();

        //provide mapping from Mac Address to light id.
        for (hueplusplus::HueLight light : lights)
        {
            macAddressLightNoMap.AddHueLight(light.getUId(), light.getId());
        }
    }
}

int calcSleepSeconds(int failedHubConnectAttempts)
{
	const int MaxDoublingPower = 6;

	//whilst retrying to connect to hub, double time delay each time to a maximum of 60 * 2**6 = 3840 seconds (just over an hour)
#ifdef _WIN32
	const int multiplier = 1 << min(failedHubConnectAttempts, MaxDoublingPower);
#else
	const int multiplier = 1 << std::min(failedHubConnectAttempts, MaxDoublingPower);
#endif

	const int sleepTime = 60 * multiplier;
	return sleepTime;
}

void HueThread::threadStart()
{
    logging.logDebug("threadstart for HueThread class started\n");

	int failedHubConnectAttempts = 0;
	while (!timeToFinish)
	{
		if (hueConnection == nullptr ||
			!hueConnection->connectedToHub)
		{
			logging.logDebug("Hue Thread attempt to connect to Hub count %d\n", failedHubConnectAttempts);
			
			connectToHub();
			
			if (hueConnection != nullptr && hueConnection->connectedToHub)
			{
				failedHubConnectAttempts = 0;
				logging.logDebug("Hue Thread now connected to Hue Hub count %d\n", failedHubConnectAttempts);
			}
			else
			{
				failedHubConnectAttempts++;
				logging.logDebug("Hue Thread failed to connect to Hue Hub count %d\n", failedHubConnectAttempts);
			}
		}

		if (hueConnection != nullptr && 
			hueConnection->connectedToHub && 
			hueMessageQueue.getQueueSize() > 0)
		{
			processQueue();
		}

		const time_t sleepSeconds = calcSleepSeconds(failedHubConnectAttempts);
		const time_t startTime = time(nullptr);
		const time_t endTime = startTime + sleepSeconds;
		while (time(nullptr) < endTime &&
			   hueMessageQueue.getQueueSize() == 0 &&
			   !timeToFinish)
		{
			CClock::SleepMilliseconds(1000); //wait 1 seconds
		}
    }
}

HueThread::~HueThread()
{
	if (hueConnection != nullptr)
	{
		free(hueConnection);
		hueConnection = nullptr;
	}
}
