//small utility function
#pragma once

char* formatTime(time_t timeDate);

class CTimeout
{
    private:
    protected:
        time_t mExpiryTime;
        time_t mDefaultDuration;
        actionNo_t mOffAction;
        char mszIdentifier[MAXRUNTIMEIDENTIFIERLENGTH + 1];


    public:
        CTimeout(time_t defaultDuration, actionNo_t offAction, char* szIdentifier);
        //~CTimeout() {};
        void reset(time_t duration);
        void mCheckSwitchOffTimeout(time_t now);
        bool mGetTimeoutExpired(time_t now);
        static void printTimeouts();
};

