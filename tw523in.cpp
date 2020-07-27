#include "include/runtime.h"

#ifndef _WIN32
//***************************************************************************************************
void Ctw523In::processReceivedHalfBit(bool receive_half_bit)
{
#if DEBUG22
    logging.logDebug("receive_half_bit=%d recieve_counter=%d receive_state=%d x10_received_message=%d\n",
    receive_half_bit, receive_counter, receive_state, x10_received_message);
#endif

    if (receive_half_bit)
    {
        centiseconds_since_last_data_received = 0;
    }
    else
    {
        centiseconds_since_last_data_received++;
    }

    receive_counter++;

    switch(receive_state)
    {
        case msg_none:
            receive_counter = 0;
            if (receive_half_bit) //silence broken by a 1 bit.
            {
                receive_state = msg_start; //possible '1110' start of X-10 message
                last_input_error = input_error_none;
            }
            break;

        case msg_start:
            //possible continuation of '1110' start of X-10 message
            if (!receive_half_bit)
            {
                if (receive_counter == 3) //received zero after 3 ones?
                {
                    receive_state = msg_data;
                    receive_counter = 0;

                    x10_message_received = false;
                    x10_received_message = 0;
                }
                else
                {
                    receive_state = msg_error;
                    last_input_error = input_error_leadin;
#if DEBUG
                    logging.logDebug("receive_counter not 3 = %d\n", receive_counter);
#endif
                }
            }
            break;

        case msg_data:
            if (receive_counter % 2 == 1)
            {
                x10_received_message = (x10_received_message << 1) | (receive_half_bit ? 1 : 0);
            }
            else
            {
                if (((x10_received_message  & 1) != 0) == receive_half_bit)
                {
                    /* error two half bits received which were the same */
                    receive_state = msg_error;
                    last_input_error = input_error_data;
#if DEBUG
                    logging.logDebug("bits do match but should not. x10_received_message %04x, receive_half_bit = %04x receive_counter = %d\n",
                                    x10_received_message, receive_half_bit, receive_counter);
#endif
                }
            }

            if (receive_counter == X10_MESSAGE_LENGTH * 2)
            {
                receive_state = msg_pause;
                receive_counter = 0;
            }
            break;

        case msg_pause:
            if (receive_half_bit) //noise during pause?
            {
                /* error non zero received during pause */
                receive_state = msg_error;
                last_input_error = input_error_leadout;
#if DEBUG
                logging.logDebug("1 during pause: receive_counter = %d\n", receive_counter);
#endif
            }
            else
            {
                // We check that we receive what we sent.
                //The receive check does not return with a value soon enough
                //unless we fiddle the silence down to 5 half bits.
                if (receive_counter >= 5 ) //wait for 3 periods (6 half bits) of silence
                {
                    receive_state = msg_none;

                    x10_message_received = true;
#if DEBUG
                    logging.logInfo("x10 message received %s\n", messageDescription(x10_received_message));
#endif
                }
            }
            break;

        case msg_error:
        {
                receive_state = msg_suppress;
                receive_counter = 0;
        }
        /* DROP THROUGH */

        case msg_suppress:
        {
            if (receive_half_bit) //noise during pause?
            {
                receive_counter = 0;
            }
            else
            {
                if (receive_counter == 6 ) //wait for 3 periods (6 half bits) of silence before re-engaging
                {
                    receive_state = msg_none;
                }
            }
        }
        break;


        default:
            receive_state = msg_none;
            assert(false);
            break;
    }
}

//***************************************************************************************************
bool Ctw523In::X10MessageReceived()
{
    return x10_message_received;
}
//***************************************************************************************************
x10_message_t Ctw523In::GetX10ReceivedMessage()
{
    assert(x10_message_received);

    x10_message_received = false;
    return x10_received_message;
}
//***************************************************************************************************
bool Ctw523In::isReadyToReceive()
{
    return receive_state == msg_none;
}
//***************************************************************************************************
//time_t Ctw523In::GetLastTimeDataReceived()
//{
    //return last_time_data_received;
//}
//***************************************************************************************************
bool Ctw523In::isQuietFor(unsigned long centiseconds = 10)
{
#if DEBUG22
    logging.logDebug("centiseconds_since_last_data_received  %d\n", centiseconds_since_last_data_received);
#endif

    bool quiet = (receive_state == msg_none && centiseconds_since_last_data_received > centiseconds);
    return quiet;
}
//***************************************************************************************************
input_error_enum Ctw523In::GetLastInputError()
{
    return last_input_error;
}
//***************************************************************************************************
Ctw523In::Ctw523In(void)
{
    x10_received_message = 0;
    x10_message_received = false;

    receive_state = msg_none;
    receive_counter = 0;
    last_input_error = input_error_none;
    centiseconds_since_last_data_received  = 0;
}

#endif