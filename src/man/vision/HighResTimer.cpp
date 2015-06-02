#include "HighResTimer.h"

namespace man {
namespace vision {

using namespace std::chrono;

double HighResTimer::lap() const 
{
    high_resolution_clock::time_point end = high_resolution_clock::now();
    duration<double> timeSpan = duration_cast<duration<double>>(end - start);
    return 1000*timeSpan.count();
}

double HighResTimer::end(std::string name_)
{
    double time = lap();

    name = name_;
    start = high_resolution_clock::now();

    return time;
}

}
}

