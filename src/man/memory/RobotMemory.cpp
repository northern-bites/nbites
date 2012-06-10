
#include <iostream>

#include "Common.h"
#include "RobotMemory.h"

namespace man {
namespace memory {

using boost::shared_ptr;
using namespace proto;
using namespace std;

RobotMemory::RobotMemory() {

    this->addObject<MVision>();
    this->addObject<MVisionSensors>();
    this->addObject<MMotionSensors>();
    this->addObject<MRawImages>();
    this->addObject<MLocalization>();
}

RobotMemory::~RobotMemory() {
    cout << "Robot Memory destructor" << endl;
}

}
}
