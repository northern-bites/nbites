#pragma once

#include <iostream>
#include <string>
#include <chrono>

class HighResTimer {
public:
    HighResTimer() : start(std::chrono::high_resolution_clock::now()) {}
    double lap() const;
    double end();

private:
    std::chrono::high_resolution_clock::time_point start;
};
