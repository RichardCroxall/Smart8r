#pragma once

#ifndef THE_END_OF_TIME
#define THE_END_OF_TIME ((time_t) 0x7F000000)
#endif

class CClock
{
    private:
        time_t mDateTime;
        int mCentiSeconds;

        void ConsistencyCheck()
        {
            if (mCentiSeconds < 0)
            {
                printf("failed 1 %d\n", mCentiSeconds);
            }
            if (mCentiSeconds >= 100)
            {
                printf("failed 2 %d\n", mCentiSeconds);
            }
            assert(mCentiSeconds >= 0 && mCentiSeconds < 100);
        }

    public:
        CClock();
        //~CClock() {};
        time_t getTime();
        short getCentiSeconds();
        void setTime(time_t newTime);
        void ResynchClockToHostRealTimeClock();
        void static SleepMilliseconds(long milliseconds);
        void tick();
};

#ifdef MAIN_PROGRAM
       CClock tickingClock;
#else
extern CClock tickingClock;
#endif
