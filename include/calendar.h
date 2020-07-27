#pragma once
class CCalendar
{
    private:
        time_t mMidnightStartDay;
        unsigned int muDayAttributes[TOTAL_DAY_ATTRIBUTE_DAYS];
        time_t mtSunRise[TOTAL_DAY_ATTRIBUTE_DAYS];
        time_t mtSunSet[TOTAL_DAY_ATTRIBUTE_DAYS];


    protected:
    public:
        CCalendar();
        void setStartDate(time_t midnightStartDay);
        time_t getStartDate();

        //~CCalendar() {};
        void setDayAttribute(time_t day, unsigned int attribute, time_t sunRise, time_t sunSet);

        unsigned int getDayAttribute(time_t day);

        time_t calculateScheduledEventTime(time_t midnightDateTime, time_t sequenceTime, time_t eventTimeOffset);
};

#ifdef MAIN_PROGRAM
        CCalendar calendar;
#else
        extern CCalendar calendar;
#endif
