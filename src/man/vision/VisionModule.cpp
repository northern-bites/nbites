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
    colorParams = new Colors();
    frontEnd = new ImageFrontEnd();
    edgeDetector = new EdgeDetector();
    edges = new EdgeList(3200);
    houghLines = new HoughLineList(128);
    hough = new HoughSpace(320, 240);
}

VisionModule::~VisionModule()
{
    delete colorParams;
    delete frontEnd;
    delete edgeDetector;
    delete edges;
    delete houghLines;
    delete hough;
}

void VisionModule::run_()
{
    // Get messages from inPortals
    imageIn.latch();
    jointsIn.latch();
    inertialsIn.latch();

    ::messages::YUVImage image(imageIn.message());
    ::messages::JointAngles joints(jointsIn.message());
    ::messages::InertialState inertials(inertialsIn.message());

    // Construct YuvLite object for use in vision system
    YuvLite yuvImage(image.width(),
                     image.height(),
                     image.rowPitch(),
                     image.pixelAddress(0, 0));

    // Run front end
    frontEnd->run(yuvImage, colorParams);
    ImageLiteU16 yImage(frontEnd->yImage());
    ImageLiteU8 greenImage(frontEnd->greenImage());

    // Run edge detection
    edgeDetector->gradient(yImage);
    edges->reset();
    edgeDetector->edgeDetect(greenImage, *edges);

    // Run hough line detection
    houghLines->clear();
    hough->run(*edges, *houghLines);
}

}
}
