#include "nbfuncs.h"

#include "RoboGrams.h"
#include "Images.h"
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

using nblog::Log;
using nblog::SExpr;

//man::vision::Colors* getColorsFromSExpr(SExpr* params);
void getCalibrationOffsets(Log* l, double* r, double* p, int w, int h, bool t);
void updateSavedColorParams(std::string sexpPath, SExpr* params, bool top);
SExpr getSExprFromSavedParams(int color, std::string sexpPath, bool top);
std::string getSExprStringFromColorJSonNode(boost::property_tree::ptree tree);
SExpr treeFromBall(man::vision::Ball& b);
SExpr treeFromBlob(man::vision::Blob& b);

messages::YUVImage emptyTop(
    man::vision::DEFAULT_TOP_IMAGE_WIDTH * 2,
    man::vision::DEFAULT_TOP_IMAGE_HEIGHT
);

messages::YUVImage emptyBot(
    man::vision::DEFAULT_TOP_IMAGE_WIDTH,
    man::vision::DEFAULT_TOP_IMAGE_HEIGHT / 2
);

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
man::vision::VisionModule& getModuleRef(const std::string robotName);

int Vision_func() {

    

int CameraCalibration_func() {
    printf("CameraCalibrate_func()\n");

    int failures = 0;
    double totalR = 0;
    double totalT = 0;
    
    man::vision::VisionModule& module = getModuleRef("");

    // Repeat for each log
    for (int i = 0; i < 7; i++) {
        module.reset();
        
        Log* l = new Log(args[i]);

        size_t length = l->data().size();
        uint8_t buf[length];
        memcpy(buf, l->data().data(), length);

        // Determine description
        bool top = l->description().find("camera_TOP") != std::string::npos;
        
        int width = 2*atoi(l->tree().find("contents")->get(1)->
                                        find("width")->get(1)->value().c_str());
        int height = atoi(l->tree().find("contents")->get(1)->
                                       find("height")->get(1)->value().c_str());

        imageSizeCheck(top, width, height);
        
        double rollChange, pitchChange;

        // Read number of bytes of image, inertials, and joints if exist
        int numBytes[3];
        std::vector<SExpr*> vec = l->tree().recursiveFind("YUVImage");
        if (vec.size() != 0) {
            SExpr* s = vec.at(vec.size()-2)->find("nbytes");
            if (s != NULL) {
                numBytes[0] = s->get(1)->valueAsInt();
            }
        }

        vec = l->tree().recursiveFind("InertialState");
        if (vec.size() != 0) {
            SExpr* s = vec.at(vec.size()-2)->find("nbytes");
            if (s != NULL) {
                numBytes[1] = s->get(1)->valueAsInt();
            }
        }

        messages::JointAngles joints;
        vec = l->tree().recursiveFind("JointAngles");
        if (vec.size() != 0) {
            SExpr* s = vec.at(vec.size()-2)->find("nbytes");
            if (s != NULL) {
                numBytes[2] = s->get(1)->valueAsInt();
                uint8_t* ptToJoints = buf + (numBytes[0] + numBytes[1]);
                joints.ParseFromArray((void *) ptToJoints, numBytes[2]);
            } else {
                std::cout << "Could not load joints from description.\n";
                rets.push_back(new Log("((failure))"));
                return 0;
            }
        }

        // If log includes "BlackStar," set flag
        std::vector<SExpr*> blackStarVec = args[0]->tree().recursiveFind("BlackStar");
        if (blackStarVec.size() != 0) {
            module.blackStar(true);
            std::cout << "\nBLACK STAR TRUE!!!\n\n";
        } else std::cout << "\nBLACK STAR FALSE\n\n";
        
        
        // Create messages
        messages::YUVImage image(buf, width, height, width);
        portals::Message<messages::YUVImage> imageMessage(&image);
        portals::Message<messages::JointAngles> jointsMessage(&joints);

        if (top) {
            portals::Message<messages::YUVImage> emptyMessage(&emptyBot);
            module.topIn.setMessage(imageMessage);
            module.bottomIn.setMessage(emptyMessage);
        } else {
            portals::Message<messages::YUVImage> emptyMessage(&emptyTop);
            module.topIn.setMessage(emptyMessage);
            module.bottomIn.setMessage(imageMessage);
        }
        
        module.jointsIn.setMessage(jointsMessage);

        module.run();

        man::vision::FieldHomography* fh = module.getFieldHomography(top);

        double rollBefore, tiltBefore, rollAfter, tiltAfter;

        rollBefore = fh->roll();
        tiltBefore = fh->tilt();

        bool success = fh->calibrateFromStar(*module.getFieldLines(top));

        if (!success) {
            failures++;
        } else {
            rollAfter = fh->roll();
            tiltAfter = fh->tilt();
            totalR += rollAfter - rollBefore;
            totalT += tiltAfter - tiltBefore;
        }
    }

    if (failures > 4) {
        // Handle failure
        printf("FAILED: %d times\n", failures);
        rets.push_back(new Log("(failure)"));
    } else {
        totalR /= (args.size() - failures);
        totalT /= (args.size() - failures);

        // Pass back averaged offsets to Tool
        std::string sexp = "((roll " + std::to_string(totalR) + ")(tilt " + std::to_string(totalT) + "))";
        rets.push_back(new Log(sexp));
    }
}


int Synthetics_func() {
    assert(args.size() == 1);

    printf("Synthetics_func()\n");
    
    double x = std::stod(args[0]->tree().find("contents")->get(1)->find("params")->get(1)->value().c_str());
    double y = std::stod(args[0]->tree().find("contents")->get(1)->find("params")->get(2)->value().c_str());
    double h = std::stod(args[0]->tree().find("contents")->get(1)->find("params")->get(3)->value().c_str());
    bool fullres = args[0]->tree().find("contents")->get(1)->find("params")->get(4)->value() == "true";
    bool top = args[0]->tree().find("contents")->get(1)->find("params")->get(5)->value() == "true";

    int wd = (fullres ? 320 : 160);
    int ht = (fullres ? 240 : 120);
    double flen = (fullres ? 544 : 272);

    int size = wd*4*ht*2;
    uint8_t pixels[size];
    man::vision::YuvLite synthetic(wd, ht, wd*4, pixels);

    man::vision::FieldHomography homography(top);
    homography.wx0(x);
    homography.wy0(y);
    homography.azimuth(h*TO_RAD);
    homography.flen(flen);

    man::vision::syntheticField(synthetic, homography);

    std::string sexpr("(nblog (version 6) (contents ((type YUVImage) ");
    sexpr += top ? "(from camera_TOP) " : "(from camera_BOT) ";
    sexpr += "(nbytes ";
    sexpr += std::to_string(size);
    sexpr += ") (width " + std::to_string(2*wd);
    sexpr += ") (height " + std::to_string(2*ht);
    sexpr += ") (encoding \"[Y8(U8/V8)]\"))))";

    Log* log = new Log(sexpr);
    std::string buf((const char*)pixels, size);
    log->setData(buf);

    rets.push_back(log);
}

// Save the new color params to the colorParams.txt file


int Scratch_func() {}

#include <map>
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
