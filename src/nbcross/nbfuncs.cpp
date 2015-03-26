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

std::vector<nbfunc_t> FUNCS;

std::vector<logio::log_t> args;
std::vector<logio::log_t> rets;

//Common arg types -- used to check arg types and for human readability.
const char sYUVImage[] = "YUVImage";
const char sParticleSwarm_pbuf[] = "ParticleSwarm";
const char sParticle_pbuf[] = "Particle";
const char sTest[] = "test";

const char stext[] = "text";//No current sources for this data type.

int test_func() {
    assert(args.size() == 2);
    for (int i = 0; i < args.size(); ++i) {
        printf("test_func(): %s\n", args[i].desc);
    }
    
    return 0;
}

int arg_test_func() {
    printf("arg_test_func()\n");
    assert(args.size() == 2);
    for (int i = 0; i < 2; ++i) {
        printf("\t%s\n", args[i].desc);
        rets.push_back(logio::copyLog(&args[i]));
    }
    
    return 0;
}

int CrossBright_func() {
    assert(args.size() == 1);
    printf("CrossBright_func()\n");
    //work on a copy of the arg so we can safely push to rets.
    logio::log_t log = logio::copyLog(&args[0]);
    for (int i = 0; i < log.dlen; i += 2) {
        *(log.data + i) = 240;
    }
    
    printf("[%s] modified.\n", log.desc);
    rets.push_back(log);
    
    return 0;
}

int ImageConverter_func() {
    assert(args.size() == 1);

    printf("ImageConverter_func()\n");

    logio::log_t arg1 = args[0];
    std::vector<std::string> kvp = logio::pairs(arg1.desc);
    int width = 640;
    int height = 480;

    messages::YUVImage image(arg1.data, width, height, width);
    portals::Message<messages::YUVImage> message(&image);
    man::image::ImageConverterModule module;

    module.imageIn.setMessage(message);
    module.run();

    // Y image name and data
    const messages::PackedImage<short unsigned int>* yImage = module.yImage.getMessage(true).get();
    logio::log_t yRet;

    std::string yName = "type=YUVImage encoding=[Y16] width=";
    yName += std::to_string(yImage->width());
    yName += " height=";
    yName += std::to_string(yImage->height());

    yRet.desc = (char*)malloc(yName.size()+1);
    memcpy(yRet.desc, yName.c_str(), yName.size() + 1);

    yRet.dlen = yImage->width() * yImage->height() * 2;
    yRet.data = (uint8_t*)malloc(yRet.dlen);
    memcpy(yRet.data, yImage->pixelAddress(0, 0), yRet.dlen);

    rets.push_back(yRet);

    // White image retreval, description, and data
    const messages::PackedImage<unsigned char>* whiteImage = module.whiteImage.getMessage(true).get();
    logio::log_t whiteRet;

    std::string whiteName = "type=YUVImage encoding=[Y8] width=";
    whiteName += std::to_string(whiteImage->width());
    whiteName += " height=";
    whiteName += std::to_string(whiteImage->height());

    whiteRet.desc = (char*)malloc(whiteName.size()+1);
    memcpy(whiteRet.desc, whiteName.c_str(), whiteName.size()+1);

    whiteRet.dlen = whiteImage->width() * whiteImage->height();
    whiteRet.data = (uint8_t*)malloc(whiteRet.dlen);
    memcpy(whiteRet.data, whiteImage->pixelAddress(0, 0), whiteRet.dlen);

    rets.push_back(whiteRet);

    // Orange image retreval, description, and data
    const messages::PackedImage<unsigned char>* orangeImage = module.orangeImage.getMessage(true).get();
    logio::log_t orangeRet;

    std::string orangeName = "type=YUVImage encoding=[Y8] width=";
    orangeName += std::to_string(orangeImage->width());
    orangeName += " height=";
    orangeName += std::to_string(orangeImage->height());

    orangeRet.desc = (char*)malloc(orangeName.size()+1);
    memcpy(orangeRet.desc, orangeName.c_str(), orangeName.size()+1);

    orangeRet.dlen = orangeImage->width() * orangeImage->height();
    orangeRet.data = (uint8_t*)malloc(orangeRet.dlen);
    memcpy(orangeRet.data, orangeImage->pixelAddress(0, 0), orangeRet.dlen);

    rets.push_back(orangeRet);

    // Green image retreval, description, and data
    const messages::PackedImage<unsigned char>* greenImage = module.greenImage.getMessage(true).get();
    logio::log_t greenRet;

    std::cout << "Green width: " << greenImage->width() <<  std::endl;

    std::string greenName = "type=YUVImage encoding=[Y8] width=";
    greenName += std::to_string(greenImage->width());
    greenName += " height=";
    greenName += std::to_string(greenImage->height());

    greenRet.desc = (char*)malloc(greenName.size()+1);
    memcpy(greenRet.desc, greenName.c_str(), greenName.size()+1);

    greenRet.dlen = greenImage->width() * greenImage->height();
    greenRet.data = (uint8_t*)malloc(greenRet.dlen);
    memcpy(greenRet.data, greenImage->pixelAddress(0, 0), greenRet.dlen);

    rets.push_back(greenRet);
    
    // Done
    printf("ImageConverter module ran! W: %d, H: %d\n", yImage->width(), yImage->height());
    return 0;
}

void register_funcs() {
    
    /*test func 1*/
    nbfunc_t test;
    test.name = (const char *) "simple test";
    test.args = {sTest, sTest};
    test.func = test_func;
    FUNCS.push_back(test);
    
    /*test func 2*/
    nbfunc_t arg_test;
    arg_test.name = (char *) "arg test";
    arg_test.args = {sYUVImage, sYUVImage};
    arg_test.func = arg_test_func;
    FUNCS.push_back(arg_test);
    
    //CrossBright
    nbfunc_t CrossBright;
    CrossBright.name = "CrossBright";
    CrossBright.args = {sYUVImage};
    CrossBright.func = CrossBright_func;
    FUNCS.push_back(CrossBright);

    //ImageConverter
    nbfunc_t ImageConverter;
    ImageConverter.name = "ImageConverter";
    ImageConverter.args = {sYUVImage};
    ImageConverter.func = ImageConverter_func;
    FUNCS.push_back(ImageConverter);

}


