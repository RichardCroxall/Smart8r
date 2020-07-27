#pragma once

#include <stdarg.h>     /* va_list, va_start, va_arg, va_end */

enum LoggingLevelEnum
{
    LevelNever,
    LevelInfo,
    LevelDebug,
    LevelWarn,
    LevelError,
    LevelFatal,
};

#define LOG_DIRECTORY_NAME "log"

class CLogging
{
private:
	
    bool logFileOpen;
    FILE* logFileHandle;
    time_t currentDay;

    LoggingLevelEnum printLoggingLevel;
    LoggingLevelEnum writeLoggingLevel;

    void sendMessage(LoggingLevelEnum logging_level, const char *format, va_list args);
    void WriteMessage(const char *message);

protected:
public:
    CLogging();
    ~CLogging();
    void EnableLogging(void);

    void logFatal(const char* format, ...);
    void logError(const char* format, ...);
    void logWarn(const char* format, ...);
    void logDebug(const char* format, ...);
    void logInfo(const char* format, ...);
    void closeFile();

};

#ifdef MAIN_PROGRAM
CLogging logging;
#else
extern CLogging logging;
#endif


