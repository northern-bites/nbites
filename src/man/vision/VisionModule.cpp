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
      visionOut(base()),
      robotObstacleOut(base()),
      ballOn(false),
      ballOnCount(0),
      ballOffCount(0),
      blackStar_(false)
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
        rejectedEdges[i] = new EdgeList(32000);
        houghLines[i] = new HoughLineList(128);
        calibrationParams[i] = new CalibrationParams();
        kinematics[i] = new Kinematics(i == 0);
        homography[i] = new FieldHomography(i == 0);
        fieldLines[i] = new FieldLineList();
#ifdef OFFLINE
		// Get the appropriate amount of space for the Debug Image
		if (i == 0) {
			debugSpace[0] = (uint8_t *)malloc(wd * ht * 2 * sizeof(uint8_t));
		} else {
			debugSpace[1] = (uint8_t *)malloc((wd / 2) * (ht / 2) * sizeof(uint8_t));
		}
#else
		// don't get any space if we're running on the robot
		debugSpace[i] = NULL;
#endif
		// Construct the lightweight debug images that know where the space is
		if (i == 0) {
			debugImage[i] = new DebugImage(wd, ht, debugSpace[0]);
			debugImage[i]->reset();
		} else {
			debugImage[i] = new DebugImage(wd / 2, ht / 2, debugSpace[1]);
			debugImage[i]->reset();
		}

		if (i == 0) {
			field = new Field(wd / 2, ht / 2, homography[0]);
		}

        ballDetector[i] = new BallDetector(homography[i], field, i == 0);
        boxDetector[i] = new GoalboxDetector();
        centerCircleDetector[i] = new CenterCircleDetector();

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
#ifdef OFFLINE
	// Here is an example of how to get access to the debug space. In this case the
	// field class only runs on the top image so it only needs that one
	field->setDebugImage(debugImage[0]);
#endif

    // Retreive calibration params for the robot name specified in the constructor
    setCalibrationParams(robotName);

    robotImageObstacle = new RobotObstacle(wd / 4, ht / 4);
}

VisionModule::~VisionModule()
{
    for (int i = 0; i < 2; i++) {
        delete colorParams[i];
        delete frontEnd[i];
        delete edgeDetector[i];
        delete edges[i];
        delete rejectedEdges[i];
        delete houghLines[i];
        delete hough[i];
        delete calibrationParams[i];
        delete kinematics[i];
        delete homography[i];
        delete fieldLines[i];
		delete debugImage[i];
		//delete debugSpace[i];
        delete boxDetector[i];
        delete cornerDetector[i];
        delete centerCircleDetector[i];
        delete ballDetector[i];
    }
	delete field;
}

    int overrun = 0;
// TODO use horizon on top image
void VisionModule::run_()
{
    PROF_ENTER(P_VISION)
    // Get messages from inPortals
    topIn.latch();
    bottomIn.latch();
    jointsIn.latch();
    inertsIn.latch();

    // Setup
    std::vector<const messages::YUVImage*> images { &topIn.message(),
                                                    &bottomIn.message() };

    bool ballDetected = false;

    // Time vision module
    double topTimes[12];
    double bottomTimes[12];
    double* times[2] = { topTimes, bottomTimes };

    // Loop over top and bottom image and run line detection system
    for (int i = 0; i < images.size(); i++) {
        PROF_ENTER2(P_VISION_TOP, P_VISION_BOT, i==0)
        // Get image
        const messages::YUVImage* image = images[i];

        // Construct YuvLite object for use in vision system
        YuvLite yuvLite(image->width() / 4,
                        image->height() / 2,
                        image->rowPitch(),
                        image->pixelAddress(0, 0));


        HighResTimer timer;

/* The color image in the Front End, built from the color table, is
 * optional. When we run the tool offline, we assume that we have
 * the color image, which causes errors when accessing other images.
 * For reference, this is in vision_defs, part of nbcross, where we
 * run our instance of the vision module. For the time being,
 * we are creating a fake color table when running offline
 * so that our code will not crash in other places - namely accessing
 * other images / information from the vision func of nbcross.
 * This should later be fixed so that the "segmented image" is always
 * the last part of the SExpr built in the vision func. This will
 * affect how many views of the tool access their data. */
#ifdef OFFLINE
        uint8_t* fakeColorTableBytes = new uint8_t[1<<21];
#endif

        // Run front end
        PROF_ENTER2(P_FRONT_TOP, P_FRONT_BOT, i==0)
#ifdef OFFLINE
        frontEnd[i]->run(yuvLite, colorParams[i], fakeColorTableBytes);
#else
        frontEnd[i]->run(yuvLite, colorParams[i]);
#endif
        PROF_EXIT2(P_FRONT_TOP, P_FRONT_BOT, i==0)
        ImageLiteU16 yImage(frontEnd[i]->yImage());
        ImageLiteU8 whiteImage(frontEnd[i]->whiteImage());
        ImageLiteU8 greenImage(frontEnd[i]->greenImage());
        ImageLiteU8 orangeImage(frontEnd[i]->orangeImage());

/* Delete these fake bytes after we've run the front end so that we
 * don't have a memory leak. */
#ifdef OFFLINE
        delete[] fakeColorTableBytes;
#endif

        times[i][0] = timer.end();

        // Offset to hackily adjust tilt for high-azimuth error
        double azOffset = 0;
        if (name != "ringo")
            azOffset = azimuth_m * fabs(kinematics[i]->azimuth()) + azimuth_b;

        // Calculate kinematics and adjust homography
        if (jointsIn.message().has_head_yaw()) {
            kinematics[i]->joints(jointsIn.message());
            homography[i]->wx0(kinematics[i]->wx0());
            homography[i]->wy0(kinematics[i]->wy0());
            homography[i]->wz0(kinematics[i]->wz0());
            homography[i]->roll(calibrationParams[i]->getRoll());

            homography[i]->tilt(kinematics[i]->tilt() + calibrationParams[i]->getTilt() + azOffset);
#ifndef OFFLINE
            homography[i]->azimuth(kinematics[i]->azimuth());
#endif
        }

        times[i][1] = timer.end();


        // Approximate brightness gradient
        PROF_ENTER2(P_GRAD_TOP, P_GRAD_BOT, i==0)
        edgeDetector[i]->gradient(yImage);
        PROF_EXIT2(P_GRAD_TOP, P_GRAD_BOT, i==0)

        times[i][2] = timer.end();

		// only calculate the field in the top camera
        PROF_ENTER2(P_FIELD_TOP, P_FIELD_BOT, i==0)
		if (!i) {
			// field needs the color images
			field->setImages(frontEnd[0]->whiteImage(), frontEnd[0]->greenImage(),
							 frontEnd[0]->orangeImage());
			GeoLine horizon = homography[0]->horizon(image->width() / 2);
			double x1, x2, y1, y2;
			horizon.endPoints(x1, y1, x2, y2);
			int hor = static_cast<int>(y1);
			hor = image->height() / 4 - hor;
			int hor2 = static_cast<int>(y2);
			hor2 = image->height() / 4 - hor2;
			field->findGreenHorizon(hor, hor2);
		}
        PROF_EXIT2(P_FIELD_TOP, P_FIELD_BOT, i==0)

        times[i][3] = timer.end();

        // Run edge detection
        PROF_ENTER2(P_EDGE_TOP, P_EDGE_BOT, i==0)
        edgeDetector[i]->edgeDetect(greenImage, *(edges[i]));
        PROF_EXIT2(P_EDGE_TOP, P_EDGE_BOT, i==0)
        times[i][4] = timer.end();

        // Run hough line detection
        PROF_ENTER2(P_HOUGH_TOP, P_HOUGH_BOT, i==0)
        hough[i]->run(*(edges[i]), *(rejectedEdges[i]), *(houghLines[i]));
        PROF_EXIT2(P_HOUGH_TOP, P_HOUGH_BOT, i==0)
        times[i][5] = timer.end();

        // Find world coordinates for hough lines
        houghLines[i]->mapToField(*(homography[i]), *field);
        times[i][6] = timer.end();

        // Find world coordinates for rejected edges
        PROF_ENTER2(P_EDGEMAP_TOP, P_EDGEMAP_BOT, i==0)
        rejectedEdges[i]->mapToField(*(homography[i]));
        PROF_EXIT2(P_EDGEMAP_TOP, P_EDGEMAP_BOT, i==0)
        times[i][7] = timer.end();

        // Detect center circle on top
        PROF_ENTER2(P_CIRCLE_TOP, P_CIRCLE_BOT, i==0)
        if (!i) centerCircleDetector[i]->detectCenterCircle(*(rejectedEdges[i]), *field);
        PROF_EXIT2(P_CIRCLE_TOP, P_CIRCLE_BOT, i==0)
        times[i][8] = timer.end();

        // Pair hough lines to field lines
        PROF_ENTER2(P_LINES_TOP, P_LINES_BOT, i==0)
        fieldLines[i]->find(*(houghLines[i]), blackStar());
        PROF_EXIT2(P_LINES_TOP, P_LINES_BOT, i==0)
        times[i][9] = timer.end();

        // Classify field lines
        PROF_ENTER2(P_LINECLASS_TOP, P_LINECLASS_BOT, i==0)
        fieldLines[i]->classify(*(boxDetector[i]), *(cornerDetector[i]), *(centerCircleDetector[i]));
        PROF_EXIT2(P_LINECLASS_TOP, P_LINECLASS_BOT, i==0)
        times[i][10] = timer.end();

        PROF_ENTER2(P_BALL_TOP, P_BALL_BOT, i==0)
        ballDetected |= ballDetector[i]->findBall(orangeImage, kinematics[i]->wz0());
        PROF_EXIT2(P_BALL_TOP, P_BALL_BOT, i==0)
        times[i][11] = timer.end();

        PROF_EXIT2(P_VISION_TOP, P_VISION_BOT, i==0)
#ifdef USE_LOGGING
        logImage(i);
#endif
    }
    double topTotal;
    double botTotal;

    for (int i = 0; i < 2; i++) {
        if (i == 0) {
            topTotal = (times[i][0] + times[i][1] + times[i][2] + times[i][3] +
                        times[i][4] + times[i][5] + times[i][6] + times[i][7] +
                        times[i][8] + times[i][9] + times[i][10] + times[i][11]);
        } else {
            botTotal = (times[i][0] + times[i][1] + times[i][2] + times[i][3] +
                        times[i][4] + times[i][5] + times[i][6] + times[i][7] +
                        times[i][8] + times[i][9] + times[i][10] + times[i][11]);
        }
    }

    if (topTotal + botTotal > 16.0 && false) {
        overrun++;
        for (int i = 0; i < 2; i++) {
            if (i == 0) {
                std::cout << "From top camera:" << std::endl;
            } else {
                std::cout << std::endl << "From bottom camera:" << std::endl;
            }
            std::cout << "Front end:      " << times[i][0] << std::endl;
            std::cout << "Homography:     " << times[i][1] << std::endl;
            std::cout << "Gradient:       " << times[i][2] << std::endl;
            std::cout << "Field:          " << times[i][3] << std::endl;
            std::cout << "Edge detection: " << times[i][4] << std::endl;
            std::cout << "Hough:          " << times[i][5] << std::endl;
            std::cout << "Hough to world: " << times[i][6] << std::endl;
            std::cout << "Edges to world: " << times[i][7] << std::endl;
            std::cout << "CenterCircle:   " << times[i][8] << std::endl;
            std::cout << "Field lines:    " << times[i][9] << std::endl;
            std::cout << "FL classify:    " << times[i][10] << std::endl;
            std::cout << "Ball:           " << times[i][11] << std::endl;
            std::cout << "Total:          " << (!i ? topTotal : botTotal) <<std::endl;
        }
        std::cout << std::endl << "TOTAL:          " << topTotal + botTotal << " " <<
        edges[0]->count() << " edges in top. " << overrun <<
        " total overruns" << std::endl << std::endl;
    }
    


    // Send messages on outportals
    ballOn = ballDetected;
    
    outportalVisionField();

    PROF_ENTER(P_OBSTACLE)
    updateObstacleBox();

    PROF_EXIT(P_OBSTACLE)

    PROF_EXIT(P_VISION);
}

void VisionModule::outportalVisionField()
{
    // Mark repeat lines (already found in bottom camera) in top camera
    for (int i = 0; i < fieldLines[0]->size(); i++) {
        for (int j = 0; j < fieldLines[1]->size(); j++) {
            FieldLine& topField = (*(fieldLines[0]))[i];
            FieldLine& botField = (*(fieldLines[1]))[j];
            for (int k = 0; k < 2; k++) {
                const GeoLine& topGeo = topField[k].field();
                const GeoLine& botGeo = botField[k].field();
                if (topGeo.error(botGeo) < 0.001) // TODO constant
                    (*(fieldLines[0]))[i].repeat(true);
            }
        }
    }
    // Outportal results
    // NOTE repeats are not outportaled
    messages::Vision visionField;

    // (1) Outportal lines
    // NOTE repeats (in top and bottom camera) are outportaled
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < fieldLines[i]->size(); j++) {
            FieldLine& line = (*(fieldLines[i]))[j];
            if (line.repeat()) continue;
            messages::FieldLine* pLine = visionField.add_line();

            for (int k = 0; k < 2; k++) {
                messages::HoughLine pHough;
                HoughLine& hough = line[k];

                // Rotate to post vision relative robot coordinate system
                GeoLine rotated;
                rotated.set(hough.field().r(), hough.field().t(), hough.field().ep0(), hough.field().ep1());
                rotated.translateRotate(0, 0, -(M_PI / 2));

                pHough.set_r(rotated.r());
                pHough.set_t(rotated.t());
                pHough.set_ep0(rotated.ep0());
                pHough.set_ep1(rotated.ep1());

                if (hough.field().r() < 0)
                    pLine->mutable_outer()->CopyFrom(pHough);
                else
                    pLine->mutable_inner()->CopyFrom(pHough);
            }

            pLine->set_id(static_cast<int>(line.id()));
            pLine->set_index(static_cast<int>(line.index()));
            pLine->set_wz0(homography[i]->wz0());
        }
    }

    // (2) Outportal Corners
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < cornerDetector[i]->size(); j++) {
            messages::Corner* pCorner = visionField.add_corner();
            Corner& corner = (*(cornerDetector[i]))[j];

            // Rotate to post vision relative robot coordinate system
            double rotatedX, rotatedY;
            man::vision::translateRotate(corner.x, corner.y, 0, 0, -(M_PI / 2), rotatedX, rotatedY);

            pCorner->set_x((float)rotatedX);
            pCorner->set_y((float)rotatedY);
            pCorner->set_id(static_cast<int>(corner.id));
            pCorner->set_line1(static_cast<int>(corner.first->index()));
            pCorner->set_line2(static_cast<int>(corner.second->index()));
            pCorner->set_wz0(homography[i]->wz0());
        }
    }

    // (3) Outportal Center Circle
    messages::CenterCircle* cc = visionField.mutable_circle(); 
    cc->set_on(centerCircleDetector[0]->on());
    cc->set_wz0(homography[0]->wz0());

    // Rotate to post vision relative robot coordinate system
    double rotatedX, rotatedY;
    man::vision::translateRotate(centerCircleDetector[0]->x(), centerCircleDetector[0]->y(), 0, 0, -(M_PI / 2), rotatedX, rotatedY);
    cc->set_x(rotatedX);
    cc->set_y(rotatedY);

    // (4) Outportal Ball
    messages::VBall* vb = visionField.mutable_ball();

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

    vb->set_on(ballOn);
    vb->set_frames_on(ballOnCount);
    vb->set_frames_off(ballOffCount);
    vb->set_intopcam(top);
    vb->set_wz0(homography[!top]->wz0());

    if (ballOn)
    {
        vb->set_distance(best.dist);

        vb->set_radius(best.blob.firstPrincipalLength());
        double bearing = atan(best.x_rel / best.y_rel);
        vb->set_bearing(bearing);
        vb->set_bearing_deg(bearing * TO_DEG);

        double angle_x = (best.imgWidth/2 - best.getBlob().centerX()) /
            (best.imgWidth) * HORIZ_FOV_DEG;
        double angle_y = (best.imgHeight/2 - best.getBlob().centerY()) /
            (best.imgHeight) * VERT_FOV_DEG;
        vb->set_angle_x_deg(angle_x);
        vb->set_angle_y_deg(angle_y);

        vb->set_confidence(best.confidence());
        vb->set_x(static_cast<int>(best.blob.centerX()));
        vb->set_y(static_cast<int>(best.blob.centerY()));
    }

    visionField.set_horizon_dist(field->horizonDist());

    // Send
    portals::Message<messages::Vision> visionOutMessage(&visionField);
    visionOut.setMessage(visionOutMessage);
}

void VisionModule::updateObstacleBox()
{
    // only want bottom camera
    robotImageObstacle->updateVisionObstacle(frontEnd[1]->whiteImage(),
                                             *(edges[1]), obstacleBox,
                                             homography[1]);

    // std::cout<<"about to set message for obstacle vision"<<std::endl;
    portals::Message<messages::RobotObstacle> boxOut(0);
    boxOut.get()->set_closest_y(obstacleBox[0]);
    boxOut.get()->set_box_bottom(obstacleBox[1]);
    boxOut.get()->set_box_left(obstacleBox[2]);
    boxOut.get()->set_box_right(obstacleBox[3]);
    robotObstacleOut.setMessage(boxOut);

    // printf("Obstacle Box VISION: (%g, %g, %g, %g)\n", obstacleBox[0],
    //         obstacleBox[1], obstacleBox[2], obstacleBox[3]);
}

void VisionModule::setColorParams(Colors* colors, bool topCamera)
{ 
    delete colorParams[!topCamera];
    colorParams[!topCamera] = colors;
}

const std::string VisionModule::getStringFromTxtFile(std::string path) 
{
    std::ifstream textFile;
    textFile.open(path);

    // Get size of file
    textFile.seekg (0, textFile.end);
    long size = (long)textFile.tellg();
    textFile.seekg(0);

    // Read file into buffer and convert to string
    char* buff = new char[size];
    textFile.read(buff, size);
    std::string sexpText(buff);

    textFile.close();
    return (const std::string)sexpText;
}

#ifdef OFFLINE
	void VisionModule::setDebugDrawingParameters(nblog::SExpr* params) {
		std::cout << "In debug drawing parameters" << params->print() << std::endl;
		int cameraHorizon = params->get(1)->find("CameraHorizon")->get(1)->valueAsInt();
		int fieldHorizon = params->get(1)->find("FieldHorizon")->get(1)->valueAsInt();
		int debugHorizon = params->get(1)->find("DebugHorizon")->get(1)->valueAsInt();
		int debugField = params->get(1)->find("DebugField")->get(1)->valueAsInt();
		int debugBall = params->get(1)->find("DebugBall")->get(1)->valueAsInt();
		field->setDrawCameraHorizon(cameraHorizon);
		field->setDrawFieldHorizon(fieldHorizon);
		field->setDebugHorizon(debugHorizon);
		field->setDebugFieldEdge(debugField);
		ballDetector[0]->setDebugBall(debugBall);
		ballDetector[1]->setDebugBall(debugBall);
		debugImage[0]->reset();
		debugImage[1]->reset();
	}
#endif

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

void VisionModule::setCalibrationParams(std::string robotName) 
{
    setCalibrationParams(0, robotName);
    setCalibrationParams(1, robotName);
}

void VisionModule::setCalibrationParams(int camera, std::string robotName) 
{
    if (std::string::npos != robotName.find(".local")) {
        robotName.resize(robotName.find("."));
        //Much love for the edge cases.
        if (robotName == "she-hulk")
            robotName = "shehulk";
    }

    // Set local private variable
    name = robotName;

    if (robotName == "") {
        return;
    }

    nblog::SExpr* robot = calibrationLisp->get(1)->find(robotName);

    if (robot != NULL) {
        std::string cam = camera == 0 ? "TOP" : "BOT";
        double roll =  robot->find(cam)->get(1)->valueAsDouble();
        double tilt = robot->find(cam)->get(2)->valueAsDouble();
        delete calibrationParams[camera];
        calibrationParams[camera] = new CalibrationParams(roll, tilt);

        std::cerr << "Found and set calibration params for " << robotName;
        std::cerr << "Roll: " << roll << " Tilt: " << tilt << std::endl;
    }
}

void VisionModule::setCalibrationParams(CalibrationParams* params, bool topCamera)
{
    delete calibrationParams[!topCamera];
    calibrationParams[!topCamera] = params;
}

#ifdef USE_LOGGING
void VisionModule::logImage(int i) 
{
    bool blackStar = false;

    if (getenv("LOG_THIS") != NULL) {
        if (strcmp(getenv("LOG_THIS"), std::string("top").c_str()) == 0) {
            if (i != 0)
                return;
            else {
                setenv("LOG_THIS", "false", 1);
                blackStar = true;
                std::cerr << "pCal logging top log\n";
            }
        } else if (strcmp(getenv("LOG_THIS"), std::string("bottom").c_str()) == 0) {   
            if (i != 1)
                return;
            else {
                setenv("LOG_THIS", "false", 1);
                blackStar = true;
                std::cerr << "pCal logging bot log\n";
            }
        } else 
            return;
    }

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
        if (blackStar)
            cal.append(nblog::SExpr("BlackStar"));
        contents.push_back(cal);

        nblog::NBLog(NBL_IMAGE_BUFFER, "tripoint", contents, im_buf);
    }
}
#endif

}
}
