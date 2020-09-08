#pragma once
class gpioTransmit
{
private:
    x10_message_t x10_sent_message;
    bool x10_message_sent;

    int send_counter;
    msg_state_t send_state;
    transmission_t x10_transmission;

protected:
public:
	gpioTransmit();
	void setTransmitX10Message(x10_message_t x10_message);
	bool receiveHalfBit(bool firstSample);
};

