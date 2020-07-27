#pragma once

typedef struct
{
    time_t mTimerFireTime;
    actionNo_t mFireActionNo;
} event_t;

typedef struct
{
    event_t* mFirstEvent;
    int mNoEventsInSequence;
    char mszName[MAXRUNTIMEIDENTIFIERLENGTH + 1];
    time_t mSequenceFireTime;
    unsigned int muDaysToFire;
} sequence_t;
// *************************************************************************

// *************************************************************************

// This class is the common ancestor of all timers
// It is an abstract class which should never be instanciated
class CTimer
{
    private:
        char mszName[MAXRUNTIMEIDENTIFIERLENGTH + 1];
        time_t mlFireTimeDate; // seconds since 00:00 on 1/1/1970 (to 2038)
        sequence_t* mFirstSequence;

        int mNextEventToAdd;
        int mNextEventToFire;

        int mNextSequenceToAdd;
        int mNextSequenceToFire;
        int mNoSequencesInTimer;
        time_t mSequenceFireTimeAtMidnight;

        void SetUpNextSequenceToFire();
        void completeDefinition();

    protected:
    public:
        CTimer(char *szName, int noSequencesInTimer)
        {
            strcpy(mszName, szName); 
            mlFireTimeDate = 0;
            mNoSequencesInTimer = noSequencesInTimer;
            mFirstSequence = new sequence_t[noSequencesInTimer];
            mNextSequenceToAdd = 0;

            mNextEventToAdd = 0;
            mNextEventToFire = 0;

            mNextSequenceToFire = -1;
            mSequenceFireTimeAtMidnight = -1;
        }

        //~CTimer() {};

        time_t getFireTimeDate() {return mlFireTimeDate;};
        char* getName() {return mszName;};
        char* getSequenceName() {assert(mNextSequenceToFire>=0); return mFirstSequence[mNextSequenceToFire].mszName;};
        int getNextEventToFire() {return mNextEventToFire;};
        int getNoEventsInSequence()
        {
            assert(mNextSequenceToFire>=0);
            return mFirstSequence[mNextSequenceToFire].mNoEventsInSequence;
        };


        void addSequenceToTimer(char* mszName, time_t mSequenceFireTime,
                                    unsigned int muDaysToFire, int noEventsInSequence)
        {
            assert(mNextSequenceToAdd < mNoSequencesInTimer);
            mNextEventToAdd = 0;
            mFirstSequence[mNextSequenceToAdd].mFirstEvent = new event_t[noEventsInSequence];
            mFirstSequence[mNextSequenceToAdd].mNoEventsInSequence = noEventsInSequence;
            strcpy(mFirstSequence[mNextSequenceToAdd].mszName, mszName);
            mFirstSequence[mNextSequenceToAdd].mSequenceFireTime = mSequenceFireTime;
            mFirstSequence[mNextSequenceToAdd].muDaysToFire = muDaysToFire;
            mNextSequenceToAdd++;
        }

        void addEventToSequence(time_t timerFireTime, actionNo_t fireActionNo)
        {
            assert(mNextSequenceToAdd <= mNoSequencesInTimer);

            // note mNextSequenceToAdd prematurely incremented in addSequenceToTimer
            mFirstSequence[mNextSequenceToAdd - 1].mFirstEvent[mNextEventToAdd].mTimerFireTime = timerFireTime;
            mFirstSequence[mNextSequenceToAdd - 1].mFirstEvent[mNextEventToAdd].mFireActionNo = fireActionNo;
            mNextEventToAdd++;
        }

#ifdef DEBUG_TIMER
        void dump(bool detailed = false);
#endif

        //void setFirstSequence(CSequence* firstSequence){mFirstSequence = firstSequence;};
        void fire(bool runtime);
        static void completeDefinitions();
};

// *************************************************************************

