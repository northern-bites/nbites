#pragma once

#include "RoboGrams.h"

namespace tool {
namespace unlog {

class UnlogModule : public portals::Module {
public:
    UnlogModule();
    ~UnlogModule() {}

protected:
    virtual void run_();
};

}
}


