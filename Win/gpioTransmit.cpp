#include "../include/runtime.h"


gpioTransmit::gpioTransmit()
{
    x10_sent_message = 0;
    x10_message_sent = true;
    x10_transmission = transmission_not_acknowledged;

    send_counter = 0;
    send_state = msg_none;
}

void gpioTransmit::setTransmitX10Message(x10_message_t x10_message)
{
    assert(x10_sent_message == 0);
	
    x10_sent_message = x10_message;
}

bool gpioTransmit::receiveHalfBit(bool firstSample)
{
    bool send_half_bit = false;

    if (firstSample)
    {
        switch (send_state)
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
            bool bit = (x10_sent_message & (1 << ((send_counter - 1) / 2))) != 0;

            if (send_counter % 2 == 0)
                send_half_bit = bit;
            else
                send_half_bit = !bit;
            send_counter--;

            if (send_counter == 0)
            {
                send_state = msg_pause;
                send_counter = 6; //send 3 periods (6 half bits) silence at end
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

                // ensure the previously sent internal messages are not confused
                // with similar externally received messages 
                x10_sent_message = 0;
            }
            break;

        default:
            assert(false);
            break;
        }
    }
    return send_half_bit;

}



