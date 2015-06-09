#include "nbfuncs.h"

#include "RoboGrams.h"
#include "Images.h"
#include "vision/VisionModule.h"

#include <cstdlib>
#include <netinet/in.h>
#include <assert.h>
#include <vector>
#include <string>
#include <iostream>

using nblog::Log;
using nblog::SExpr;

int Vision_func() {
    assert(args.size() == 1);

    printf("Vision_func()\n");

    Log* copy = new Log(args[0]);
    size_t length = copy->data().size();
    uint8_t buf[length];
    memcpy(buf, copy->data().data(), length);

    // Parse YUVImage S-expression
    bool topCamera = copy->tree().find("contents")->get(1)->
                                  find("from")->get(1)->value() == "camera_TOP";
    int width = 2*atoi(copy->tree().find("contents")->get(1)->
                                        find("width")->get(1)->value().c_str());
    int height = atoi(copy->tree().find("contents")->get(1)->
                                        find("height")->get(1)->value().c_str());

    // Read number of bytes of image, inertials, and joints
    int numBytes[3];
    for (int i = 0; i < 3; i++)
        numBytes[i] = atoi(copy->tree().find("contents")->get(i+1)->
                                        find("nbytes")->get(1)->value().c_str());
    uint8_t* ptToJoints = buf + (numBytes[0] + numBytes[1]);
    
    // Create messages
    messages::YUVImage image(buf, width, height, width);
    messages::JointAngles joints;
    joints.ParseFromArray((void *) ptToJoints, numBytes[2]);

    // Setup and run module
    portals::Message<messages::YUVImage> imageMessage(&image);
    portals::Message<messages::JointAngles> jointsMessage(&joints);

    man::vision::VisionModule module;

    module.topIn.setMessage(imageMessage);
    module.bottomIn.setMessage(imageMessage);
    module.jointsIn.setMessage(jointsMessage);

    module.run();

    // -----------
    //   Y IMAGE
    // -----------
    man::vision::ImageFrontEnd* frontEnd = module.getFrontEnd(topCamera);

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
    man::vision::EdgeList* edgeList = module.getEdges(topCamera);

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
    man::vision::HoughLineList* lineList = module.getHoughLines(topCamera);

    Log* lineRet = new Log();
    std::string lineBuf;

    std::cout << std::endl << "In image coordinates:" << std::endl;
    int i = 0;
    for (auto it = lineList->begin(); it != lineList->end(); it++) {
        man::vision::HoughLine& line = *it;
        double r = line.r();
        double t = line.t();
        double ep0 = line.ep0();
        double ep1 = line.ep1();
        int lineIndex = line.fieldLine();
        
        // Java uses big endian representation
        endswap<double>(&r);
        endswap<double>(&t);
        endswap<double>(&ep0);
        endswap<double>(&ep1);
        endswap<int>(&lineIndex);

        lineBuf.append((const char*) &r, sizeof(double));
        lineBuf.append((const char*) &t, sizeof(double));
        lineBuf.append((const char*) &ep0, sizeof(double));
        lineBuf.append((const char*) &ep1, sizeof(double));
        lineBuf.append((const char*) &lineIndex, sizeof(int));

        std::cout << i++ << ", " << line.print() << std::endl;
    }

    std::cout << std::endl << "In field coordinates:" << std::endl;
    i = 0;
    for (auto it = lineList->begin(); it != lineList->end(); it++) {
        man::vision::HoughLine& line = *it;
        std::cout << i++ << ", " << line.field().print() << std::endl;
    }

    // TODO print field lines
    // TODO refactor field line id code 
    // TODO add print methods for corner and box
    std::cout << std::endl << "Goalbox and corner detection:" << std::endl;
    man::vision::GoalboxDetector* box = module.getBox(topCamera);
    man::vision::CornerDetector* corners = module.getCorners(topCamera);
    if (box->first >= 0)
        std::cout << "Box, " << box->first << ", " << box->second << std::endl;
    for (int i = 0; i < corners->size(); i++) {
        const man::vision::Corner& corner = (*corners)[i];
        std::cout << "Corner, " << corner.line1Id << ", " << corner.line2Id << ", " << (int) corner.id << std::endl;
    }

    std::cout << std::endl << "Field line classification:" << std::endl;
    man::vision::FieldLineList* fieldLineList = module.getFieldLines(topCamera);
    for (int i = 0; i < fieldLineList->size(); i++) {
        man::vision::FieldLine& line = (*fieldLineList)[i];
        std::cout << i << ", " << (int) line.id() << std::endl;
    }

    lineRet->setData(lineBuf);
    rets.push_back(lineRet);

    return 0;
}
