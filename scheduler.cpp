#include "include/runtime.h"

CScheduler::CScheduler()
{
    miNoTimers = 0;
}

static int compareTimer(const void* first, const void* second)
{
    int iRetVal = 0;

    const long lTimeDiff =  ((*(CTimer**)first)->getFireTimeDate() -
            (*(CTimer**)second)->getFireTimeDate());
    if (lTimeDiff < 0)
    {
        iRetVal =  -1;
    }
    if (lTimeDiff > 0)
    {
        iRetVal =  +1;
    }

#if DEBUG9
    logging.logInfo("compare %2d %s %s\n", iRetVal,
    (*(CTimer**)first)->getName(), (*(CTimer**)second)->getName());
#endif

    return iRetVal;
}

void CScheduler::addTimer(CTimer* pTimer)
{
    // add new timer to the end of the list and sort
    assert(miNoTimers < MAX_TIMERS);
    mTimers[miNoTimers++] = pTimer;

    rescheduleTimer();
}

void CScheduler::rescheduleTimer()
{
    // A timer has fired and reset its time to the next time it needs to fire
    // or a timer has just altered its next firetime
    // or a timer has been added.
    // Sort all timers into date order

#if DEBUG
    logging.logInfo("sorting %d timers\n", miNoTimers);
#endif
     qsort(mTimers, miNoTimers, sizeof(mTimers[0]), compareTimer);
#if DEBUG2
    listTimers();
#endif
}

void CScheduler::listTimers()
{
#if DEBUG
    logging.logInfo("listing %d timers\n", miNoTimers);
#endif
    for (int i = 0; i < miNoTimers; i++)
    {
#if DEBUG
        logging.logInfo("listing timer %d\n", i);
#endif

    time_t fireTimeDate;

    fireTimeDate = mTimers[i]->getFireTimeDate();
#if DEBUG
    logging.logInfo("fireTimeDate %ld\n", fireTimeDate);
    if (fireTimeDate > 0)
    {
        struct tm* localFireTimeDate = gmtime(&fireTimeDate);
        logging.logDebug("timer [%d] = %02d/%02d/%04d %02d:%02d:%02d GMT %d tmr %s seq %s ev %d of %d\n",
            i,
            localFireTimeDate->tm_mday,
            localFireTimeDate->tm_mon + 1,
            localFireTimeDate->tm_year + 1900,
            localFireTimeDate->tm_hour,
            localFireTimeDate->tm_min,
            localFireTimeDate->tm_sec,
            localFireTimeDate->tm_isdst,
             mTimers[i]->getName(),
             mTimers[i]->getSequenceName(),
             mTimers[i]->getNextEventToFire(),
             mTimers[i]->getNoEventsInSequence()
             );
         }
#endif
     }
}
