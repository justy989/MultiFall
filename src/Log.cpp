#include "Log.h"

std::ofstream Log::mOutputStream;
Log* Log::mLog = NULL;
char Log::mWideToSingleCharBuffer[128];

bool Log::setFilePath( char* path )
{
    mOutputStream.open( path, std::ios::ate );
    return mOutputStream.is_open();
}

std::ostream& Log::info()
{
    time_t rawTime;
    struct tm* timeInfo;
    char buffer[16];
    time(&rawTime);
    timeInfo = localtime( &rawTime );
    strncpy(buffer, asctime(timeInfo) + LOG_DATE_OFFSET, LOG_DATE_END);
    buffer[LOG_DATE_END] = '\0';
    mOutputStream << buffer << " Info: ";
    return mOutputStream;
}

std::ostream& Log::warn()
{
    time_t rawTime;
    struct tm * timeInfo;
    char buffer[16];
    time(&rawTime);
    timeInfo = localtime( &rawTime );
    strncpy(buffer, asctime(timeInfo) + LOG_DATE_OFFSET, LOG_DATE_END);
    buffer[LOG_DATE_END] = '\0';
    mOutputStream << buffer << " Warn: ";
    return mOutputStream;
}

std::ostream& Log::erro()
{
    time_t rawTime;
    struct tm * timeInfo;
    char buffer[16];
    time(&rawTime);
    timeInfo = localtime( &rawTime );
    strncpy(buffer, asctime(timeInfo) + LOG_DATE_OFFSET, LOG_DATE_END);
    buffer[LOG_DATE_END] = '\0';
    mOutputStream << buffer << " ERRO: ";
    return mOutputStream;
}