#include "include/runtime.h"

void CTimer::fire(bool runtime)
{
#ifdef DEBUG_TIMER
    logging.logInfo("fired %s\n", mszName);
#endif
    assert(mNextSequenceToFire >=  0);
    assert(mNextEventToFire >= 0);

    // fire event action
    loadMap.action[mFirstSequence[mNextSequenceToFire].
            mFirstEvent[mNextEventToFire].mFireActionNo]->fire(runtime);
    // advance to the next event
    mNextEventToFire++;

    // last event?
    if (mNextEventToFire < mFirstSequence[mNextSequenceToFire].mNoEventsInSequence)
    {
        // set up time for next event
        mlFireTimeDate = calendar.calculateScheduledEventTime(mSequenceFireTimeAtMidnight,
            mFirstSequence[mNextSequenceToFire].mSequenceFireTime,
            mFirstSequence[mNextSequenceToFire].mFirstEvent[mNextEventToFire].mTimerFireTime);
    }
    else
    {
        SetUpNextSequenceToFire();
    }
}



void CTimer::SetUpNextSequenceToFire()
{
    time_t dateTime = tickingClock.getTime();
    time_t timeAtMidnight = (dateTime / SECONDSPERDAY) * SECONDSPERDAY;
    int earliest_sequence = -1;
    time_t earliestSequenceFirstEventFireTime = THE_END_OF_TIME;

    while (earliest_sequence == -1)
    {
        timeAtMidnight = (dateTime / SECONDSPERDAY) * SECONDSPERDAY;
#ifdef DEBUG_TIMER
        time_t timeOfDay;
        timeOfDay = dateTime % SECONDSPERDAY;
        logging.logInfo("SetUpNextSequenceToFire timeAtMidnight = %ld timeOfDay = %ld mNoSequencesInTimer %d\n",
            timeAtMidnight, timeOfDay, mNoSequencesInTimer);
#endif
    	int sequence = 0;

        while (sequence < mNoSequencesInTimer)
        {
            // calculate when the first (zeroeth) event in this sequence will fall
            time_t sequenceFirstEventFireTime = calendar.calculateScheduledEventTime(timeAtMidnight,
                mFirstSequence[sequence].mSequenceFireTime,
                mFirstSequence[sequence].mFirstEvent[0].mTimerFireTime);

            // if this sequence fires on a day like today and
            // this sequence fires in the future and
            // this sequence is earlier than any previous event we have, note it down.
            if ((mFirstSequence[sequence].muDaysToFire & calendar.getDayAttribute(timeAtMidnight))!= 0 &&
                sequenceFirstEventFireTime > tickingClock.getTime() &&
                sequenceFirstEventFireTime < earliestSequenceFirstEventFireTime)
            {
                earliest_sequence = sequence;
                earliestSequenceFirstEventFireTime = sequenceFirstEventFireTime;
            }
            sequence++;
        }
        // in case no sequence will fire today, advance to tomorrow
        dateTime = timeAtMidnight + SECONDSPERDAY;
    }

    mNextEventToFire = 0;
    mSequenceFireTimeAtMidnight = timeAtMidnight;
    mlFireTimeDate = earliestSequenceFirstEventFireTime;
    mNextSequenceToFire = earliest_sequence;

#ifdef DEBUG_TIMER
    logging.logDebug("firing sequence %s\n", mFirstSequence[earliest_sequence].mszName);
#endif
}


void CTimer::completeDefinition()
{
#ifdef DEBUG_TIMER
    logging.logDebug("completedefinition timer %s\n", mszName);
#endif
    SetUpNextSequenceToFire();
};

void CTimer::completeDefinitions()
{
    for (int i = 0; i < codeHeaderRecord.noTimerEntries; i++)
    {
        loadMap.timer[i]->completeDefinition();
    }
}


#ifdef DEBUG_TIMER
void CTimer::dump(bool detailed)
{
    logging.logInfo("mNoSequencesInTimer = %d mNextSequenceToFire %d\n",
        mNoSequencesInTimer, mNextSequenceToFire);

    if (detailed)
    {
        for (int sequenceNo = 0; sequenceNo < mNoSequencesInTimer; sequenceNo++)
        {
            printf("sequence %d %s firetime %ld firedays %x mNoEventsInSequence %d\n",
                sequenceNo,
                mFirstSequence[sequenceNo].mszName,
                mFirstSequence[sequenceNo].mSequenceFireTime,
                mFirstSequence[sequenceNo].muDaysToFire,
                mFirstSequence[sequenceNo].mNoEventsInSequence);

            for (int eventNo = 0; eventNo < mFirstSequence[sequenceNo].mNoEventsInSequence; eventNo++)
            {
                printf("event %d firetime %ld action No. %d\n",
                    eventNo,
                    mFirstSequence[sequenceNo].mFirstEvent[eventNo].mTimerFireTime,
                    mFirstSequence[sequenceNo].mFirstEvent[eventNo].mFireActionNo);
            }
        }
    }
}
#endif

