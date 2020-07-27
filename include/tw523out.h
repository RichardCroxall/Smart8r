#pragma once
#include "runtime.h"

enum transmission_t
{
    transmission_not_acknowledged,
    transmission_acknowledged,
    transmission_different
};

class Ctw523Out
{
    private:
        x10_message_t x10_sent_message;
        bool x10_message_sent;

        int send_counter;
        msg_state_t send_state;
        bool first_time;
        transmission_t x10_transmission;

    protected:

    public:
        Ctw523Out();
        bool calc_Output_HalfBit(void);
        void SendMessage(x10_message_t  message);
        bool ClearToSendMessage();
        x10_message_t GetSentMessage();
        void noteMessageReceived(x10_message_t  messageReceived);
        transmission_t GetX10Transmission();
};
