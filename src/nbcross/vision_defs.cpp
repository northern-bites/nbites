#include "nbfuncs.h"

#include "RoboGrams.h"
#include "Images.h"
#include "vision/VisionModule.h"

#include <assert.h>
#include <vector>
#include <string>
#include <iostream>

using nblog::Log;
using nblog::SExpr;

int ImageConverter_func() {
    assert(args.size() == 1);

    printf("ImageConverter_func()\n");
    printf("******* If you want to use a color table, define in nbcross/vision_defs.cpp *******\n");


    int width = 320;
    int height = 240;
    int pitch = 1280;

    Log * copy = new Log(args[0]);
    size_t length = copy->data().size();
    uint8_t buf[length];
    memcpy(buf, copy->data().data(), length);
    
    messages::YUVImage image(buf, width, height, pitch);
    messages::YUVImage emptyImage;
    messages::JointAngles emptyJoints;
    messages::InertialState emptyInertials;

    portals::Message<messages::YUVImage> emptyImageMessage(&emptyImage);
    portals::Message<messages::YUVImage> message(&image);
    portals::Message<messages::JointAngles> emptyJointsMessage(&emptyJoints);
    portals::Message<messages::InertialState> emptyInertialsMessage(&emptyInertials);
   // char tableAddrr[] = "/home/evanhoyt/nbites/data/tables/evanStation15V5.mtb";
    //man::image::ImageConverterModule module = man::image::ImageConverterModule(tableAddrr);

    man::vision::VisionModule module;


    module.topIn.setMessage(emptyImageMessage);
    module.bottomIn.setMessage(message);
    module.jointsIn.setMessage(emptyJointsMessage);
    module.inertialsIn.setMessage(emptyInertialsMessage);

    module.run();
    man::vision::ImageFrontEnd* frontEnd = module.getFrontEnd();

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

    // Done
    return 0;
}

int Edges_func() {
    assert(args.size() == 1);
    printf("Edges_func()\n");

    size_t length = args[0]->data().size();
    uint8_t buf[length];
    memcpy(buf, args[0]->data().data(), length);

    messages::YUVImage image(buf, 640, 480, 640);
    portals::Message<messages::YUVImage> message(&image);

    man::vision::VisionModule module = man::vision::VisionModule();
    module.topIn.setMessage(message);
    module.bottomIn.setMessage(message);
    module.run();
}
