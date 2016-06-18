#include "Helpers.hpp"

/* HELPER FUNCTIONS */

SExpr treeFromSpot(man::vision::Spot & b, int width, int height)
{
    SExpr xLo(b.xLo() + width / 2);
    SExpr xHi(b.xHi() + width / 2);
    SExpr yLo(b.yLo() + height / 2);
    SExpr yHi(b.yHi() + height / 2);

    SExpr x(b.rawX);
    SExpr y(b.rawY);
    SExpr p = SExpr::list({x, y});
    SExpr ul = SExpr::list({xLo, yHi});
    SExpr lr = SExpr::list({xHi, yLo});

    SExpr center = SExpr::keyValue("center", p);
    SExpr topleft = SExpr::keyValue("topLeft", ul);
    SExpr lowerright = SExpr::keyValue("lowerRight", lr);
    SExpr innerdiam = SExpr::keyValue("inner", b.innerDiam);
    SExpr outerdiam = SExpr::keyValue("outer", b.outerDiam);
    SExpr spottype = SExpr::keyValue("spottype", b.spotType);
    SExpr toRet = SExpr::list({center, topleft, lowerright, innerdiam, outerdiam,
        spottype});

    return toRet;
}

SExpr treeFromBall(man::vision::Ball& b, int width, int height)
{
    SExpr x(b.x_rel);
    SExpr y(b.y_rel);
    SExpr p = SExpr::list({x, y});
    SExpr bl = treeFromSpot(b.getSpot(), width, height);

    SExpr rel = SExpr::keyValue("rel", p);
    SExpr spot = SExpr::keyValue("blob", bl);
    SExpr exDiam = SExpr::keyValue("expectedDiam", b.expectedDiam);
    SExpr toRet = SExpr::list({rel, spot, exDiam});

    return toRet;
}


SExpr treeFromBlob(man::vision::Blob& b)
{
    SExpr x(b.centerX());
    SExpr y(b.centerY());
    SExpr p = SExpr::list({x, y});

    SExpr center = SExpr::keyValue("center", p);
    SExpr area = SExpr::keyValue("area", b.area());
    SExpr count = SExpr::keyValue("count", b.count());
    SExpr len1 = SExpr::keyValue("len1", b.firstPrincipalLength());
    SExpr len2 = SExpr::keyValue("len2", b.secondPrincipalLength());
    SExpr ang1 = SExpr::keyValue("ang1", b.firstPrincipalAngle());
    SExpr ang2 = SExpr::keyValue("ang2", b.secondPrincipalAngle());
    SExpr toRet = SExpr::list({center, area, count, len1, len2, ang1, ang2});

    return toRet;
}

SExpr treeFromRobot(man::vision::Robot& rob)
{
    SExpr left = SExpr::keyValue("left", rob.left);
    SExpr right = SExpr::keyValue("right", rob.right);
    SExpr top = SExpr::keyValue("top", rob.top);
    SExpr bottom = SExpr::keyValue("bottom", rob.bottom);
    SExpr returnList = SExpr::list({left, right, top, bottom});
    return returnList;
}

void imageSizeCheck(bool top, int width, int height) {
    if (top) {
        if (width != 2 * man::vision::DEFAULT_TOP_IMAGE_WIDTH ||
            height != man::vision::DEFAULT_TOP_IMAGE_HEIGHT ) {
            printf("WARNING! topCamera dimensions (%i, %i) NOT DEFAULT, VisionModule results undefined!\n",
                   width, height);
        }
    } else {
        //bot
        if ( // 2 / 2 == 1
            width != man::vision::DEFAULT_TOP_IMAGE_WIDTH ||
            height != ( man::vision::DEFAULT_TOP_IMAGE_HEIGHT / 2 ) ) {
            printf("WARNING! botCamera dimensions (%i, %i) NOT DEFAULT, VisionModule results undefined!\n",
                   width, height);
        }
    }
}

//robotName may be empty ("").
std::map<const std::string, man::vision::VisionModule *> vmRefMap;

man::vision::VisionModule& getModuleRef(const std::string robotName) {
    if (vmRefMap.find(robotName) != vmRefMap.end()) {
        printf("nbcross-getModuleRef REUSING MODULE [%s]\n",
               robotName.c_str() );
        man::vision::VisionModule* module = vmRefMap[robotName];
        module->reset();
        return *module;

    } else {
        printf("nbcross-getModuleRef CREATING NEW MODULE [%s]\n",
               robotName.c_str() );
        man::vision::VisionModule * newInst =
        new man::vision::VisionModule(man::vision::DEFAULT_TOP_IMAGE_WIDTH,
                                      man::vision::DEFAULT_TOP_IMAGE_HEIGHT, robotName);
        vmRefMap[robotName] = newInst;
        return *newInst;
    }
}

void bumpLineFitThreshold(double newVal) {
    NBL_WARN("vision line fit threshold being moved to value: %lf !", newVal)
    man::vision::FIT_THRESH_START = newVal;
}

