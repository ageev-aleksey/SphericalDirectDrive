#ifndef _TIMER_H_
#define _TIMER_H_
#include <chrono>



class Timer {
public:
	Timer();
	Timer(size_t period);
	void setPeriod(size_t period);
	//time_t elapsedTime();
	bool isRang();
	//time_t timeLeft();
private:
	std::chrono::steady_clock::time_point time;
};

#endif
