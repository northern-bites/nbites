#include "VisionModule.h"
#include "Edge.h"
#include "HighResTimer.h"
#include "NBMath.h"
#include "../control/control.h"
#include "../log/logging.h"

#include <fstream>
#include <iostream>
#include <fstream>
#include <chrono>

#include "Profiler.h"
#include "DebugConfig.h"
//#include "PostDetector.h"

namespace man {
namespace vision {

VisionModule::VisionModule(int wd, int ht, std::string robotName)
    : Module(),
      topIn(),
      bottomIn(),
      jointsIn(),
      linesOut(base()),
      cornersOut(base()),
      ballOut(base()),
      ballOn(false),
      ballOnCount(0),
      ballOffCount(0)
{
    std:: string colorPath, calibrationPath;
    #ifdef OFFLINE
        colorPath =  calibrationPath = std::string(getenv("NBITES_DIR"));
        colorPath += "/src/man/config/colorParams.txt";
        calibrationPath += "/src/man/config/calibrationParams.txt";
    #else
        colorPath = "/home/nao/nbites/Config/colorParams.txt";
        calibrationPath = "/home/nao/nbites/Config/calibrationParams.txt";
    #endif

    // Get SExpr from string
    nblog::SExpr* colors = nblog::SExpr::read(getStringFromTxtFile(colorPath));
    calibrationLisp = nblog::SExpr::read(getStringFromTxtFile(calibrationPath));

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
        calibrationParams[i] = new CalibrationParams();
        kinematics[i] = new Kinematics(i == 0);
        homography[i] = new FieldHomography(i == 0);
        fieldLines[i] = new FieldLineList();
        ballDetector[i] = new BallDetector(homography[i], i == 0);
        boxDetector[i] = new GoalboxDetector();

        if (i == 0) {
          hough[i] = new HoughSpace(wd / 2, ht / 2);
          cornerDetector[i] = new CornerDetector(wd / 2, ht / 2);
        } else {
          hough[i] = new HoughSpace(wd / 4, ht / 4);
          cornerDetector[i] = new CornerDetector(wd / 4, ht / 4);
        }

        bool fast = true;
        frontEnd[i]->fast(fast);
        edgeDetector[i]->fast(fast);
        hough[i]->fast(fast);
    }
    setCalibrationParams(robotName);
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
        delete calibrationParams[i];
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
    inertsIn.latch();

    // Setup
    std::vector<const messages::YUVImage*> images { &topIn.message(),
                                                    &bottomIn.message() };
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

        // Run front end
        frontEnd[i]->run(yuvLite, colorParams[i]);
        ImageLiteU16 yImage(frontEnd[i]->yImage());
        ImageLiteU8 greenImage(frontEnd[i]->greenImage());
        ImageLiteU8 orangeImage(frontEnd[i]->orangeImage());

        // Calculate kinematics and adjust homography
        if (jointsIn.message().has_head_yaw()) {
            kinematics[i]->joints(jointsIn.message());
            homography[i]->wx0(kinematics[i]->wx0());
            homography[i]->wy0(kinematics[i]->wy0());
            homography[i]->wz0(kinematics[i]->wz0());
            homography[i]->roll(calibrationParams[i]->getRoll());
            homography[i]->tilt(kinematics[i]->tilt() + calibrationParams[i]->getTilt());
#ifndef OFFLINE
            homography[i]->azimuth(kinematics[i]->azimuth());
#endif
        }

        // Approximate brightness gradient
        edgeDetector[i]->gradient(yImage);
        
        // Run edge detection
        edgeDetector[i]->edgeDetect(greenImage, *(edges[i]));

        // Run hough line detection
        hough[i]->run(*(edges[i]), *(houghLines[i]));

        // Find field lines
        houghLines[i]->mapToField(*(homography[i]));
        fieldLines[i]->find(*(houghLines[i]));

        // Classify field lines
        fieldLines[i]->classify(*(boxDetector[i]), *(cornerDetector[i]));

        ballDetected |= ballDetector[i]->findBall(orangeImage, kinematics[i]->wz0());
    }

    // Send messages on outportals
    sendLinesOut();
    ballOn = ballDetected;
    updateVisionBall();

// TODO move to logImage
#ifdef USE_LOGGING
    if (getenv("LOG_THIS") != NULL) {
        if (strcmp(getenv("LOG_THIS"), std::string("top").c_str()) == 0) {
            logImage(0);
            setenv("LOG_THIS", "false", 1);
            std::cerr << "pCal logging top log\n";
        } else if (strcmp(getenv("LOG_THIS"), std::string("bottom").c_str()) == 0) {
            logImage(1);
            setenv("LOG_THIS", "false", 1);
            std::cerr << "pCal logging bot log\n";
        }// else
           // std::cerr << "N "; 
    } else {
        logImage(0);
        logImage(1);
    }
#endif
}

#ifdef USE_LOGGING
void VisionModule::logImage(int i) 
{
    std::string t = "true";

    if (control::flags[control::tripoint]) {
        ++image_index;
        
        messages::YUVImage image;
        std::string image_from;
        if (!i) {
            image = topIn.message();
            image_from = "camera_TOP";
        } else {
            image = bottomIn.message();
            image_from = "camera_BOT";
        }
        
        long im_size = (image.width() * image.height() * 1);
        int im_width = image.width() / 2;
        int im_height= image.height();
        
        messages::JointAngles ja_pb = jointsIn.message();
        messages::InertialState is_pb = inertsIn.message();
        
        std::string ja_buf;
        std::string is_buf;
        std::string im_buf((char *) image.pixelAddress(0, 0), im_size);
        ja_pb.SerializeToString(&ja_buf);
        is_pb.SerializeToString(&is_buf);
        
        im_buf.append(is_buf);
        im_buf.append(ja_buf);
        
        std::vector<nblog::SExpr> contents;
        
        nblog::SExpr imageinfo("YUVImage", image_from, clock(), image_index, im_size);
        imageinfo.append(nblog::SExpr("width", im_width)   );
        imageinfo.append(nblog::SExpr("height", im_height) );
        imageinfo.append(nblog::SExpr("encoding", "[Y8(U8/V8)]"));
        contents.push_back(imageinfo);
        
        nblog::SExpr inerts("InertialState", "tripoint", clock(), image_index, is_buf.length());
        inerts.append(nblog::SExpr("acc_x", is_pb.acc_x()));
        inerts.append(nblog::SExpr("acc_y", is_pb.acc_y()));
        inerts.append(nblog::SExpr("acc_z", is_pb.acc_z()));
        
        inerts.append(nblog::SExpr("gyr_x", is_pb.gyr_x()));
        inerts.append(nblog::SExpr("gyr_y", is_pb.gyr_y()));
        
        inerts.append(nblog::SExpr("angle_x", is_pb.angle_x()));
        inerts.append(nblog::SExpr("angle_y", is_pb.angle_y()));
        contents.push_back(inerts);
        
        nblog::SExpr joints("JointAngles", "tripoint", clock(), image_index, ja_buf.length());
        joints.append(nblog::SExpr("head_yaw", ja_pb.head_yaw()));
        joints.append(nblog::SExpr("head_pitch", ja_pb.head_pitch()));

        joints.append(nblog::SExpr("l_shoulder_pitch", ja_pb.l_shoulder_pitch()));
        joints.append(nblog::SExpr("l_shoulder_roll", ja_pb.l_shoulder_roll()));
        joints.append(nblog::SExpr("l_elbow_yaw", ja_pb.l_elbow_yaw()));
        joints.append(nblog::SExpr("l_elbow_roll", ja_pb.l_elbow_roll()));
        joints.append(nblog::SExpr("l_wrist_yaw", ja_pb.l_wrist_yaw()));
        joints.append(nblog::SExpr("l_hand", ja_pb.l_hand()));

        joints.append(nblog::SExpr("r_shoulder_pitch", ja_pb.r_shoulder_pitch()));
        joints.append(nblog::SExpr("r_shoulder_roll", ja_pb.r_shoulder_roll()));
        joints.append(nblog::SExpr("r_elbow_yaw", ja_pb.r_elbow_yaw()));
        joints.append(nblog::SExpr("r_elbow_roll", ja_pb.r_elbow_roll()));
        joints.append(nblog::SExpr("r_wrist_yaw", ja_pb.r_wrist_yaw()));
        joints.append(nblog::SExpr("r_hand", ja_pb.r_hand()));

        joints.append(nblog::SExpr("l_hip_yaw_pitch", ja_pb.l_hip_yaw_pitch()));
        joints.append(nblog::SExpr("r_hip_yaw_pitch", ja_pb.r_hip_yaw_pitch()));

        joints.append(nblog::SExpr("l_hip_roll", ja_pb.l_hip_roll()));
        joints.append(nblog::SExpr("l_hip_pitch", ja_pb.l_hip_pitch()));
        joints.append(nblog::SExpr("l_knee_pitch", ja_pb.l_knee_pitch()));
        joints.append(nblog::SExpr("l_ankle_pitch", ja_pb.l_ankle_pitch()));
        joints.append(nblog::SExpr("l_ankle_roll", ja_pb.l_ankle_roll()));

        joints.append(nblog::SExpr("r_hip_roll", ja_pb.r_hip_roll() ));
        joints.append(nblog::SExpr("r_hip_pitch", ja_pb.r_hip_pitch() ));
        joints.append(nblog::SExpr("r_knee_pitch", ja_pb.r_knee_pitch() ));
        joints.append(nblog::SExpr("r_ankle_pitch", ja_pb.r_ankle_pitch() ));
        joints.append(nblog::SExpr("r_ankle_roll", ja_pb.r_ankle_roll() ));
        contents.push_back(joints);

        nblog::SExpr cal("CalibrationParams", "tripoint", clock(), image_index, 0);
        cal.append(nblog::SExpr(image_from, calibrationParams[i]->getRoll(), calibrationParams[i]->getTilt()));
        contents.push_back(cal);

        nblog::NBLog(NBL_IMAGE_BUFFER, "tripoint", contents, im_buf);
    }
}

#endif

void VisionModule::sendLinesOut()
{
    // Mark repeat lines (already found in bottom camera) in top camera
    for (int i = 0; i < fieldLines[0]->size(); i++) {
        for (int j = 0; j < fieldLines[1]->size(); j++) {
            FieldLine& topField = (*(fieldLines[0]))[i];
            FieldLine& botField = (*(fieldLines[1]))[j];
            for (int k = 0; k < 2; k++) {
                const GeoLine& topGeo = topField[k].field();
                const GeoLine& botGeo = botField[k].field();
                if (topGeo.error(botGeo) < 0.3) // TODO constant
                    (*(fieldLines[0]))[i].repeat(true);
            }
        }
    }

    // Outportal results
    // NOTE repeats are not outportaled
    messages::FieldLines pLines;
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < fieldLines[i]->size(); j++) {
            messages::FieldLine* pLine = pLines.add_line();
            FieldLine& line = (*(fieldLines[i]))[j];
            if (line.repeat()) continue;

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

// TODO repeats
void VisionModule::sendCornersOut()
{
    messages::Corners pCorners;
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < cornerDetector[i]->size(); j++) {
            messages::Corner* pCorner = pCorners.add_corner();
            Corner& corner = (*(cornerDetector[i]))[j];

            pCorner->set_x(corner.x);
            pCorner->set_y(corner.y);
            pCorner->set_id(static_cast<int>(corner.id));
        }
    }

    portals::Message<messages::Corners> cornersOutMessage(&pCorners);
    cornersOut.setMessage(cornersOutMessage);
}

const std::string VisionModule::getStringFromTxtFile(std::string path) 
{
    std::ifstream textFile;
    textFile.open(path);

    // Get size of file
    textFile.seekg (0, textFile.end);
    long size = textFile.tellg();
    textFile.seekg(0);
    
    // Read file into buffer and convert to string
    char* buff = new char[size];
    textFile.read(buff, size);
    std::string sexpText(buff);

    textFile.close();
    return (const std::string)sexpText;
}

/*
 Lisp data in config/colorParams.txt stores 32 parameters. Read lisp and
  load the three compoenets of a Colors struct, white, green, and orange,
  from the 18 values for either the top or bottom image. 
*/
Colors* VisionModule::getColorsFromLisp(nblog::SExpr* colors, int camera) 
{
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

    ball_message.get()->set_on(ballOn);
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
            (best.imgWidth) * HORIZ_FOV_DEG;
        double angle_y = (best.imgHeight/2 - best.getBlob().centerY()) /
            (best.imgHeight) * VERT_FOV_DEG;
        ball_message.get()->set_angle_x_deg(angle_x);
        ball_message.get()->set_angle_y_deg(angle_y);

        ball_message.get()->set_confidence(best.confidence());
        ball_message.get()->set_x(static_cast<int>(best.blob.centerX()));
        ball_message.get()->set_y(static_cast<int>(best.blob.centerY()));
    }

    ballOut.setMessage(ball_message);
}

void VisionModule::setCalibrationParams(std::string robotName) 
{
    setCalibrationParams(0, robotName);
    setCalibrationParams(1, robotName);
}

void VisionModule::setCalibrationParams(int camera, std::string robotName) 
{
    if (std::string::npos != robotName.find(".local")) {
        robotName.resize(robotName.find("."));
        if (robotName == "she-hulk")
            robotName = "shehulk";
    }
    if (robotName == "") {
        return;
    }
    
    nblog::SExpr* robot = calibrationLisp->get(1)->find(robotName);

    if (robot != NULL) {
        std::string cam = camera == 0 ? "TOP" : "BOT";
        double roll =  robot->find(cam)->get(1)->valueAsDouble();
        double tilt = robot->find(cam)->get(2)->valueAsDouble();
        calibrationParams[camera] = new CalibrationParams(roll, tilt);

        std::cerr << "Found and set calibration params for " << robotName;
        std::cerr << "Roll: " << roll << " Tilt: " << tilt << std::endl;
    }
}

}
}
