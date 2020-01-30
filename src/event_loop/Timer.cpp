#include "Timer.h"
#include <math.h>
using namespace std::chrono;

static constexpr milliseconds EPSILON = 1ms;


Timer::Timer() : time(0ms)
{}

Timer::Timer(size_t period)
{
	setPeriod(period);
}

void Timer::setPeriod(size_t period) {
	auto t = duration<size_t, std::ratio<1, 1000>>(period);
    time = steady_clock::now() + t;
}


bool Timer::isRang()
{
	return ((time - steady_clock::now()) < EPSILON);
}