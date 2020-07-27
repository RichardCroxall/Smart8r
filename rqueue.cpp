#include "include/runtime.h"

void rqueue::pushMessage(X10_io_message_t message)
{
	std::lock_guard<std::mutex> guard(reverseQueueMutex);
	reverseQueue.push(message);
	fastQueueSize++;
}

int rqueue::queueSize()
{
	//std::lock_guard<std::mutex> guard(reverseQueueMutex);

	//return reverseQueue.size();
	return fastQueueSize;
}

X10_io_message_t rqueue::popMessage()
{
	std::lock_guard<std::mutex> guard(reverseQueueMutex);

	fastQueueSize--;
	assert(fastQueueSize >= 0);
	const X10_io_message_t result = reverseQueue.front();
	reverseQueue.pop();

	return result;
}
