#include "include/runtime.h"

CCalendar::CCalendar()
{
    mMidnightStartDay = THE_END_OF_TIME;
};

void CCalendar::setStartDate(time_t midnightStartDay)
{
    mMidnightStartDay = midnightStartDay;
};

time_t CCalendar::getStartDate()
{
    return mMidnightStartDay;
}

//~CCalendar() {};

unsigned int CCalendar::getDayAttribute(time_t day)
{
    const int index = (int) ((day - mMidnightStartDay)/ SECONDSPERDAY);
    assert(index >= 0 && index < TOTAL_DAY_ATTRIBUTE_DAYS);
    return muDayAttributes[index];
};


time_t CCalendar::calculateScheduledEventTime(time_t midnightDateTime,
                time_t sequenceTime,
                time_t eventTimeOffset)
{
    const int index = (int) ((midnightDateTime - mMidnightStartDay)/ SECONDSPERDAY);
    time_t adjustment = 0;

#ifdef DEBUG_CALENDAR
    logging.logDebug("mMidnightStartDay %ld midnightDateTime %ld indexThread %d\n",
                                    mMidnightStartDay, midnightDateTime, indexThread);
#endif
    assert(midnightDateTime % SECONDSPERDAY == 0);
    assert(index >= 0);
    assert(index < TOTAL_DAY_ATTRIBUTE_DAYS);

    if (sequenceTime == SUNRISE_TIME)
    {
        sequenceTime = mtSunRise[index];
    }
    else if (sequenceTime == SUNSET_TIME)
    {
        sequenceTime = mtSunSet[index];
    }
    else if ((muDayAttributes[index] & DAY_BST) != 0)
    {
        // Sunrise & Sunset are already in GMT and hence do not need adjustment

        // This adjustment does not work correctly for first day of BST,
        // and the first day of GMT between midnight, 1 & 2 in the morning.
        adjustment = -3600;
    }
    assert(sequenceTime > 0 && sequenceTime < SECONDSPERDAY);
    assert(eventTimeOffset > -SECONDSPERDAY && eventTimeOffset < SECONDSPERDAY);


    return midnightDateTime + sequenceTime + eventTimeOffset + adjustment;
}

#ifdef DEBUG_CALENDAR
time_t timeOfDay(time_t t)
{
    time_t seconds;
    time_t minutes;
    time_t hours;
    assert(t < SECONDSPERDAY);

    hours = t / 3600;
    minutes = (t / 60) % 60;
    seconds = t % 60;

    return hours * 10000L + minutes * 100L + seconds;
}
#endif
void CCalendar::setDayAttribute(time_t day, unsigned int attribute, time_t sunRise, time_t sunSet)
{
    const int index = (int) ((day - mMidnightStartDay)/ SECONDSPERDAY);

#ifdef DEBUG_CALENDAR
    logging.logDebug("setDayAttribute mMidnightStartDay %ld day = %ld indexThread = %d %03x rise %06ld set %06ld\n",
        mMidnightStartDay, day, indexThread, attribute, timeOfDay(sunRise), timeOfDay(sunSet));
#endif
    if (index >= 0 && index < TOTAL_DAY_ATTRIBUTE_DAYS)
    {
        muDayAttributes[index] |= (attribute);
        mtSunRise[index] = sunRise;
        mtSunSet[index] = sunSet;
    }
    else
    {
         std::cout << "indexThread = " << index << "\n";
    }
    //assert (indexThread >= 0 && indexThread < TOTAL_DAY_ATTRIBUTE_DAYS);
}
