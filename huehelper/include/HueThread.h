#pragma once
class HueThread
{
	friend class HueConnection;
private:
	std::vector<std::reference_wrapper<hueplusplus::HueLight>> lights;


public: /* TODO should be private or protected: */
	bool timeToFinish = false;
	HueConnection* hueConnection = nullptr;
public:
	HueThread();
	~HueThread();
	
	void ConstructFailureMessage(HueQueueMessage_t hueMessage, HueReverseQueueMessageSuccess_t hueReverseQueueMessage, HueCommandSuccessEnum hue_command_success);
	
	void processMessage(const HueQueueMessage_t& hueMesssage);
	void processQueue();
	void connectToHub();
	void threadStart();
	void FinishNow() { timeToFinish = true; }
	bool TimeToFinish() { return timeToFinish; }
};

