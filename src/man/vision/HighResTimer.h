#pragma once

#include <iostream>
#include <string>
#include <chrono>

namespace man {
namespace vision {

class HighResTimer {
public:
    HighResTimer(std::string name_) : name(name_), start(std::chrono::high_resolution_clock::now()) {}
    void lap() const;
    void end(std::string name_);

private:
    std::string name;
    std::chrono::high_resolution_clock::time_point start;
};

}
}
