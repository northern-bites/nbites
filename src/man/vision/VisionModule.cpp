#include "VisionModule.h"
#include "Edge.h"

namespace man {
namespace vision {

VisionModule::VisionModule()
    : Module(),
      topIn(),
      bottomIn(),
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
    topIn.latch();
    bottomIn.latch();
    jointsIn.latch();
    inertialsIn.latch();

    messages::YUVImage top(topIn.message());
    messages::YUVImage bottom(bottomIn.message());
    messages::JointAngles joints(jointsIn.message());
    messages::InertialState inertials(inertialsIn.message());

    // Setup pre runnning vision system
    std::vector<messages::YUVImage> images { top, bottom };
    houghLines->clear();

    // Loop over top and bottom image and run line detection system
    for (int i = 0; i < images.size(); i++) {
        // Get image
        messages::YUVImage image(images[i]);

        // Construct YuvLite object for use in vision system
        YuvLite yuvLite(image.width(),
                        image.height(),
                        image.rowPitch(),
                        image.pixelAddress(0, 0));

        // Run front end
        frontEnd->run(yuvLite, colorParams);
        ImageLiteU16 yImage(frontEnd->yImage());
        ImageLiteU8 greenImage(frontEnd->greenImage());

        // Run edge detection
        edgeDetector->gradient(yImage);
        edges->reset();
        edgeDetector->edgeDetect(greenImage, *edges);

        // Run hough line detection
        // TODO duplicate lines in houghLines?
        hough->run(*edges, *houghLines);
    }
}

}
}
