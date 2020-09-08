#pragma once

enum input_error_enum
{
    input_error_none,
    input_error_leadin,
    input_error_data,
    input_error_leadout,
};


class Ctw523In
{
    private:
        x10_message_t x10_received_message;
        bool x10_message_received;

        int receive_counter;
        msg_state_t receive_state;
        unsigned long centiseconds_since_last_data_received;
        input_error_enum last_input_error;
	
    protected:

    public:
        Ctw523In();
        bool isQuietFor(unsigned long centiseconds);
        void processReceivedHalfBit(bool receive_half_bit);
        bool X10MessageReceived();
        x10_message_t GetX10ReceivedMessage();
        input_error_enum GetLastInputError();
        bool isQuietFor(int centiseconds);
        bool isReadyToReceive();
};
