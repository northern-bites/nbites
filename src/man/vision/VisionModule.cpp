#include "VisionModule.h"
#include "Edge.h"
#include "HighResTimer.h"

#include <iostream>

namespace man {
namespace vision {

// TODO constants and parameters
VisionModule::VisionModule()
    : Module(),
      topIn(),
      bottomIn(),
      jointsIn()
{
    // NOTE Constructed on heap because some of the objects below do
    //      not have default constructors, all class members must be initialized
    //      after the initializer list is run, which requires calling default
    //      constructors in the case of C-style arrays, limitation theoretically
    //      removed in C++11.
    for (int i = 0; i < 2; i++) {
        colorParams[i] = new Colors();
        frontEnd[i] = new ImageFrontEnd();
        edgeDetector[i] = new EdgeDetector();
        edges[i] = new EdgeList(32000);
        houghLines[i] = new HoughLineList(128);
        kinematics[i] = new Kinematics(i == 0);
        homography[i] = new FieldHomography();
        fieldLines[i] = new FieldLineList();
        boxDetector[i] = new GoalboxDetector();

        if (i == 0) {
          hough[i] = new HoughSpace(320, 240);
          cornerDetector[i] = new CornerDetector(320, 240);
        } else {
          hough[i] = new HoughSpace(160, 120);
          cornerDetector[i] = new CornerDetector(160, 120);
        }

        // TODO flag
        bool fast = true;
        frontEnd[i]->fast(fast);
        edgeDetector[i]->fast(fast);
        hough[i]->fast(fast);
    }
}

VisionModule::~VisionModule()
{
    for (int i = 0; i < 2; i++) {
        delete colorParams[i];
        delete frontEnd[i];
        delete edgeDetector[i];
        delete edges[i];
        delete houghLines[i];
        delete hough[i];
        delete kinematics[i];
        delete homography[i];
        delete fieldLines[i];
    }
}

// TODO use horizon on top image
void VisionModule::run_()
{
    // Get messages from inPortals
    topIn.latch();
    bottomIn.latch();
    jointsIn.latch();

    // Setup
    std::vector<const messages::YUVImage*> images { &topIn.message(),
                                                    &bottomIn.message() };

    // Time vision module
    double topTimes[6];
    double bottomTimes[6];
    double* times[2] = { topTimes, bottomTimes };

    // Loop over top and bottom image and run line detection system
    for (int i = 0; i < images.size(); i++) {
        // Get image
        const messages::YUVImage* image = images[i];

        // Construct YuvLite object for use in vision system
        YuvLite yuvLite(image->width() / 4,
                        image->height() / 2,
                        image->rowPitch(),
                        image->pixelAddress(0, 0));

        HighResTimer timer;

        // Run front end
        frontEnd[i]->run(yuvLite, colorParams[i]);
        ImageLiteU16 yImage(frontEnd[i]->yImage());
        ImageLiteU8 greenImage(frontEnd[i]->greenImage());

        times[i][0] = timer.end();

        // Calculate kinematics and adjust homography
        if (jointsIn.message().has_head_yaw()) {
            kinematics[i]->joints(jointsIn.message());
            homography[i]->wz0(kinematics[i]->wz0());
            homography[i]->tilt(kinematics[i]->tilt());
#ifndef OFFLINE
            homography[i]->azimuth(kinematics[i]->azimuth());
#endif
        }

        // Approximate brightness gradient
        edgeDetector[i]->gradient(yImage);
        
        times[i][1] = timer.end();

        // Run edge detection
        edgeDetector[i]->edgeDetect(greenImage, *(edges[i]));

        times[i][2] = timer.end();

        // Run hough line detection
        hough[i]->run(*(edges[i]), *(houghLines[i]));

        times[i][3] = timer.end();

        // Find field lines
        houghLines[i]->mapToField(*(homography[i]));
        fieldLines[i]->find(*(houghLines[i]));

        times[i][4] = timer.end();

        // Classify field lines
        fieldLines[i]->classify(*(boxDetector[i]), *(cornerDetector[i]));

        times[i][5] = timer.end();
    }

    // for (int i = 0; i < 2; i++) {
    //     if (i == 0)
    //         std::cout << "From top camera:" << std::endl;
    //     else
    //         std::cout << std::endl << "From bottom camera:" << std::endl;
    //     std::cout << "Front end: " << times[i][0] << std::endl;
    //     std::cout << "Gradient: " << times[i][1] << std::endl;
    //     std::cout << "Edge detection: " << times[i][2] << std::endl;
    //     std::cout << "Hough: " << times[i][3] << std::endl;
    //     std::cout << "Field lines detection: " << times[i][4] << std::endl;
    //     std::cout << "Field lines classification: " << times[i][5] << std::endl;
    // }
}

}
}
