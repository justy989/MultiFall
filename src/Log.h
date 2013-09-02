#ifndef LOG_H
#define LOG_H

#include <fstream>
#include <string>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>

#define LOG_INFO Log::getInst()->info()
#define LOG_WARN Log::getInst()->warn()
#define LOG_ERRO Log::getInst()->erro()

//Macro to convert Wide character strings to character strings for logging. Max of 128
#define LOG_WC_TO_C( string ) Log::getInst()->wcToC( string )

#define LOG_ENDL std::endl

//Using this, we skip printing the date
#define LOG_DATE_OFFSET 11
#define LOG_DATE_END 8

#define WC_TO_C( wString ) 

class Log{
public:

    inline static Log* getInst();
    inline static void destroyInst();

    static bool setFilePath( char* path );

    //For various log levels
    std::ostream& info();
    std::ostream& warn();
    std::ostream& erro();

    inline char* wcToC( LPCWSTR string );

protected:

    Log(){}
    ~Log(){}

    static Log* mLog;
    static std::ofstream mOutputStream;

    static char mWideToSingleCharBuffer[128];
};

inline Log* Log::getInst()
{
    if( !mLog ){
        mLog = new Log();
    }

    return mLog;
}

inline void Log::destroyInst()
{
    mOutputStream.close();
    mOutputStream.clear();

    if( mLog ){
        delete mLog;
    }
}

inline char* Log::wcToC( LPCWSTR string )
{
    wcstombs( mWideToSingleCharBuffer, string, 128 );
    return mWideToSingleCharBuffer;
}

#endif