#include "Timer.h"
#include <Windows.h>

Timer::Timer( float initialTime )
{
	mElapsed = initialTime;
}

Timer::~Timer()
{

}

void Timer::start()
{
	QueryPerformanceFrequency( (LARGE_INTEGER*)&mFreq );
	QueryPerformanceCounter( (LARGE_INTEGER*)&mStartStamp );
}

void Timer::stop()
{
	QueryPerformanceCounter( (LARGE_INTEGER*)&mEndStamp );
	mElapsed = (float)( mEndStamp - mStartStamp ) / (float)mFreq;
}

float Timer::getTimeElapsed()
{
	return mElapsed;
}