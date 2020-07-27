#pragma once
#define MAX_TIMERS 20
class CScheduler 
{
    private:
        int miNoTimers;
        CTimer* mTimers[MAX_TIMERS];

    protected:
    public:
        CScheduler();
        //~CScheduler() {};
        void addTimer(CTimer* pTimer);
        void rescheduleTimer();
        void listTimers();
        bool timerReady()
        {
             assert(miNoTimers > 0);
             return (tickingClock.getTime() >= mTimers[0]->getFireTimeDate());
        };
        time_t nextTimerFireTime() { return mTimers[0]->getFireTimeDate();};
        void fire(bool runtime)    { mTimers[0]->fire(runtime); }
};

#if MAIN_PROGRAM
       CScheduler scheduler;
#else
extern CScheduler scheduler;
#endif

