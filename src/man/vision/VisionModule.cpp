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

    // TODO flag
    bool fast = true;
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

// TODO run on lowres bottom image
void VisionModule::run_()
{
    // Get messages from inPortals
    topIn.latch();
    bottomIn.latch();
    jointsIn.latch();
    inertialsIn.latch();

    // Setup
    std::vector<const messages::YUVImage*> images { &topIn.message(),
                                                    &bottomIn.message() };

    // Time vision module
    HighResTimer timer;
    double times[4];

    // Loop over top and bottom image and run line detection system
    for (int i = 0; i < images.size(); i++) {
        // Get image
        const messages::YUVImage* image = images[i];

        // Construct YuvLite object for use in vision system
        YuvLite yuvLite(image->width() / 4,
                        image->height() / 2,
                        image->rowPitch(),
                        image->pixelAddress(0, 0));

        // Run front end
        frontEnd->run(yuvLite, colorParams);
        ImageLiteU16 yImage(frontEnd->yImage());
        ImageLiteU8 greenImage(frontEnd->greenImage());

        times[0] = timer.end();

        // Approximate brightness gradient
        edgeDetector->gradient(yImage);
        
        times[1] = timer.end();

        // Run edge detection
        edges->reset();
        edgeDetector->edgeDetect(greenImage, *edges);

        times[2] = timer.end();

        // Run hough line detection
        houghLines->clear();
        hough->run(*edges, *houghLines);

        times[3] = timer.end();
    }

    std::cout << "Front end: " << times[0] << std::endl;
    std::cout << "Gradient: " << times[1] << std::endl;
    std::cout << "Edge detection: " << times[2] << std::endl;
    std::cout << "Hough: " << times[3] << std::endl;
}

}
}
