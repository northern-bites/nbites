#include "VisionModule.h"
#include "Edge.h"

namespace man {
namespace vision {

VisionModule::VisionModule()
    : Module(),
      imageIn(),
      jointsIn(),
      inertialsIn()
{}

// TODO
VisionModule::VisionModule(char* tablePathname)
{}

VisionModule::~VisionModule()
{}

void VisionModule::run_()
{
    yImageIn.latch();
    whiteImageIn.latch();
    greenImageIn.latch();
    whiteImageIn.latch();

    jointsIn.latch();
    inertialsIn.latch();

    messages::PackedImage16 yImage(yImageIn.message());
    messages::PackedImage8 whiteImage(whiteImageIn.message());
    messages::PackedImage8 greenImage(greenImageIn.message());
    messages::PackedImage8 orangeImage(orangeImageIn.message());

    messages::JointAngles joints(jointsIn.message());
    messages::InertialState inertials(inertialsIn.message());
}

}
}
