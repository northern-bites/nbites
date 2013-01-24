#include "UnlogModule.h"
#include <iostream>

namespace tool {
namespace unlog {

UnlogModule::UnlogModule() : Module()
{}

void UnlogModule::run_()
{
    std::cout << "Unlog Module run!" << std::endl;
}

}
}
