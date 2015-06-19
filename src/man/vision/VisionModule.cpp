#include "VisionModule.h"
#include "Edge.h"
#include "HighResTimer.h"
#include "NBMath.h"

#include <iostream>
#include <fstream>
#include <chrono>

#include "Profiler.h"
#include "DebugConfig.h"
//#include "PostDetector.h"

namespace man {
namespace vision {

VisionModule::VisionModule(int wd, int ht)
    : Module(),
      topIn(),
      bottomIn(),
      jointsIn(),
      linesOut(base()),
      ballOut(base()),
      ballOn(false),
      ballOnCount(0),
      ballOffCount(0)
{
    std::string sexpPath;
#ifdef OFFLINE
    sexpPath = std::string(getenv("NBITES_DIR"));
    sexpPath += "/src/man/config/colorParams.txt";
#else
    sexpPath = "/home/nao/nbites/Config/colorParams.txt"; // TODO check this
#endif
    std::ifstream textFile;
    textFile.open(sexpPath.c_str());

    // Get size of file
    textFile.seekg (0, textFile.end);
    long size = textFile.tellg();
    textFile.seekg(0);

    // Read file into buffer and convert to string
    char* buff = new char[size];
    textFile.read(buff, size);
    std::string sexpText(buff);

    // Get SExpr from string
    nblog::SExpr* colors = nblog::SExpr::read((const std::string)sexpText);

    // Set module pointers for top then bottom images
    // NOTE Constructed on heap because some of the objects below do
    //      not have default constructors, all class members must be initialized
    //      after the initializer list is run, which requires calling default
    //      constructors in the case of C-style arrays, limitation theoretically
    //      removed in C++11.
    for (int i = 0; i < 2; i++) {
        colorParams[i] = getColorsFromLisp(colors, i);
        frontEnd[i] = new ImageFrontEnd();
        edgeDetector[i] = new EdgeDetector();
        edges[i] = new EdgeList(32000);
        houghLines[i] = new HoughLineList(128);
        kinematics[i] = new Kinematics(i == 0);
        homography[i] = new FieldHomography(i == 0);
        fieldLines[i] = new FieldLineList();
        ballDetector[i] = new BallDetector(homography[i], i == 0);
        boxDetector[i] = new GoalboxDetector();

        // TODO set width and height dynamically
        if (i == 0) {
          hough[i] = new HoughSpace(wd / 2, ht / 2);
          cornerDetector[i] = new CornerDetector(wd / 2, ht / 2);
        } else {
          hough[i] = new HoughSpace(wd / 4, ht / 4);
          cornerDetector[i] = new CornerDetector(wd / 4, ht / 4);
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

    bool ballDetected = false;

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
        ImageLiteU8 orangeImage(frontEnd[i]->orangeImage());

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

        if (!ballDetected) {
            ballDetected = ballDetector[i]->findBall(orangeImage, kinematics[i]->wz0());
        }

        times[i][6] = timer.end();
    }

    ballOn = ballDetected;

    // Send messages on outportals
    sendLinesOut();
    updateVisionBall();

    for (int i = 0; i < 2; i++) {
        break;
        if (i == 0)
            std::cout << "From top camera:" << std::endl;
        else
            std::cout << std::endl << "From bottom camera:" << std::endl;
        std::cout << "Front end: " << times[i][0] << std::endl;
        std::cout << "Gradient: " << times[i][1] << std::endl;
        std::cout << "Edge detection: " << times[i][2] << std::endl;
        std::cout << "Hough: " << times[i][3] << std::endl;
        std::cout << "Field lines detection: " << times[i][4] << std::endl;
        std::cout << "Field lines classification: " << times[i][5] << std::endl;
        std::cout << "Ball: " << times[i][6] << std::endl;
    }
}

/*
 Lisp data in config/colorParams.txt stores 32 parameters. Read lisp and
  load the three compoenets of a Colors struct, white, green, and orange,
  from the 18 values for either the top or bottom image. 
*/
Colors* VisionModule::getColorsFromLisp(nblog::SExpr* colors, int camera) {
    Colors* ret = new man::vision::Colors;
    nblog::SExpr* params;

    if (camera == 0) {
        params = colors->get(1)->find("Top")->get(1);
    } else if (camera == 1) {
        params = colors->get(1)->find("Bottom")->get(1);
    } else {
        params = colors->get(1);
    }

    colors = params->get(0)->get(1);

    
    ret->white. load(std::stof(colors->get(0)->get(1)->serialize()),
                     std::stof(colors->get(1)->get(1)->serialize()),
                     std::stof(colors->get(2)->get(1)->serialize()),
                     std::stof(colors->get(3)->get(1)->serialize()),
                     std::stof(colors->get(4)->get(1)->serialize()),
                     std::stof(colors->get(5)->get(1)->serialize())); 
    
    colors = params->get(1)->get(1);

    ret->green. load(std::stof(colors->get(0)->get(1)->serialize()),
                     std::stof(colors->get(1)->get(1)->serialize()),
                     std::stof(colors->get(2)->get(1)->serialize()),
                     std::stof(colors->get(3)->get(1)->serialize()),
                     std::stof(colors->get(4)->get(1)->serialize()),
                     std::stof(colors->get(5)->get(1)->serialize()));  
    
    colors = params->get(2)->get(1);

    ret->orange.load(std::stof(colors->get(0)->get(1)->serialize()),
                     std::stof(colors->get(1)->get(1)->serialize()),
                     std::stof(colors->get(2)->get(1)->serialize()),
                     std::stof(colors->get(3)->get(1)->serialize()),
                     std::stof(colors->get(4)->get(1)->serialize()),
                     std::stof(colors->get(5)->get(1)->serialize()));

    return ret;
}

void VisionModule::updateVisionBall()
{
    portals::Message<messages::VisionBall> ball_message(0);

    Ball topBall = ballDetector[0]->best();
    Ball botBall = ballDetector[1]->best();

    bool top = false;
    Ball best = botBall;

    if (ballOn) {
        ballOnCount++;
        ballOffCount = 0;
    }
    else {
        ballOnCount = 0;
        ballOffCount++;
    }

    if (topBall.confidence() > botBall.confidence()) {
        best = topBall;
        top = true;
    }

    ball_message.get()->set_on(true);
    ball_message.get()->set_frames_on(ballOnCount);
    ball_message.get()->set_frames_off(ballOffCount);
    ball_message.get()->set_intopcam(top);

    if (ballOn)
    {
        ball_message.get()->set_distance(best.dist);

        ball_message.get()->set_radius(best.blob.firstPrincipalLength());
        double bearing = atan(best.x_rel / best.y_rel);
        ball_message.get()->set_bearing(bearing);
        ball_message.get()->set_bearing_deg(bearing * TO_DEG);

        double angle_x = (best.imgWidth/2 - best.getBlob().centerX()) /
            (best.imgWidth/2) * HORIZ_FOV_DEG;
        double angle_y = (best.imgHeight/2 - best.getBlob().centerY()) /
            (best.imgHeight/2) * VERT_FOV_DEG;
        ball_message.get()->set_angle_x_deg(angle_x);
        ball_message.get()->set_angle_y_deg(angle_y);

        ball_message.get()->set_confidence(best.confidence());
        ball_message.get()->set_x(static_cast<int>(best.blob.centerX()));
        ball_message.get()->set_y(static_cast<int>(best.blob.centerY()));
    }

    ballOut.setMessage(ball_message);
}

// TODO filter out repeat lines
void VisionModule::sendLinesOut()
{
    messages::FieldLines pLines;

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < fieldLines[i]->size(); j++) {
            messages::FieldLine* pLine = pLines.add_line();
            FieldLine& line = (*(fieldLines[i]))[j];

            for (int k = 0; k < 2; k++) {
                messages::HoughLine pHough;
                HoughLine& hough = line[k];

                pHough.set_r(hough.field().r());
                pHough.set_t(hough.field().t());
                pHough.set_ep0(hough.field().ep0());
                pHough.set_ep1(hough.field().ep1());

                if (hough.field().r() < 0)
                    pLine->mutable_outer()->CopyFrom(pHough);
                else
                    pLine->mutable_inner()->CopyFrom(pHough);
            }

            pLine->set_id(static_cast<int>(line.id()));
        }
    }

    portals::Message<messages::FieldLines> linesOutMessage(&pLines);
    linesOut.setMessage(linesOutMessage);
}

}
}
