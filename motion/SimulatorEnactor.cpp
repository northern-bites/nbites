#include "SimulatorEnactor.h"

#include <iostream>

void SimulatorEnactor::run() {
    std::cout << "SimulatorEnactor::run()" << std::endl;
    while (running) {
        if (!running)
            break;
    }
}

