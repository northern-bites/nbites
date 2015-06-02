#include "VisionModule.h"
#include "Edge.h"
#include "HighResTimer.h"

#include <iostream>

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
    edges = new EdgeList(32000);
    houghLines = new HoughLineList(128);
    hough = new HoughSpace(320, 240);

    // TODO compile time flag
    bool fast = false;
    frontEnd->fast(fast);
    edgeDetector->fast(fast);
    hough->fast(fast);
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

    HighResTimer timer("Setup");

    // Setup
    houghLines->clear();
    std::vector<const messages::YUVImage*> images { &topIn.message(), 
                                                    &bottomIn.message() };

    // Loop over top and bottom image and run line detection system
    for (int i = 0; i < images.size(); i++) {
        // Get image
        const messages::YUVImage* image = images[i];

        // Construct YuvLite object for use in vision system
        YuvLite yuvLite(image->width(),
                        image->height(),
                        image->rowPitch(),
                        image->pixelAddress(0, 0));

        timer.end("Front end");

        // Run front end
        frontEnd->run(yuvLite, colorParams);
        ImageLiteU16 yImage(frontEnd->yImage());
        ImageLiteU8 greenImage(frontEnd->greenImage());

        timer.end("Gradient");

        // Approximate brightness gradient
        edgeDetector->gradient(yImage);

        timer.end("Edge detection");

        // Run edge detection
        edges->reset();
        edgeDetector->edgeDetect(greenImage, *edges);

        timer.end("Hough");

        // Run hough line detection
        // TODO duplicate lines in houghLines?
        hough->run(*edges, *houghLines);
    }

    timer.lap();
}

}
}
