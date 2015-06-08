#include "nbfuncs.h"

#include "RoboGrams.h"
#include "Images.h"
#include "vision/VisionModule.h"
#include "vision/FrontEnd.h"
#include "ParamReader.h"

#include <netinet/in.h>
#include <assert.h>
#include <vector>
#include <string>
#include <iostream>

using nblog::Log;
using nblog::SExpr;

man::vision::Colors* getColorsFromSliderValues(float* params);
void updateSavedColorParams(std::string jsonPath, float* params, bool top);
SExpr getSExprFromSavedParams(int color, std::string jsonPath, bool top);
std::string getSExprStringFromColorJSonNode(boost::property_tree::ptree tree);

int Vision_func() {
    assert(args.size() > 0);

    printf("Vision_func()\n");

    Log* copy = new Log(args[0]);
    size_t length = copy->data().size();
    uint8_t buf[length];
    memcpy(buf, copy->data().data(), length);

    bool top;
    top = copy->description().find("from camera_TOP") != std::string::npos;

    int width = 2*640;
    int height = 480;

    std::string jsonPath = "/home/evanhoyt/Desktop/ColorParams.json";
    
    messages::YUVImage image(buf, width, height, width);
    messages::JointAngles emptyJoints;
    messages::InertialState emptyInertials;

    portals::Message<messages::YUVImage> imageMessage(&image);
    portals::Message<messages::JointAngles> emptyJointsMessage(&emptyJoints);
    portals::Message<messages::InertialState> emptyInertialsMessage(&emptyInertials);

    // Init module to color params from JSON file
    man::vision::VisionModule module;

    module.topIn.setMessage(imageMessage);
    module.bottomIn.setMessage(imageMessage);
    module.jointsIn.setMessage(emptyJointsMessage);
    module.inertialsIn.setMessage(emptyInertialsMessage);

    // If log included colors in description, update color params, but leave json data
    if (args.size() > 1) {
        man::vision::Colors* newParams = getColorsFromSliderValues((float*)args[1]);
        module.setColorParams(newParams, top);
    }

    // If log says to save, save current params to json file
    if (args.size() > 2) {
        updateSavedColorParams(jsonPath, (float*)args[1], top);
    }

    man::vision::ImageFrontEnd* frontEnd = module.runAndGetFrontEnd(top);

    // -----------
    //   Y IMAGE
    // -----------

    Log* yRet = new Log();
    int yLength = 240*320*2;

    // Create temp buffer and fill with yImage from FrontEnd
    uint8_t yBuf[yLength];
    memcpy(yBuf, frontEnd->yImage().pixelAddr(), yLength);

    // Convert to string and set log
    std::string yBuffer((const char*)yBuf, yLength);
    yRet->setData(yBuffer);

    rets.push_back(yRet);

    // ---------------
    //   WHITE IMAGE
    // ---------------
    Log* whiteRet = new Log();
    int whiteLength = 240*320;;

    // Create temp buffer and fill with white image 
    uint8_t whiteBuf[whiteLength];
    memcpy(whiteBuf, frontEnd->whiteImage().pixelAddr(), whiteLength);

    // Convert to string and set log
    std::string whiteBuffer((const char*)whiteBuf, whiteLength);
    whiteRet->setData(whiteBuffer);

    // Read params from JSon and attach to image 
    whiteRet->setTree(getSExprFromSavedParams(0, jsonPath, top));

    rets.push_back(whiteRet);

    // ---------------
    //   GREEN IMAGE
    // ---------------
    Log* greenRet = new Log();
    int greenLength = 240*320;

    // Create temp buffer and fill with gree image 
    uint8_t greenBuf[greenLength];
    memcpy(greenBuf, frontEnd->greenImage().pixelAddr(), greenLength);

    // Convert to string and set log
    std::string greenBuffer((const char*)greenBuf, greenLength);
    greenRet->setData(greenBuffer);

    // Read params from JSon and attach to image 
    greenRet->setTree(getSExprFromSavedParams(1, jsonPath, top));

    rets.push_back(greenRet);

    // ----------------
    //   ORANGE IMAGE
    // ----------------
    Log* orangeRet = new Log();
    int orangeLength = 240*320;

    // Create temp buffer and fill with orange image 
    uint8_t orangeBuf[orangeLength];
    memcpy(orangeBuf, frontEnd->orangeImage().pixelAddr(), orangeLength);

    // Convert to string and set log
    std::string orangeBuffer((const char*)orangeBuf, orangeLength);
    orangeRet->setData(orangeBuffer);

    // Read params from JSon and attach to image 
    orangeRet->setTree(getSExprFromSavedParams(2, jsonPath, top));

    rets.push_back(orangeRet);

    //-------------------
    //  SEGMENTED IMAGE
    //-------------------
    Log* colorSegRet = new Log();
    int colorSegLength = 240*320;

    // Create temp buffer and fill with segmented image
    uint8_t segBuf[colorSegLength];
    memcpy(segBuf, frontEnd->colorImage().pixelAddr(), colorSegLength);

    // Convert to string and set log
    std::string segBuffer((const char*)segBuf, colorSegLength);
    colorSegRet->setData(segBuffer);

    rets.push_back(colorSegRet);
    
    //-------------------
    //  EDGES
    //-------------------
    man::vision::EdgeList* edgeList = module.getEdges();

    Log* edgeRet = new Log();
    std::string edgeBuf;

    man::vision::AngleBinsIterator<man::vision::Edge> abi(*edgeList);
    for (const man::vision::Edge* e = *abi; e; e = *++abi) {
        uint32_t x = htonl(e->x() + 160);
        edgeBuf.append((const char*) &x, sizeof(uint32_t));
        uint32_t y = htonl(-e->y() + 120);
        edgeBuf.append((const char*) &y, sizeof(uint32_t));
        uint32_t mag = htonl(e->mag());
        edgeBuf.append((const char*) &mag, sizeof(uint32_t));
        uint32_t angle = htonl(e->angle());
        edgeBuf.append((const char*) &angle, sizeof(uint32_t));
    }

    edgeRet->setData(edgeBuf);
    rets.push_back(edgeRet);

    //-------------------
    //  LINES
    //-------------------
    man::vision::FieldLineList* lineList = module.getFieldLines();

    Log* lineRet = new Log();
    std::string lineBuf;

    for (int i = 0; i < lineList->size(); i++) {
        for (int j = 0; j < 2; j++) {
            man::vision::HoughLine& line = (*lineList)[i].lines(j);
            double r = line.r();
            double t = line.t();
            std::cout << r << std::endl;
            std::cout << t << std::endl;
            double ep0 = line.ep0();
            double ep1 = line.ep1();
            
            // In Java, doubles are stored in big endian representation
            endswap<double>(&r);
            endswap<double>(&t);
            endswap<double>(&ep0);
            endswap<double>(&ep1);

            lineBuf.append((const char*) &r, sizeof(double));
            lineBuf.append((const char*) &t, sizeof(double));
            lineBuf.append((const char*) &ep0, sizeof(double));
            lineBuf.append((const char*) &ep1, sizeof(double));
        }
    }

    lineRet->setData(lineBuf);
    rets.push_back(lineRet);

    return 0;
}

man::vision::Colors* getColorsFromSliderValues(float* params) {
    man::vision::Colors* ret = new man::vision::Colors;
    ret->white. load(params[0], params[1], params[2],
                     params[3], params[4], params[5]);
    ret->green. load(params[6], params[7], params[8],
                     params[9], params[10],params[11]);
    ret->orange.load(params[12],params[13],params[14],
                     params[15],params[16],params[17]);
    return ret;
}

void updateSavedColorParams(std::string jsonPath, float* params, bool top) {

    // TODO make this work. Covert to string and write/replace

    boost::property_tree::ptree tree;
    boost::property_tree::read_json(jsonPath, tree);

    if (top) {
        tree = tree.get_child("colorParams.topColors");
    } else {
        tree = tree.get_child("colorParams.bottomColors");
    }

    for (int i = 0; i < 3; i++) {
        switch (i) {
            case 0 : tree = tree.get_child("white");
            case 1 : tree = tree.get_child("green");
            case 2 : tree = tree.get_child("orange");
        }

        tree.put<float>("darkU",  params[i*6 + 0]);
        tree.put<float>("darkV",  params[i*6 + 1]);
        tree.put<float>("lightU", params[i*6 + 2]);
        tree.put<float>("lightV", params[i*6 + 3]);
        tree.put<float>("fuzzyU", params[i*6 + 4]);
        tree.put<float>("fuzzyV", params[i*6 + 5]);
    }
}

SExpr getSExprFromSavedParams(int color, std::string jsonPath, bool top) {
//    std::string sexp = "(";
    boost::property_tree::ptree tree;
    boost::property_tree::read_json(jsonPath, tree);

    if (top) {
        tree = tree.get_child("colorParams.topColors");
    } else {
        tree = tree.get_child("colorParams.bottomColors");
    }

    if (color == 0) {               // White
        tree = tree.get_child("white");
    //    sexp.newAtom("WhiteParams");
    } else if (color == 1) {    // Green
        tree = tree.get_child("green");
    //    sexp.newAtom("GreenParams");
    } else {                    // Orange
        tree = tree.get_child("orange");
    //    sexp.newAtom("OrangeParams");
    }

    std::vector<SExpr> atoms = { 
        SExpr::atom("Params"),
        SExpr::keyValue("dark_u", (float)tree.get<float>("darkU")),
        SExpr::keyValue( "dark_v", (float)tree.get<float>("darkV")),
        SExpr::keyValue("light_u", (float)tree.get<float>("lightU")),
        SExpr::keyValue("light_v", (float)tree.get<float>("lightV")),
        SExpr::keyValue("fuzzy_u", (float)tree.get<float>("fuzzyU")),
        SExpr::keyValue("fuzzy_v", (float)tree.get<float>("fuzzyV"))
    };
    
    return SExpr(atoms);
}
