#include "VisionModule.h"
#include "Edge.h"

namespace man {
namespace vision {

VisionModule::VisionModule()
    : Module(),
      imageIn(),
      jointsIn(),
      inertialsIn()
{
    // TODO constants
    edgeDetector = new EdgeDetector();
    edges = new EdgeList(3200);
    houghLines = new HoughLineList(128);
    hough = new HoughSpace(320, 240);
}

VisionModule::~VisionModule()
{
    delete edgeDetector;
    delete edges;
    delete houghLines;
    delete hough;
}

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

    edgeDetector->gradient(yImage.pixelAddress(0, 0), yImage.width(),
                           yImage.height(), yImage.width());
    edges->reset();
    edgeDetector->edgeDetect(greenImage.pixelAddress(0, 0), 
                             greenImage.width(),
                             *edges);

    houghLines->clear();
    hough->run(*edges, *houghLines);
}

}
}
