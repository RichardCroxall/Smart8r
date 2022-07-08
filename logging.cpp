
#include "include/runtime.h"
#if _WIN32
#include <processthreadsapi.h>
#endif


#include <map>

const int LogfileDaysToKeep = 10;

static const char* loggingLevelNames[] = {
    "Never",
    "Infor",
    "Debug",
    "Warni",
	"Asser",
    "Error",
    "Fatal",
};

CLogging::CLogging()
{
    logFileOpen = false;
    logFileHandle = nullptr;
    currentDay = 0;

    printLoggingLevel = LevelWarn;
    writeLoggingLevel = LevelWarn;
}


void CLogging::EnableLogging(bool localInteractive)
{
	if (localInteractive)
	{
        printLoggingLevel = LevelDebug;
	}
    else
    {
        printLoggingLevel = LevelWarn;
    }
    writeLoggingLevel = LevelInfo;
}

CLogging::~CLogging()
{
    closeFile();
}

void CLogging::logFatal(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    sendMessage(LevelFatal, format, args);
    va_end(args);
};
void CLogging::logError(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    sendMessage(LevelError, format, args);
    va_end(args);
};

void CLogging::logAssert(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    sendMessage(LevelAssert, format, args);
    va_end(args);
};

void CLogging::logWarn(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    sendMessage(LevelWarn, format, args);
    va_end(args);
};
void CLogging::logDebug(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    sendMessage(LevelDebug, format, args);
    va_end(args);
};

void CLogging::logInfo(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    sendMessage(LevelInfo, format, args);
    va_end(args);
};

//map thread identifiers to small integers
std::size_t indexThread(const std::thread::id id)
{
	//declare mapping to
    static std::size_t nextindex = 1 ;
    static std::mutex my_mutex;
    static std::map<std::thread::id, std::size_t> ids;
	
    std::lock_guard<std::mutex> lock(my_mutex);
	
    if (ids.find(id) == ids.end())
    {
        ids[id] = nextindex++;
    }
    return ids[id];
}

int CpuNumber()
{
#ifdef _WIN32
    unsigned int cpu = GetCurrentProcessorNumber();
#else
    unsigned int cpu = sched_getcpu();
#endif
    return (int) cpu;
}

void CLogging::sendMessage(LoggingLevelEnum logging_level, const char *format, va_list args)
{
    if (logging_level >= printLoggingLevel ||
        logging_level >= writeLoggingLevel)
    {
        char buffer[1024 + 1];
        time_t rawtime = tickingClock.getTime();
        struct tm * timeinfo = localtime(&rawtime);
        const short centiSeconds = tickingClock.getCentiSeconds();

    	//get current thread as a small integer
        const std::thread::id threadId = std::this_thread::get_id();
        const int threadIdInt = indexThread(threadId);

        const int cpu = CpuNumber();
    	
        sprintf(buffer, "%4d/%02d/%02d %02d:%02d:%02d.%02d %d %d %s:", timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday, 
            timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, centiSeconds, threadIdInt, cpu, loggingLevelNames[logging_level]);
        vsprintf(buffer + strlen(buffer), format, args);

        if (logging_level >= printLoggingLevel)
        {
            printf("%s", buffer);
        }

        if (logging_level >= writeLoggingLevel)
        {
            WriteMessage(buffer);
        }
    }
}

/// Calculate logfile name for the given day.
//note statically allocated storage for filename is overwritten on next call to LogFileNameForDay.
static char* LogFileNameForDay(time_t rawtime)
{
    struct tm * timeInfo = localtime(&rawtime);
    static char filename[30 + 1];
    sprintf(filename, "%s/smartlog_%04d_%02d_%02d.txt", LOG_DIRECTORY_NAME, timeInfo->tm_year + 1900, timeInfo->tm_mon + 1, timeInfo->tm_mday);
    return filename;
}

void CLogging::WriteMessage(const char *message)
{
    const time_t rawTime = tickingClock.getTime();
    const time_t today = rawTime / SECONDSPERDAY;

    if (logFileOpen && today != currentDay)
    {
        closeFile();
    }

    if (!logFileOpen)
    {
#ifndef _WIN32
    	int ignored = mkdir(LOG_DIRECTORY_NAME, 0); //TODO set directory permissions correctly.
#else
        int ignored = _mkdir(LOG_DIRECTORY_NAME);
#endif
        //
        //remove a file which is exactly the right age to delete just before 
        time_t oldTime = rawTime - LogfileDaysToKeep * SECONDSPERDAY;
        char* oldLogFileName = LogFileNameForDay(oldTime);
        remove(oldLogFileName);

        char* filename = LogFileNameForDay(rawTime);

        currentDay = today;
        logFileHandle = fopen(filename, "a");
        logFileOpen = true;
    }

    fprintf(logFileHandle, "%s", message);
}

void CLogging::closeFile()
{
    if (logFileHandle != nullptr)
    {
        fflush(logFileHandle);
        fclose(logFileHandle);
    }
    logFileOpen = false;
    logFileHandle = nullptr;
}