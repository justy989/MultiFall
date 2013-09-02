#ifndef TIMER_H
#define TIMER_H

/*Timer - keeps time between frames*/
class Timer
{
public:

	Timer( float initialTime = 0.016667f );
	~Timer();

	//start the timer
	void start();

	//stop the timer
	void stop();

	//get time elapsed between start and end calls
	float getTimeElapsed();
	void setTimeElapsed( float t ){mElapsed = t;}

protected:

	__int64 mStartStamp; //start time
	__int64 mEndStamp; //end time
	__int64 mFreq; //timer frequency

	float mElapsed; //time elapsed
};

#endif