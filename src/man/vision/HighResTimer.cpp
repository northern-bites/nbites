#include "HighResTimer.h"

namespace man {
namespace vision {

using namespace std::chrono;

void HighResTimer::lap() const 
{
    high_resolution_clock::time_point end = high_resolution_clock::now();
    duration<double> timeSpan = duration_cast<duration<double>>(end - start);
    std::cout << name << " timed at: " << 1000*timeSpan.count() << " milliseconds." << std::endl;
}

void HighResTimer::end(std::string name_)
{
    lap();

    name = name_;
    start = high_resolution_clock::now();
}

}
}
