#include "nbfuncs.h"
#include <assert.h>
#include <vector>
#include <string>
#include <iostream>

#include "vision/PostDetector.h"
#include "vision/Gradient.h"
#include "vision/EdgeDetector.h"

    //PostDetector
    // nbfunc_t PostDetector;
    // PostDetector.name = "PostDetector";
    // PostDetector.args = {sYUVImage};
    // PostDetector.func = PostDetector_func;
    // FUNCS.push_back(PostDetector);

using nblog::Log;
using nblog::SExpr;

int test_func() {
    assert(args.size() == 2);
    for (int i = 0; i < args.size(); ++i) {
        printf("test_func(): %s\n", args[i]->description().c_str());
    }
    
    return 0;
}

int arg_test_func() {
    printf("arg_test_func()\n");
    assert(args.size() == 2);
    for (int i = 0; i < 2; ++i) {
        printf("\t%s\n", args[i]->description().c_str());
        rets.push_back(new Log(args[i]));
    }
    
    return 0;
}

int CrossBright_func() {
    assert(args.size() == 1);
    printf("CrossBright_func()\n");
    //work on a copy of the arg so we can safely push to rets.
    
    Log * copy = new Log(args[0]);
    size_t length = copy->data().size();
    uint8_t buf[length];
    memcpy(buf, copy->data().data(), length);
    
    for (int i = 0; i < length; i += 2) {
        *(buf + i) = 240;
    }
    std::string buffer((const char *) buf, length);
    copy->setData(buffer);
    
    printf("[%s] modified.\n", copy->description().c_str());
    rets.push_back(copy);
    
    return 0;
}

int PostDetector_func() {
    assert(args.size() == 1);
    printf("PostDetector_func()\n");

    logio::log_t log = logio::copyLog(&args[0]);

    int width = 640;
    int height = 480;

    messages::YUVImage image(log.data, width, height, width);
    portals::Message<messages::YUVImage> message(&image);
    man::image::ImageConverterModule imageConverter;

    imageConverter.imageIn.setMessage(message);
    imageConverter.run();

    const messages::PackedImage16* yImage = imageConverter.yImage.getMessage(true).get();
    const messages::PackedImage8* whiteImage = imageConverter.whiteImage.getMessage(true).get();

    man::vision::Gradient* gradient = new man::vision::Gradient();

    man::vision::EdgeDetector edgeDetector;
    edgeDetector.sobelOperator(0, yImage->pixelAddress(0, 0), *gradient);

    man::vision::PostDetector detector(*gradient, *whiteImage);
    const std::vector<int>& posts = detector.getCandidates();

    printf("Found %d candidate posts.\n", posts.size());
    for(int i = 0; i < posts.size(); i++)
        printf("Found post at %d column.\n", posts[i]);

    const messages::PackedImage8& postImage = detector.getPostImage();
    logio::log_t postImageRet;

    std::string postImageDesc = "type=YUVImage encoding=[Y8] width=";
    postImageDesc += std::to_string(postImage.width());
    postImageDesc += " height=";
    postImageDesc += std::to_string(postImage.height());

    postImageRet.desc = (char*)malloc(postImageDesc.size()+1);
    memcpy(postImageRet.desc, postImageDesc.c_str(), postImageDesc.size()+1);

    postImageRet.dlen = postImage.width() * postImage.height();
    postImageRet.data = (uint8_t*)malloc(postImageRet.dlen);
    memcpy(postImageRet.data, postImage.pixelAddress(0, 0), postImageRet.dlen);

    rets.push_back(postImageRet);

    logio::log_t unfiltHistRet;
    std::string unfiltHistDesc = "type=Histogram";

    unfiltHistRet.desc = (char*)malloc(unfiltHistDesc.size()+1);
    memcpy(unfiltHistRet.desc, unfiltHistDesc.c_str(), unfiltHistDesc.size()+1);

    unfiltHistRet.dlen = 8 * detector.getLengthOfHistogram();
    unfiltHistRet.data = (uint8_t*)malloc(unfiltHistRet.dlen);
    memcpy(unfiltHistRet.data, detector.getUnfilteredHistogram(), unfiltHistRet.dlen);

    rets.push_back(unfiltHistRet);

    logio::log_t filtHistRet;
    std::string filtHistDesc = "type=Histogram";

    filtHistRet.desc = (char*)malloc(filtHistDesc.size()+1);
    memcpy(filtHistRet.desc, filtHistDesc.c_str(), filtHistDesc.size()+1);

    filtHistRet.dlen = 8 * detector.getLengthOfHistogram();
    filtHistRet.data = (uint8_t*)malloc(filtHistRet.dlen);
    memcpy(filtHistRet.data, detector.getFilteredHistogram(), filtHistRet.dlen);

    rets.push_back(filtHistRet);

    logio::log_t postsRet;
    std::string postsRetDesc = "type=DetectedGoalposts";

    postsRet.desc = (char*)malloc(postsRetDesc.size()+1);
    memcpy(postsRet.desc, postsRetDesc.c_str(), postsRetDesc.size()+1);

    postsRet.dlen = 4 * posts.size();
    if (postsRet.dlen) {
        postsRet.data = (uint8_t*)malloc(postsRet.dlen);
        int* dataAsIntPt = reinterpret_cast<int*>(postsRet.data);
        for (int i = 0; i < posts.size(); i++)
            dataAsIntPt[i] = posts[i]; 
    } else {
        postsRet.data = NULL;
    }

    rets.push_back(postsRet);

    delete gradient;
}
