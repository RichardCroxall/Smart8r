#pragma once

class Ctw523
{
    private:
        bool timeToFinish;
#ifndef _WIN32
        bool cycle_positive;
        bool previous_cycle_positive;
        bool x10_message_sent;

        int send_counter;
        msg_state_t send_state;
        bool first_time;
        nanoseconds_t samplingExpiryTime;
        nanoseconds_t zeroCrossingExpiryTime;
        bool GetSampleInputHalfBit();
        void mSendMessage();
        void mSendMessageWithImmediateRetry();

        // monitor state of zero crossing square wave
        void waitForZeroCrossing(void);

        void sleepToLetOtherProcessesIn();

        Ctw523In ctw523In;
        Ctw523Out ctw523Out;
#endif
        x10_message_t x10_sent_message;

		bool mSendQueueWithoutRetry();
        bool mFlushQueue(int retryCount);
        void waitForQuiet(unsigned long centiseconds);
        void do_half_cycle(void);

    protected:

    public:
         Ctw523();
        ~Ctw523();
        bool ok();

        void tw523ThreadStart(void);
        void FinishNow() { timeToFinish = true; }
        bool TimeToFinish() { return timeToFinish; }
};

#ifdef MAIN_PROGRAM
Ctw523 tw523;
#else
extern Ctw523 tw523;
#endif