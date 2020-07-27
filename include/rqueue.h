#pragma once

typedef struct 
{
	bool external;
	x10_message_t x10_message;
} X10_io_message_t;

const int MessageQueueSize = 3;

class rqueue
{
private:
	
	//queue of received message from second thread back to first thread
	std::queue<X10_io_message_t> reverseQueue;
	std::mutex reverseQueueMutex;
	int fastQueueSize = 0;
	
protected:
	
public:
	void pushMessage(X10_io_message_t message);
	int queueSize();
	X10_io_message_t popMessage();
};


#ifdef MAIN_PROGRAM
rqueue reverseMessageQueue;
#else
extern rqueue reverseMessageQueue;
#endif
