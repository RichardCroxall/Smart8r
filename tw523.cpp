
#include "include/runtime.h"

#ifndef _WIN32
#include <pthread.h>
#endif


extern CX10MessageQueue x10MessageQueue;
extern CX10House house;
extern rqueue reverseMessageQueue;


#ifndef _WIN32
//***************************************************************************************************
void initialisePorts()
{
    const int TargetPriority = -20; //negative is the highest priority. Positive value is the lowest.

    GPIOexport(X10ZeroCrossingPort, X10ReceivePort);
    GPIOdirection(X10ZeroCrossingPort, X10ZeroCrossingPort, false);
    GPIOdirection(X10TransmitPort, X10TransmitPort, true);
    GPIOdirection(X10ReceivePort, X10ReceivePort, false);

    GPIOOpenValue(X10ZeroCrossingPort, X10ZeroCrossingPort, false);
    GPIOOpenValue(X10TransmitPort, X10TransmitPort, true);
    GPIOOpenValue(X10ReceivePort, X10ReceivePort, false);

    int newpriority = nice(TargetPriority);
    if (newpriority != TargetPriority)
    {
        logging.logError("nice newpriority = %d Errno = %d\n", newpriority, errno);
    }
}

//***************************************************************************************************
void finalisePorts()
{
    GPIOCloseValue(0, 7);
    GPIOunexport(0, 7);
}

//***************************************************************************************************
nanoseconds_t nanoSecondsSinceEpoch()
{
    timespec realTime;
    clock_gettime(CLOCK_REALTIME, &realTime);

    nanoseconds_t ns = 1000000000LL * realTime.tv_sec +  realTime.tv_nsec;
    return ns;
}

//***************************************************************************************************
bool mainsCyclePositive(void)
{
    return !GPIOGet(X10ZeroCrossingPort);
};

//***************************************************************************************************
bool sampleReceiveHalfBit(void)
{
    return !GPIOGet(X10ReceivePort);
};

//***************************************************************************************************
void outputSendHalfBit(bool send_half_bit)
{
    GPIOSet(X10TransmitPort, send_half_bit);
};

//***************************************************************************************************
void Ctw523::waitForZeroCrossing(void)
{
    long waitCount = 0;
    bool previous_previous_cycle_positive = previous_cycle_positive;

    previous_cycle_positive = mainsCyclePositive();

#if DEBUG
    if (previous_previous_cycle_positive != previous_cycle_positive)
    {
        logging.logInfo("lost zero crossing sync previous %d now %d\n", previous_previous_cycle_positive, previous_cycle_positive);
    }
#endif

    // loop until the cycle switches from positive to negative or vice-versa
    do
    {
        waitCount++;
        cycle_positive = mainsCyclePositive();
    } while (previous_cycle_positive == cycle_positive);

#if DEBUG
    if (waitCount <= 1)
    {
        logging.logWarn("waitForZeroCrossing waitCount = %ld %d\n", waitCount, cycle_positive);
    }
#endif

    previous_cycle_positive = cycle_positive;

    // zero crossings of the mains happen every 100th of a second
    // on average (instantaneous variations can be between 4%,
    // but the long term average is maintained
    // keep time in step
    tickingClock.tick();
};


//***************************************************************************************************
bool Ctw523::GetSampleInputHalfBit()
{
    //wait for receive bit to go high at any time during the 1ms
    nanoseconds_t now = nanoSecondsSinceEpoch();

    samplingExpiryTime = now + 1000LL * 1000; //1ms

    //9.6ms is lowest delay for a half cycle when the frequency is 50Hz - 4%.
    zeroCrossingExpiryTime = now + 1000LL * 9600;

    bool receive_half_bit = false;
    int samples = 0;
    nanoseconds_t previousNow = now;
    while (now < samplingExpiryTime)
    {
        receive_half_bit |= sampleReceiveHalfBit();
        previousNow = now;
        now = nanoSecondsSinceEpoch();
        samples++;
    }

#if DEBUG2
    nanoseconds_t delay = (now - samplingExpiryTime)/1000; //in microseconds
    if (delay > 300)
    {
        logging.logDebug("delay %lld us samples %d final gap %lld us\n", delay, samples, (samplingExpiryTime - previousNow) / 1000);
    }
#endif
    return receive_half_bit;
}
#endif
//***************************************************************************************************
void Ctw523::do_half_cycle(void)
{
#ifndef _WIN32
    bool send_half_bit = ctw523Out.calc_Output_HalfBit();

    waitForZeroCrossing();
    outputSendHalfBit(send_half_bit); //send after zero crossing for 1ms.
    bool receive_half_bit = GetSampleInputHalfBit(); //sample input for 1ms
    outputSendHalfBit(false); //stop sending 1ms after zero crossing

    ctw523In.processReceivedHalfBit(receive_half_bit);

    if (ctw523In.X10MessageReceived())
    {
        x10_message_t x10_received_message = ctw523In.GetX10ReceivedMessage();
        ctw523Out.noteMessageReceived(x10_received_message);

        bool external = x10_received_message != x10_sent_message;

#if DEBUG2
        if (external)
        {
            logging.logDebug("x10 message received %s\n", messageDescription(x10_received_message));
        }
#endif
        X10_io_message_t x10_received_io_message = { external, x10_received_message };
        reverseMessageQueue.pushMessage(x10_received_io_message);
    }
    sleepToLetOtherProcessesIn();
#endif
}


//***************************************************************************************************
bool Ctw523::ok()
{
#ifndef _WIN32
    /* note current state of mains cycle */
    previous_cycle_positive = mainsCyclePositive();

    /* check for active && plugged in TW523 by monitoring zero crossings
     * no zero crossings = no active TW523
     */

    nanoseconds_t now = nanoSecondsSinceEpoch();
    nanoseconds_t expiryTime = now + 250000000;
    int crossings = 0;

    while (nanoSecondsSinceEpoch() < expiryTime)
    {
         cycle_positive = mainsCyclePositive();
         if (previous_cycle_positive != cycle_positive)
         {
            crossings++;
         }
         previous_cycle_positive = cycle_positive;
    }

#if DEBUG
    logging.logDebug("crossings = %d\n", crossings);
#endif

    return (crossings >= 23 && crossings <= 28);
#else
    return true;
#endif
}
//***************************************************************************************************

Ctw523::Ctw523(void)
{
    timeToFinish = false;
    x10_sent_message = 0;
#ifndef _WIN32
    x10_message_sent = true;

    send_counter = 0;
    send_state = msg_none;
    first_time = true;

    initialisePorts();

    cycle_positive = mainsCyclePositive();
    previous_cycle_positive = cycle_positive;
#endif
}

Ctw523::~Ctw523(void)
{
#ifndef _WIN32
    finalisePorts();
#endif
}

void Ctw523::tw523ThreadStart()
{
    logging.logDebug("threadstart for tw523 class started\n");

    while (!timeToFinish)
    {
        if (x10MessageQueue.getQueueSize() > 0)
        {
            // ensure other thread does not mess whilst we are filling the queue.
            std::lock_guard<std::mutex> guard(x10MessageQueue.mQueueMutex);

            const bool success = mFlushQueue(1);
            assert(success);

            x10MessageQueue.clearQueue(); //hope that we have cleared all IR commands out.

        }
    	
        do_half_cycle();
    }
}
//***************************************************************************************************
#ifndef _WIN32
void Ctw523:: sleepToLetOtherProcessesIn()
{

    nanoseconds_t now = nanoSecondsSinceEpoch();
    long long nanoSecondsDelayNeededFromNow = zeroCrossingExpiryTime - now;

    if (nanoSecondsDelayNeededFromNow > 0 && nanoSecondsDelayNeededFromNow < 9000000)
    {
        timespec timespecDelay = {0, 8000000};
        timespec remainingTimespecDelay = {0, 0};
        timespecDelay.tv_nsec = (long) nanoSecondsDelayNeededFromNow;

        int result = nanosleep (&timespecDelay, &remainingTimespecDelay);
        while  (result < 0 && errno == EINTR)
        {
            logging.logDebug("nanosleep suspended by Linux\n");
            timespecDelay = remainingTimespecDelay;
            result = nanosleep (&timespecDelay, &remainingTimespecDelay);
        }
    }
    else
    {
        logging.logDebug("delay out of range %lld\n", nanoSecondsDelayNeededFromNow);
    }

}
//***************************************************************************************************
void Ctw523::mSendMessage()
{
    ctw523Out.SendMessage(x10_sent_message);

#if DEBUG44
    logging.logDebug("message sent %s\n", messageDescription(x10_sent_message));
#endif

    /* avoid clash with received message by waiting for no messages to be in progress */
    while (!ctw523Out.ClearToSendMessage() || !ctw523In.isReadyToReceive())
    {
        do_half_cycle();
    }
}
//***************************************************************************************************
void Ctw523::mSendMessageWithImmediateRetry()
{
    //where there are DIM or BRIGHT commands, it is not safe to resend individual commands
    //(or you might get the wrong number of DIMs/BRIGHTs.
    //Where another transmitter might select an X-10 device, it is not safe to resend individual commands
    //as the other selection will probably not be what you wanted.

    int immediateRetryCount = 0;

    do
    {
        if (immediateRetryCount  > 0)
        {
            logging.logDebug("message %s not acknowledged, retrying individual command %d\n", messageDescription(x10_sent_message), ctw523Out.GetX10Transmission());
            waitForQuiet(20 + 20 * immediateRetryCount);
        }

        mSendMessage();
        immediateRetryCount++;

    } while (immediateRetryCount < 10 &&
             ctw523Out.GetX10Transmission() != transmission_acknowledged &&
           !x10MessageQueue.getHouseCodeHasOtherTransmitters() &&
           !x10MessageQueue.getQueueContainsDimOrBright() );
}
//***************************************************************************************************
#endif
bool Ctw523::mSendQueueWithoutRetry()
{
    bool success = true; //until we know otherwise

    while(x10MessageQueue.getQueueSize() > 0 && success)
    {
        x10_sent_message = x10MessageQueue.nextMessage();
#ifndef _WIN32
        mSendMessageWithImmediateRetry();

        if (ctw523Out.GetX10Transmission() != transmission_acknowledged)
        {
            success  = false;
        }
#else
#ifdef DEBUG
        logging.logDebug("X10_sent_message=%s\n", messageDescription(x10_sent_message));
#endif
            house.mMessage(x10_sent_message, false); //for Windows debugging, simulate reception of message sent
#endif
    }
    return success;
}
//***************************************************************************************************
bool Ctw523::mFlushQueue(int retryCount)
{
    //try to send complete message
    //if there is an issue, retry to send complete message, but with a longer delay each time around.

    int completeMessageRetry = 0;
    bool success = false;

    do
    {
#ifndef _WIN32
        waitForQuiet(20 + 20 * (completeMessageRetry + retryCount));
        if (completeMessageRetry  > 0)
        {
            logging.logDebug("message %s not acknowledged, restarting queue tranmission %d\n", messageDescription(x10_sent_message), ctw523Out.GetX10Transmission());
            x10MessageQueue.restartQueue();
        }
#endif
        success =  mSendQueueWithoutRetry();
        completeMessageRetry++;

    } while (!success &&
             completeMessageRetry < 10 &&
           !x10MessageQueue.getQueueContainsDimOrBright() );

    x10_sent_message = 0;

    return success;
}
//***************************************************************************************************
void Ctw523::waitForQuiet(unsigned long centiseconds)
{
#ifndef _WIN32
#if DEBUG
    if (!ctw523In.isQuietFor(centiseconds))
    {
        logging.logInfo("waiting for quiet\n");
    }
#endif

    while (!ctw523In.isQuietFor(centiseconds))
    {
        do_half_cycle();
    }

#if DEBUG
    logging.logInfo("quiet restarting queue\n");
#endif
#endif
}
