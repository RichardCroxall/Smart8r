#include "include/runtime.h"

CClock::CClock()
{
    mDateTime = 0L;
    mCentiSeconds = 0;
    setTime(THE_END_OF_TIME);
    ConsistencyCheck();
};
//~CClock() {};
time_t CClock::getTime()
{
    ConsistencyCheck();
    return (mDateTime);
};
short CClock::getCentiSeconds()
{
    ConsistencyCheck();
    return mCentiSeconds;
}

void CClock::setTime(time_t newTime)
{
    ConsistencyCheck();
    mDateTime = newTime;
    mCentiSeconds = 0;
};


void CClock::tick()
{
    ConsistencyCheck();
    // zero crossings of the mains happen every 100th of a second
    // on average (instantanous variations can be between 4%,
    // but the long term average is maintained)
    // keep time in step
    mCentiSeconds = (mCentiSeconds + 1) % 100;
    if (mCentiSeconds == 0)
    {
        mDateTime++;
    }
}

void CClock::ResynchClockToHostRealTimeClock()
{
#if DEBUG
	logging.logWarn("ResynchClockToHostRealTimeClock\n");
#endif

#ifndef _WIN32 // do not synch the actual time on Windows simulation.

	//do not synchronise the clock to the present whilst simulating the past.
	if (!simulatingPast)
	{
		setTime(time(nullptr));
	}
#endif
}


void CClock::SleepMilliseconds(long milliseconds)
{
#ifdef _WIN32
    Sleep(milliseconds);
#else
    struct timespec ts;
    int res;

    if (milliseconds > 0)
    {
        ts.tv_sec = milliseconds / 1000;
        ts.tv_nsec = (milliseconds % 1000) * 1000000;

        do {
            res = nanosleep(&ts, &ts);
        } while (res != 0 && errno == EINTR);
    }
#endif
}
