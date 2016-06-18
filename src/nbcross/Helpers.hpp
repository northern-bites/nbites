#ifndef NBCROSS_HELPER_H
#define NBCROSS_HELPER_H


#include "nbcross.hpp"

#include "RoboGrams.h"
#include "Images.h"
#include "vision/VisionCalibration.hpp"
#include "vision/VisionModule.h"
#include "vision/FrontEnd.h"
#include "vision/Homography.h"
#include "ParamReader.h"
#include "NBMath.h"

#include <cstdlib>
#include <netinet/in.h>
#include <assert.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <map>

using namespace nbl;

enum VisionColor {
    VISION_WHITE,
    VISION_GREEN,
    VISION_ORANGE
};

SExpr treeFromSpot(man::vision::Spot & b, int width, int height);

SExpr treeFromBlob(man::vision::Blob& b);

SExpr treeFromBall(man::vision::Ball& b, int width, int height);

SExpr treeFromRobot(man::vision::Robot& rob);

void imageSizeCheck(bool top, int width, int height);

man::vision::VisionModule& getModuleRef(const std::string robotName);

void bumpLineFitThreshold(double newVal = 0.70);

#endif //NBCROSS_HELPER_H