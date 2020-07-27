//test TW523 threading

#define MAIN_PROGRAM 1

#include <thread>

#include "runtime.h"

CX10MessageQueue messageQueue;
//CX10House house("Barn End");
Ctw523 tw523;

void tw523ThreadStart()
{
	tw523.tw523ThreadStart();
}

int main(int argc, char* argv[])
{
	std::cout << "Hello world\n";

	
	//initialise
	if (!tw523.ok())
	{
		//logging.logError("TW523 not connected to mains!\n");
		std::cout << "Tw523 failed\n";
	}
	else
	
	{
		std::thread tw523thread(tw523ThreadStart);
		std::cout << "Thread Started\n";

		for (int i = 0; i < 10; i++)
		{
			{
				// ensure other thread does not mess whilst we are filling the queue.
				std::lock_guard<std::mutex> guard(messageQueue.mQueueMutex);

				std::cout << "on " << i << "\n";
				messageQueue.addQueue(HouseC, UNIT1);
				messageQueue.addQueue(HouseC, funOn);
			}

			std::this_thread::sleep_for(std::chrono::seconds(3));

			{
				// ensure other thread does not mess whilst we are filling the queue.
				std::lock_guard<std::mutex> guard(messageQueue.mQueueMutex);
				std::cout << "off " << i << "\n";
				messageQueue.addQueue(HouseC, UNIT1);
				messageQueue.addQueue(HouseC, funOff);
			}

			std::this_thread::sleep_for(std::chrono::seconds(3));
		}

		tw523.FinishNow();

		std::cout << "Join\n";
		tw523thread.join();

	}
	
	std::cout << "Goodbye cruel world\n";
	return 0;
}
