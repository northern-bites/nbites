//
//  nbfuncs.cpp
//  nbcross
//
//  Created by Philip Koch on 11/28/14.
//

#include "nbfuncs.h"
#include <assert.h>
#include <string.h>
#include <sstream>

#include "Images.h"
#include "image/ImageConverterModule.h"
#include "vision/Blobber.h"
#include "vision/Blob.h"
#include "vision/BallDetector.h"
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

    const logio::log_t log = args[0];

    // Hardcoded for now. TODO
    int width = 640;
    int height = 480;

    messages::YUVImage image(log.data, width, height, width);
    portals::Message<messages::YUVImage> message(&image);
    man::image::ImageConverterModule module;

    module.imageIn.setMessage(message);
    module.run();

    const messages::PackedImage8* orangeImage = module.orangeImage.getMessage(true).get();

    man::vision::Blobber<uint8_t> b(orangeImage->pixelAddress(0, 0), orangeImage->width(),
                                    orangeImage->height(), 1, orangeImage->width());

    b.run(man::vision::NeighborRule::eight, 90, 100, 100, 100);

    logio::log_t ret1;

    std::string name = "type=YUVImage encoding=[Y16] width=320 height=240";

    ret1.desc = (char*)malloc(name.size() + 1);
    memcpy(ret1.desc, name.c_str(), name.size() + 1);

    ret1.dlen = 320 * 240 * sizeof(short unsigned int);
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

int BallImage_func() {
    assert(args.size() == 1);
    printf("BallImage_func()\n");

    logio::log_t log = logio::copyLog(&args[0]);

    int width = 640;
    int height = 480;

    messages::YUVImage image(log.data, width, height, width);
    portals::Message<messages::YUVImage> message(&image);
    man::image::ImageConverterModule module;

    module.imageIn.setMessage(message);
    module.run();

    const messages::PackedImage8* orangeImage = module.orangeImage.getMessage(true).get();

    man::vision::BallDetector detector(orangeImage);
    std::vector<std::pair<man::vision::Circle ,double> > balls = detector.findBalls();
    printf("found: %d balls!\n", balls.size());

    logio::log_t orange;
    std::string orangeDesc = "type=YUVImage encoding=[Y8] width=320 height=240";
    orange.desc = (char*)malloc(orangeDesc.size() + 1);
    memcpy(orange.desc, orangeDesc.c_str(), orangeDesc.size() + 1);

    orange.dlen = orangeImage->width() * orangeImage->height();
    orange.data = (uint8_t*)malloc(orange.dlen);
    memcpy(orange.data, orangeImage->pixelAddress(0, 0), orange.dlen);
    rets.push_back(orange);

    std::string ballStr = "";
    for(int i=0; i<balls.size(); i++) {
        std::pair<man::vision::Circle, double> ball = balls.at(i);
        std::stringstream stream;
        stream << "{" << ball.first.center.x * 2 << "," << ball.first.center.y * 2 << ",";
        stream << ball.first.radius * 2 << "," << ball.second << "} ";
        ballStr += stream.str();
        printf("   %s\n", stream.str().c_str());
    }

    logio::log_t ball;
    std::string ballDesc = "type=[Ball]";
    ball.desc = (char*)malloc(ballDesc.size() + 1);
    memcpy(ball.desc, ballDesc.c_str(), ballDesc.size() + 1);

    ball.dlen = ballStr.size() + 1;
    ball.data = (uint8_t*)malloc(ball.dlen);
    memcpy(ball.data, ballStr.c_str(), ball.dlen);
    rets.push_back(ball);

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

    //BlobTest
    nbfunc_t BlobTest;
    BlobTest.name = "BlobTest";
    BlobTest.args = {sYUVImage};
    BlobTest.func = BlobTest_func;
    FUNCS.push_back(BlobTest);

    //BallImage
    nbfunc_t BallImage;
    BallImage.name = "BallImage";
    BallImage.args = {sYUVImage};
    BallImage.func = BallImage_func;
    FUNCS.push_back(BallImage);

    //ImageConverter
    nbfunc_t ImageConverter;
    ImageConverter.name = "ImageConverter";
    ImageConverter.args = {sYUVImage};
    ImageConverter.func = ImageConverter_func;
    FUNCS.push_back(ImageConverter);

}


