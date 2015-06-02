#include "HighResTimer.h"

using namespace std::chrono;

double HighResTimer::lap() const 
{
    high_resolution_clock::time_point end = high_resolution_clock::now();
    duration<double> timeSpan = duration_cast<duration<double>>(end - start);
    return 1000*timeSpan.count();
}

double HighResTimer::end()
{
    double time = lap();
    start = high_resolution_clock::now();
    return time;
}
