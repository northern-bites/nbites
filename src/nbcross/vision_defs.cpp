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
#include <fstream>

using nblog::Log;
using nblog::SExpr;

man::vision::Colors* getColorsFromSExpr(SExpr* params);
void updateSavedColorParams(std::string sexpPath, SExpr* params, bool top);
SExpr getSExprFromSavedParams(int color, std::string sexpPath, bool top);
std::string getSExprStringFromColorJSonNode(boost::property_tree::ptree tree);

int Vision_func() {
    assert(args.size() == 1);

    printf("Vision_func()\n");

    Log* copy = new Log(args[0]);
    size_t length = copy->data().size();
    uint8_t buf[length];
    memcpy(buf, copy->data().data(), length);

    // Determine if we are looking at a top or bottom image from log description
    bool top;
    top = copy->description().find("from camera_TOP") != std::string::npos;
    std::cout << top << std::endl;

    int width = 2*640;
    int height = 480;

    // Location of lisp text file with color params
    std::string sexpPath = std::string(getenv("NBITES_DIR"));
    sexpPath += "/src/man/config/colorParams.txt";
    
    // Empty messages to pass to Vision Module so it doesn't freak
    messages::YUVImage image(buf, width, height, width);
    messages::JointAngles emptyJoints;
    messages::InertialState emptyInertials;

    portals::Message<messages::YUVImage> imageMessage(&image);
    portals::Message<messages::JointAngles> emptyJointsMessage(&emptyJoints);
    portals::Message<messages::InertialState> emptyInertialsMessage(&emptyInertials);

    // VisionModule default constructor loads color params from Lisp in config/colorParms.txt
    man::vision::VisionModule module;

    module.topIn.setMessage(imageMessage);
    module.bottomIn.setMessage(imageMessage);
    module.jointsIn.setMessage(emptyJointsMessage);
    module.inertialsIn.setMessage(emptyInertialsMessage);
    
    // If log included color parameters in description, have module use those
    SExpr* params = args[0]->tree().find("Params");
    if (params != NULL) {

        // Set new parameters as frontEnd colorParams
        module.setColorParams(getColorsFromSExpr(params), top);

        // Look for atom value "SaveParams", i.e. "save" button press
        SExpr* save = params->get(1)->find("SaveParams");
        if (save != NULL) {

            // Save attached parameters to txt file
            updateSavedColorParams(sexpPath, params, top);
        }
    }

    // Run and retrieve front end from vision module
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

    // Read params from Lisp and attach to image 
    whiteRet->setTree(getSExprFromSavedParams(0, sexpPath, top));

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
    greenRet->setTree(getSExprFromSavedParams(1, sexpPath, top));

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
    orangeRet->setTree(getSExprFromSavedParams(2, sexpPath, top));

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

/* Helper function to convert from SExpr to Colors type.
    Use 18 parameters to intialize Colors struct.
 */
man::vision::Colors* getColorsFromSExpr(SExpr* params) {
    man::vision::Colors* ret = new man::vision::Colors;
    int i, j = 0;

    ret->white.load(std::stof(params->get(1)->get(j++ / 6)->get(1)->get(i++ % 6)->get(1)->serialize()),
                    std::stof(params->get(1)->get(j++ / 6)->get(1)->get(i++ % 6)->get(1)->serialize()),
                    std::stof(params->get(1)->get(j++ / 6)->get(1)->get(i++ % 6)->get(1)->serialize()),
                    std::stof(params->get(1)->get(j++ / 6)->get(1)->get(i++ % 6)->get(1)->serialize()),
                    std::stof(params->get(1)->get(j++ / 6)->get(1)->get(i++ % 6)->get(1)->serialize()),
                    std::stof(params->get(1)->get(j++ / 6)->get(1)->get(i++ % 6)->get(1)->serialize())); 

    ret->green.load(std::stof(params->get(1)->get(j++ / 6)->get(1)->get(i++ % 6)->get(1)->serialize()),
                    std::stof(params->get(1)->get(j++ / 6)->get(1)->get(i++ % 6)->get(1)->serialize()),
                    std::stof(params->get(1)->get(j++ / 6)->get(1)->get(i++ % 6)->get(1)->serialize()),
                    std::stof(params->get(1)->get(j++ / 6)->get(1)->get(i++ % 6)->get(1)->serialize()),
                    std::stof(params->get(1)->get(j++ / 6)->get(1)->get(i++ % 6)->get(1)->serialize()),
                    std::stof(params->get(1)->get(j++ / 6)->get(1)->get(i++ % 6)->get(1)->serialize()));  
 
   ret->orange.load(std::stof(params->get(1)->get(j++ / 6)->get(1)->get(i++ % 6)->get(1)->serialize()),
                    std::stof(params->get(1)->get(j++ / 6)->get(1)->get(i++ % 6)->get(1)->serialize()),
                    std::stof(params->get(1)->get(j++ / 6)->get(1)->get(i++ % 6)->get(1)->serialize()),
                    std::stof(params->get(1)->get(j++ / 6)->get(1)->get(i++ % 6)->get(1)->serialize()),
                    std::stof(params->get(1)->get(j++ / 6)->get(1)->get(i++ % 6)->get(1)->serialize()),
                    std::stof(params->get(1)->get(j++ / 6)->get(1)->get(i++ % 6)->get(1)->serialize()));

    return ret;
}

// Save the new color params to the colorParams.txt file
void updateSavedColorParams(std::string sexpPath, SExpr* params, bool top) {
    std::ifstream textFile;
    textFile.open(sexpPath);

    // Get size of file
    textFile.seekg (0, textFile.end);
    long size = textFile.tellg();
    textFile.seekg(0);
    
    // Read file into buffer and convert to string
    char* buff = new char[size];
    textFile.read(buff, size);
    std::string sexpText(buff);

    // Get SExpr from string
    SExpr* savedParams, * savedSExpr = SExpr::read((const std::string)sexpText);
    
    if (top) {
        savedParams = savedSExpr->get(1)->find("Top");
    } else {
        savedParams = savedSExpr->get(1)->find("Bottom");
    }

    // Remove "SaveParams True" pair from expression
    params->get(1)->remove(3);

    const std::vector<SExpr>& newParams = *params->get(1)->getList();
    savedParams->get(1)->setList(newParams);
       
    // Write out
    size = savedSExpr->print().length();
    char* buffer = new char[size + 1];
    std::strcpy(buffer, savedSExpr->print().c_str());
    std::ofstream out;
    out.open(sexpPath);
    out.write(buffer, savedSExpr->print().length());

    delete[] buff;
    delete[] buffer;
    textFile.close();
    out.close();
}

SExpr getSExprFromSavedParams(int color, std::string sexpPath, bool top) {
    std::ifstream textFile;
    textFile.open(sexpPath);

    // Get size of file
    textFile.seekg (0, textFile.end);
    long size = textFile.tellg();
    textFile.seekg(0);

    // Read file into buffer and convert to string
    char* buff = new char[size];
    textFile.read(buff, size);
    std::string sexpText(buff);

    // Get SExpr from string
    SExpr* savedSExpr = SExpr::read((const std::string)sexpText);

    // Point to required set of 6 params
    if (top)
        savedSExpr = savedSExpr->get(1)->find("Top");
    else 
        savedSExpr = savedSExpr->get(1)->find("Bottom");
    
    if (color == 0)                                         // White
        savedSExpr = savedSExpr->get(1)->find("White");
    else if (color == 1)                                    // Green
        savedSExpr = savedSExpr->get(1)->find("Green");
    else                                                    // Orange
        savedSExpr = savedSExpr->get(1)->find("Orange");    
    

    // Build SExpr from params
    std::vector<SExpr> atoms; 
    for (SExpr s : *(savedSExpr->getList()))
        atoms.push_back(s);

    return SExpr(atoms);
}

