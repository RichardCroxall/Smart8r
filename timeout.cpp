#include "include/runtime.h"

CTimeout::CTimeout(time_t defaultDuration, actionNo_t offAction, char* szIdentifier)
{
    mDefaultDuration = defaultDuration;
    mExpiryTime = THE_END_OF_TIME;
    //mExpiryTime = time(NULL) + mDefaultDuration; // what about everyone out timers??
    mOffAction = offAction;
    assert(strlen(szIdentifier) < sizeof(mszIdentifier));
    strcpy(mszIdentifier, szIdentifier);
};

//~CTimeout() {};
void CTimeout::reset(time_t duration)
{
    mExpiryTime = tickingClock.getTime() + duration;
#if DEBUG
    logging.logInfo("timeout %s expiry %d duration %d defaultDuration %d\n", mszIdentifier, mExpiryTime, duration, mDefaultDuration);
#endif
}

void CTimeout::mCheckSwitchOffTimeout(time_t now)
{
    if (now >= mExpiryTime)
    {
#if DEBUG
        logging.logInfo("Expiry Timeout %s now=%ld mSwitchOffTimeout=%ld\n", mszIdentifier, now, mExpiryTime);
#endif
        mExpiryTime = THE_END_OF_TIME;
        if (mOffAction != NULL_ACTION)
        {
            loadMap.action[mOffAction]->fire(true);
        }
    }
}
bool CTimeout::mGetTimeoutExpired(time_t now)
{
    return (now >= mExpiryTime);
}





//note this returns a static variable containing the time.
//if you call it a second time without using the result, the first result will be **OVERWRITTEN**!
char* formatTime(time_t timeDate)
{
    static char formattedTime[20 + 1];

    if (timeDate != THE_END_OF_TIME)
    {
        struct tm* localCurrentTimeDate = gmtime(&timeDate);
        sprintf(formattedTime, "%02d/%02d/%02d %02d:%02d:%02d", //17 characters long
            localCurrentTimeDate->tm_mday,
            localCurrentTimeDate->tm_mon + 1,
            localCurrentTimeDate->tm_year + 1900 - 2000, /* calculate 2 digit year beyond 2000 */
            localCurrentTimeDate->tm_hour,
            localCurrentTimeDate->tm_min,
            localCurrentTimeDate->tm_sec);
    }
    else
    {
        strcpy(formattedTime, "---end-of-time---"); //17 characters long
    }

    return formattedTime;
}


void CTimeout::printTimeouts()
{
    for (int i = 0; i < codeHeaderRecord.noTimeoutEntries; i++)
    {
        printf("%30s Expiry %s offact %s\n",
            loadMap.timeout[i]->mszIdentifier,
            formatTime(loadMap.timeout[i]->mExpiryTime),
            loadMap.action[loadMap.timeout[i]->mOffAction]->mszIdentifier);
    }
}
