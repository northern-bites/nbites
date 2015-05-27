//
//  nbfuncs.cpp
//  nbcross
//
//  Created by Philip Koch on 11/28/14.
//  Copyright (c) 2014 pkoch. All rights reserved.
//

#include "nbfuncs.h"
#include <assert.h>
#include <vector>

#include "Images.h"
#include "image/ImageConverterModule.cpp"
#include "RoboGrams.h"

using nblog::Log;


int ImageConverter_func() {
    assert(args.size() == 1);

    printf("ImageConverter_func()\n");


    int width = 640;
    int height = 480;
    int pitch = 640;

    Log * copy = new Log(args[0]);
    size_t length = copy->data().size();
    uint8_t buf[length];
    memcpy(buf, copy->data().data(), length);
    
    messages::YUVImage image(buf, width, height, pitch);
    portals::Message<messages::YUVImage> message(&image);
    man::image::ImageConverterModule module;

    module.imageIn.setMessage(message);
    module.run();

    // -----------
    //   Y IMAGE
    // -----------
    Log* yRet = new Log();
    int yLength = 240*320*2;

    // Get yImage from module message
    const messages::PackedImage<short unsigned int>* yImage = module.yImage.getMessage(true).get();

    // Create temp buffer and fill with yImage 
    uint8_t yBuf[yLength];
    memcpy(yBuf, yImage->pixelAddress(0, 0), yLength);

    // Convert to string and set log
    std::string yBuffer((const char*)yBuf, yLength);
    yRet->setData(yBuffer);

    rets.push_back(yRet);


    // ---------------
    //   WHITE IMAGE
    // ---------------
    Log* whiteRet = new Log();
    int whiteLength = 240*320;

    // Get yImage from module message
    const messages::PackedImage<unsigned char>* whiteImage = module.whiteImage.getMessage(true).get();

    // Create temp buffer and fill with yImage 
    uint8_t whiteBuf[whiteLength];
    memcpy(whiteBuf, whiteImage->pixelAddress(0, 0), whiteLength);

    // Convert to string and set log
    std::string whiteBuffer((const char*)whiteBuf, whiteLength);
    whiteRet->setData(whiteBuffer);

    rets.push_back(whiteRet);

    // ----------------
    //   ORANGE IMAGE
    // ----------------
    Log* orangeRet = new Log();
    int orangeLength = 240*320;

    // Get yImage from module message
    const messages::PackedImage<unsigned char>* orangeImage = module.orangeImage.getMessage(true).get();

    // Create temp buffer and fill with yImage 
    uint8_t orangeBuf[orangeLength];
    memcpy(orangeBuf, orangeImage->pixelAddress(0, 0), orangeLength);

    // Convert to string and set log
    std::string orangeBuffer((const char*)orangeBuf, orangeLength);
    orangeRet->setData(orangeBuffer);

    rets.push_back(orangeRet);


    // ---------------
    //   GREEN IMAGE
    // ---------------
    Log* greenRet = new Log();
    int greenLength = 240*320;

    // Get yImage from module message
    const messages::PackedImage<unsigned char>* greenImage = module.greenImage.getMessage(true).get();

    // Create temp buffer and fill with yImage 
    uint8_t greenBuf[greenLength];
    memcpy(greenBuf, greenImage->pixelAddress(0, 0), greenLength);

    // Convert to string and set log
    std::string greenBuffer((const char*)greenBuf, greenLength);
    greenRet->setData(greenBuffer);

    rets.push_back(greenRet);

    // Done
    printf("ImageConverter module ran! W: %d, H: %d\n", yImage->width(), yImage->height());
    return 0;
}
