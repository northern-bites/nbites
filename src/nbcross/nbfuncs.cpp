//
//  nbfuncs.cpp
//  nbcross
//
//  Created by Philip Koch on 11/28/14.
//  Copyright (c) 2014 pkoch. All rights reserved.
//

#include "nbfuncs.h"
#include <assert.h>
#include <string.h>

#include "Images.h"
#include "image/ImageConverterModule.h"
#include "new_vision/Blobber.h"
#include "new_vision/Blob.h"
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

int BlobTest_func() {
    assert(args.size() == 1);
    printf("BlobTest_func()\n");

    const logio::log_t arg1 = args[0];

    // Hardcoded for now. TODO
    man::vision::Blobber<uint8_t> b(arg1.data, 640, 480, 2, 640*2);
    printf("about to run\n");
    b.run(man::vision::NeighborRule::eight, 180, 200, 50);

    logio::log_t ret1;

    std::string name = "type=YUVImage encoding=[Y16] width=640 height=480";

    ret1.desc = (char*)malloc(name.size() + 1);
    memcpy(ret1.desc, name.c_str(), name.size() + 1);

    ret1.dlen = 640 * 480 * sizeof(short unsigned int);
    ret1.data = (uint8_t*)malloc(640*480* sizeof(short unsigned int));
    memcpy(ret1.data, b.getImage(), ret1.dlen);

    std::vector<man::vision::Blob> results = b.getResult();

    for(int i=0; i<results.size(); i++){
        man::vision::Blob found = results.at(i);
        printf("Blob of size:%f, centered at:(%f, %f), with lengths: %f, %f\n",
               found.area(), found.xCenter(), found.yCenter(),
               found.principalLength1(), found.principalLength2());
    }

    rets.push_back(ret1);
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

    const messages::PackedImage<short unsigned int>* yImage = module.yImage.getMessage(true).get();

    logio::log_t ret1;

    std::string name = "type=YUVImage encoding=[Y16] width=";
    name += std::to_string(yImage->width());
    name += " height=";
    name += std::to_string(yImage->height());

    ret1.desc = (char*)malloc(name.size()+1);
    memcpy(ret1.desc, name.c_str(), name.size() + 1);

    ret1.dlen = yImage->width() * yImage->height() * 2;
    ret1.data = (uint8_t*)malloc(ret1.dlen);
    memcpy(ret1.data, yImage->pixelAddress(0, 0), ret1.dlen);

    rets.push_back(ret1);

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

    //BlobTest
    nbfunc_t BlobTest;
    BlobTest.name = "BlobTest";
    BlobTest.args = {sYUVImage};
    BlobTest.func = BlobTest_func;
    FUNCS.push_back(BlobTest);

    //ImageConverter
    nbfunc_t ImageConverter;
    ImageConverter.name = "ImageConverter";
    ImageConverter.args = {sYUVImage};
    ImageConverter.func = ImageConverter_func;
    FUNCS.push_back(ImageConverter);

}


