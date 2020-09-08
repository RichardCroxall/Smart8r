#include "../include/runtime.h"

/// <summary>
/// Simulate Reception of transmission from Raspberry Pi GPIO port.
/// </summary>

#if _WIN32
gpioReceive::gpioReceive()
{
    x10_received_message = 0;
    x10_message_received = false;

    receive_state = msg_none;
    receive_counter = 0;
    last_input_error = input_error_none;
    centiseconds_since_last_data_received = 0;

    receive_data = false;
    first_time = true;
    x10_first_received_message = 0;
}

x10_message_t gpioReceive::processReceivedHalfBit(bool receive_half_bit)
{
    x10_message_t resultX10Message = 0;
	
#if DEBUG22
    logging.logDebug("GPIO receive_half_bit=%d recieve_counter=%d receive_state=%d x10_received_message=%d\n",
        receive_half_bit, receive_counter, receive_state, x10_received_message);
#endif

#ifdef TEST3
        //if (receive_half_bit)
    {
        const intmax_t ticks = (std::chrono::steady_clock::now() - beginning_of_time).count();
        tickTimeStruct_t tickTimeStruct{ ticks / 1000, receive_half_bit };
        times.emplace_back(tickTimeStruct);
        if (times.size() > 300)
        {
            bool stop = true;
        }
    }
#endif


    receive_data = receive_half_bit;

    if (receive_half_bit)
    {
        centiseconds_since_last_data_received = 0;
    }
    else
    {
        centiseconds_since_last_data_received++;
    }

    receive_counter++;

    switch (receive_state)
    {
    case msg_none:
        receive_counter = 0;
        if (receive_half_bit) //silence broken by a 1 bit.
        {
            receive_state = msg_start; //possible '1110' start of X-10 message
            last_input_error = input_error_none;
            first_time = true;
        }
        break;

    case msg_start:
        //possible continuation of '1110' start of X-10 message
        if (!receive_half_bit)
        {
            //on 1st pass the initial one is not counted, so we have 2 ones and a zero -> 3.
        	//on 2nd pass we start here, so we count 3 ones and a zero -> 4.
            int counterTarget = first_time ? 3 : 4;
            if (receive_counter == counterTarget) //received zero after 3 ones?
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
                logging.logDebug("GPIO receive_counter not 3 = %d\n", receive_counter);
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
            if (((x10_received_message & 1) != 0) == receive_half_bit)
            {
                /* error two half bits received which were the same */
                receive_state = msg_error;
                last_input_error = input_error_data;
#if DEBUG
                logging.logDebug("GPIO bits do match but should not. x10_received_message %04x, receive_half_bit = %04x receive_counter = %d\n",
                    x10_received_message, receive_half_bit, receive_counter);
#endif
            }
        }

        if (receive_counter == X10_MESSAGE_LENGTH * 2)
        {
            receive_counter = 0;
            if (first_time)
            {
                receive_counter = 0;
                receive_state = msg_start; //second message also starts '1110'.
                first_time = false;
                x10_first_received_message = x10_received_message;
                resultX10Message = x10_received_message;
                x10_received_message = 0;
            }
            else
            {
                receive_state = msg_pause;
                assert(x10_received_message == x10_first_received_message);
            }
        }
        break;

    case msg_pause:
        if (receive_half_bit) //noise during pause?
        {
            /* error non zero received during pause */
            receive_state = msg_error;
            last_input_error = input_error_leadout;
#if DEBUG
            logging.logDebug("GPIO 1 during pause: receive_counter = %d\n", receive_counter);
#endif
        }
        else
        {
            // We check that we receive what we sent.
            //The receive check does not return with a value soon enough
            //unless we fiddle the silence down to 5 half bits.
            if (receive_counter >= 5) //wait for 3 periods (6 half bits) of silence
            {
                receive_state = msg_none;

                x10_message_received = true;
                gpio_devices->Message(x10_received_message);
#if DEBUG
                logging.logInfo("GPIO x10 message received %s\n", messageDescription(x10_received_message));
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
            if (receive_counter == 6) //wait for 3 periods (6 half bits) of silence before re-engaging
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

    return resultX10Message;
}


void gpioReceive::createDevice(deviceType_t deviceType, house_code_t houseCode, device_code_t deviceCode)
{
    gpio_devices->createDevice(deviceType, houseCode, deviceCode);
}

#endif
