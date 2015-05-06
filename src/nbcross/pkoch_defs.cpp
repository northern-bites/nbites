#include "nbfuncs.h"
#include <assert.h>
#include <vector>
#include <string>
#include <iostream>

#include "vision/PostDetector.h"
#include "vision/Gradient.h"
#include "vision/EdgeDetector.h"

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

    Log* log = new Log(args[0]);

    int width = 640;
    int height = 480;

    messages::YUVImage image((unsigned char)log->data().c_str(), width, height, width);
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

    // Post image
    const messages::PackedImage8& postImage = detector.getPostImage();
    std::string postImageDesc = "type=YUVImage encoding=[Y8] width=";
    postImageDesc += std::to_string(postImage.width());
    postImageDesc += " height=";
    postImageDesc += std::to_string(postImage.height());
    Log* postImageRet = new Log(postImageDesc);

    postImageRet->setData(std::string((const char *)postImage.pixelAddress(0, 0), postImage.width()*postImage.height()));
    rets.push_back(postImageRet);

    // Unfiltered histogram
    std::string unfiltHistDesc = "type=Histogram";
    Log* unfiltHistRet = new Log(unfiltHistDesc);

    unfiltHistRet->setData(std::string((const char *)detector.getUnfilteredHistogram(), 8*detector.getLengthOfHistogram()));
    rets.push_back(unfiltHistRet);

    // Filtered histogram
    std::string filtHistDesc = "type=Histogram";
    Log* filtHistRet = new Log(filtHistDesc);

    filtHistRet->setData(std::string((const char *)detector.getFilteredHistogram(), 8*detector.getLengthOfHistogram()));
    rets.push_back(filtHistRet);

    // Goalpost candidates
    std::string postsRetDesc = "type=DetectedGoalposts";
    Log* postsRet = new Log(postsRetDesc);

    int size = 4 * posts.size();
    if (size) {
        uint8_t* temp = (uint8_t*)malloc(postsRet.dlen);
        int* dataAsIntPt = reinterpret_cast<int*>(temp);
        for (int i = 0; i < posts.size(); i++)
            dataAsIntPt[i] = posts[i]; 
        postsRet->setData((const char *)dataAsIntPt, size);
        free(temp);
    }
    rets.push_back(postsRet);

    // Cleanup
    delete gradient;
}
