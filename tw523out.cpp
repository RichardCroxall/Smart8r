#include "include/runtime.h"

#ifndef _WIN32
//***************************************************************************************************
bool Ctw523Out::calc_Output_HalfBit(void)
{
    bool send_half_bit = false;

    switch(send_state)
    {
        case msg_none:
            if (x10_sent_message == 0)
            {
                break;
            }
            else
            {
                send_state = msg_start;
                send_counter = 3; //X-10 messages starts '1110'. 
                first_time = true;

                /* DROP THROUGH !!! */
                }

        case msg_start: //X-10 messages starts '1110'. 
            if (send_counter > 0)
            {
                send_half_bit = true;
                send_counter--;
            }
            else
            {
                send_half_bit = false;
                send_state = msg_data; //'1110' preamble has been sent
                send_counter = X10_MESSAGE_LENGTH * 2;
            }
            break;

        case msg_data:
            {
                bool bit = (x10_sent_message & (1 << ((send_counter - 1) / 2)) ) != 0;

                if (send_counter % 2 == 0)
                    send_half_bit = bit;
                else
                    send_half_bit = !bit;
                send_counter--;

                if (send_counter == 0)
                {
                    if (first_time)
                    {
                        send_state = msg_start;
                        send_counter = 3; //X-10 messages starts '1110'. 
                        first_time = false; //each message is sent twice
                    }
                    else
                    {
                        send_state = msg_pause;
                        send_counter = 6; //send 3 periods (6 half bits) silence at end
                    }
                }
            }
            break;

        case msg_pause:
            send_half_bit = false;
            send_counter--;
            if (send_counter == 0)
            {
                send_state = msg_none;
                x10_message_sent = true; //send 3 periods (6 half bits) silence at end
                                                                first_time = true;

                                                                // ensure the previously sent internal messages are not confused
                                                                // with similar externally received messages 
                                                                x10_sent_message = 0;
            }
            break;

        default:
            assert(false);
            break;
    }
    return send_half_bit;
}
//***************************************************************************************************
void Ctw523Out::SendMessage(x10_message_t  message)
{
                assert(send_state == msg_none);
                assert(send_counter == 0);
                assert(x10_message_sent == true);
    assert(first_time == true);

                x10_transmission = transmission_not_acknowledged;
                x10_sent_message  = message;
}
//***************************************************************************************************
bool Ctw523Out::ClearToSendMessage()
{
                return send_state == msg_none && x10_sent_message == 0;
}
//***************************************************************************************************
x10_message_t Ctw523Out::GetSentMessage()
{
                return x10_sent_message;
}
//***************************************************************************************************
void Ctw523Out::noteMessageReceived(x10_message_t  messageReceived)
{
    if (messageReceived == x10_sent_message)
    {
        x10_transmission = transmission_acknowledged;
        //logging.logDebug("acknowledged\n");
    }
    else
    {
        //usually this will be the result of receiving an external message whilst we are not sending
        if (x10_sent_message != 0)
        {
            x10_transmission = transmission_different;
            logging.logDebug("different received %d sent %d\n", messageReceived, x10_sent_message);
        }
    }
}
//***************************************************************************************************
transmission_t Ctw523Out::GetX10Transmission()
{
                return x10_transmission;
}
//***************************************************************************************************

Ctw523Out::Ctw523Out(void)
{
    x10_sent_message = 0;
    x10_message_sent = true;
                x10_transmission = transmission_not_acknowledged;

    send_counter = 0;
    send_state = msg_none;
    first_time = true;
}
#endif