//
//  VisionDefs.cpp
//  tool8-separate
//

#ifndef __APPLE__

#include <stdio.h>
#include "nbcross.hpp"

#include "RoboGrams.h"
#include "Images.h"
#include "vision/VisionModule.h"
#include "vision/FrontEnd.h"
#include "vision/Homography.h"
#include "vision/Spots.h"
#include "ParamReader.h"
#include "NBMath.h"

#include <cstdlib>
#include <netinet/in.h>
#include <assert.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <map>

#include "Helpers.hpp"

messages::YUVImage emptyTop(
                            man::vision::DEFAULT_TOP_IMAGE_WIDTH * 2,
                            man::vision::DEFAULT_TOP_IMAGE_HEIGHT
                            );

messages::YUVImage emptyBot(
                            man::vision::DEFAULT_TOP_IMAGE_WIDTH,
                            man::vision::DEFAULT_TOP_IMAGE_HEIGHT / 2
                            );

/* NBCROSS FUNCTIONS */

NBCROSS_FUNCTION(Vision, false, nbl::SharedConstants::LogClass_Tripoint())
    (const std::vector<nbl::logptr> &arguments)
{
    printf("Vision()\n");
    NBL_ASSERT(arguments.size() == 1);
    NBL_ASSERT(arguments[0]->blocks.size() > 2);

    logptr theLog = arguments[0];

    Block& imageBlock = theLog->blocks[0];
    Block& inertialBlock = theLog->blocks[1];
    Block& jointsBlock = theLog->blocks[2];

    bool topCamera;
    if (imageBlock.whereFrom == "camera_TOP") {
        topCamera = true;
    } else if (imageBlock.whereFrom == "camera_BOT") {
        topCamera = false;
    } else {
        throw std::runtime_error("unknown camera in Vision()");
    }

    int width = 2 * imageBlock.dict.at(SharedConstants::LOG_BLOCK_IMAGE_WIDTH_PIXELS()).asConstNumber().asInt();
    int height = imageBlock.dict.at(SharedConstants::LOG_BLOCK_IMAGE_HEIGHT_PIXELS()).asConstNumber().asInt();

    imageSizeCheck(topCamera, width, height);

    printf("ARGUMENT WAS: camera:%d width:%d height:%d",
           !topCamera, width, height);

//    messages::YUVImage realImage = imageBlock.copyAsYUVImage();
//    messages::YUVImage realImage = imageBlock.parseAsYUVImage();

    std::string lbuf;
    messages::YUVImage realImage = imageBlock.copyAsYUVImage(lbuf);

    printf("parsed image width=%d, height=%d\n", realImage.width(), realImage.height() );

    // Read number of bytes of image, inertials, and joints if exist
    messages::JointAngles joints;
    jointsBlock.parseAsProtobuf(joints);

    std::string rname = theLog->topLevelDictionary.at(SharedConstants::LOG_TOPLEVEL_HOST_NAME()).asString();

    man::vision::VisionModule& module = getModuleRef(rname);

    // Setup module
    portals::Message<messages::YUVImage> rImageMessage(&realImage);
    portals::Message<messages::YUVImage> eImageMessage(
                                                       topCamera ? &emptyBot : & emptyTop );

    portals::Message<messages::JointAngles> jointsMessage(&joints);

    if (topCamera) {
        module.topIn.setMessage(rImageMessage);
        module.bottomIn.setMessage(eImageMessage);
    }
    else {
        module.topIn.setMessage(eImageMessage);
        module.bottomIn.setMessage(rImageMessage);
    }

    module.jointsIn.setMessage(jointsMessage);

    module.blackStar(false);

    if (theLog->topLevelDictionary.find("OriginalCameraOffsets") !=
        theLog->topLevelDictionary.end() ) {
        NBL_INFO("using OriginalCameraOffsets");
        json::Object& offsets = theLog->topLevelDictionary["OriginalCameraOffsets"].asObject();
        module.setCalibrationParams(man::vision::calibration::parseOffsetsFromJSON(offsets), topCamera);
    }

    if (theLog->topLevelDictionary.find("ModifiedCameraOffsets") !=
        theLog->topLevelDictionary.end()) {
        NBL_INFO("using ModifiedCameraOffsets");
        json::Object& offsets = theLog->topLevelDictionary["ModifiedCameraOffsets"].asObject();
        module.setCalibrationParams(man::vision::calibration::parseOffsetsFromJSON(offsets), topCamera);
    }

    if (theLog->topLevelDictionary.find("OriginalColorParams") !=
        theLog->topLevelDictionary.end()) {
        NBL_INFO("using OriginalColorParams");
        json::Object& offsets = theLog->topLevelDictionary["OriginalColorParams"].asObject();
        module.setColorParams(man::vision::calibration::parseColorsFromJSON(offsets), topCamera);
    }

    if (theLog->topLevelDictionary.find("ModifiedColorParams") !=
        theLog->topLevelDictionary.end()) {
        NBL_INFO("using ModifiedColorParams");
        json::Object& offsets = theLog->topLevelDictionary["ModifiedColorParams"].asObject();
//        NBL_INFO("ModifiedColorParams object retrieved");
        module.latestUsedColorParams[!topCamera] = offsets;
        module.setColorParams(man::vision::calibration::parseColorsFromJSON(offsets), topCamera);
        NBL_INFO("ModifiedColorParams object parsed");
    }

    if (theLog->blocks.size() > 3 && theLog->blocks[2].type == SharedConstants::SexprType_DEFAULT()) {
        NBL_WARN("using camera offsets parameters in LISP FORM (v8.0)");
        SExpr _calParams = theLog->blocks[3].parseAsSexpr();

        SExpr* calParams = &_calParams;
        calParams = topCamera ? calParams->find("camera_TOP") : calParams->find("camera_BOT");
        if (calParams != NULL) {
            std::cout << "Found and using calibration params in log description: "
            << "Roll: " << calParams->get(1)->valueAsDouble() << " Tilt: " <<  calParams->get(2)->valueAsDouble()<< std::endl;
            man::vision::CalibrationParams* ncp =
                new man::vision::CalibrationParams(calParams->get(1)->valueAsDouble(),
                                               calParams->get(2)->valueAsDouble());

            module.setCalibrationParams(ncp, topCamera);
        }
    }

    if (theLog->topLevelDictionary.find("DebugDrawing") !=
            theLog->topLevelDictionary.end()) {
        std::string ser = theLog->topLevelDictionary["DebugDrawing"].asString();

        SExpr * deser = SExpr::read(ser);
        module.setDebugDrawingParameters(deser);
        delete deser;
    }

    // Run it!
    module.run();

    std::vector<Block> retVec;
    // -----------
    //   Y IMAGE
    // -----------
    man::vision::ImageFrontEnd* frontEnd = module.getFrontEnd(topCamera);

    // Create temp buffer and fill with yImage from FrontEnd
    int yLength = (width / 4) * (height / 2) * 2;

    uint8_t yBuf[yLength];
    memcpy(yBuf, frontEnd->yImage().pixelAddr(), yLength);

    // Convert to string and set log
    std::string yBuffer((const char*)yBuf, yLength);
    retVec.push_back(Block{yBuffer, json::Object{}, "yBuffer", "nbcross", 0, 0});

    // ---------------
    //   WHITE IMAGE
    // ---------------
    int whiteLength = (width / 4) * (height / 2);
    json::Object whiteDictionary;
    //    whiteDictionary["ColorParams"] = json::String(getSExprFromSavedParams(VISION_WHITE, sexpPath, topCamera).serialize());
    whiteDictionary["ColorParams"] = module.latestUsedColorParams[!topCamera]["white"];
    retVec.push_back(Block{std::string{ (const char *) frontEnd->whiteImage().pixelAddr(), whiteLength}, whiteDictionary, "whiteRet", "nbcross", 0, 0});

    // ---------------
    //   GREEN IMAGE
    // ---------------

    int greenLength = (width / 4) * (height / 2);
    json::Object greenDictionary;
    //    greenDictionary["ColorParams"] = json::String(getSExprFromSavedParams(VISION_GREEN, sexpPath, topCamera).serialize());
    greenDictionary["ColorParams"] = module.latestUsedColorParams[!topCamera]["green"];
    retVec.push_back(Block{std::string{ (const char *)  (const char *)frontEnd->greenImage().pixelAddr(), greenLength}, greenDictionary, "greenRet", "nbcross", 0, 0});

    // ----------------
    //   ORANGE IMAGE
    // ----------------
    int orangeLength = (width / 4) * (height / 2);
    json::Object orangeDictionary;
    //    orangeDictionary["ColorParams"] = json::String(getSExprFromSavedParams(VISION_ORANGE, sexpPath, topCamera).serialize());
    orangeDictionary["ColorParams"] = module.latestUsedColorParams[!topCamera]["orange"];
    retVec.push_back(Block{std::string{ (const char *) frontEnd->orangeImage().pixelAddr(), orangeLength}, orangeDictionary, "orangeRet", "nbcross", 0, 0});

    //-------------------
    //  SEGMENTED IMAGE
    //-------------------
    int colorSegLength = (width / 4) * (height / 2);
    retVec.push_back(Block{std::string{ (const char *) frontEnd->colorImage().pixelAddr(), colorSegLength}, json::Object{}, "segBuffer", "nbcross", 0, 0});

    //-------------------
    //  EDGES
    //-------------------

    man::vision::EdgeList* edgeList = module.getEdges(topCamera);

    // Uncomment to display rejected edges used in center detection
    // man::vision::EdgeList* edgeList = module.getRejectedEdges(topCamera);

    std::string edgeBuf;
    man::vision::AngleBinsIterator<man::vision::Edge> abi(*edgeList);
    for (const man::vision::Edge* e = *abi; e; e = *++abi) {
        uint32_t x = htonl(e->x() + (width / 8));
        edgeBuf.append((const char*) &x, sizeof(uint32_t));
        uint32_t y = htonl(-e->y() + (height / 4));
        edgeBuf.append((const char*) &y, sizeof(uint32_t));
        uint32_t mag = htonl(e->mag());
        edgeBuf.append((const char*) &mag, sizeof(uint32_t));
        uint32_t angle = htonl(e->angle());
        edgeBuf.append((const char*) &angle, sizeof(uint32_t));
    }

    retVec.push_back(Block{edgeBuf, json::Object{}, "edgeBuffer", "nbcross", 0, 0});

    //-------------------
    //  LINES
    //-------------------
    man::vision::HoughLineList* lineList = module.getHoughLines(topCamera);

    std::string lineBuf;

    bool debugLines = false;
    if (debugLines)
    std::cout << std::endl << "Hough lines in image coordinates:" << std::endl;

    for (auto it = lineList->begin(); it != lineList->end(); it++) {
        man::vision::HoughLine& line = *it;

        // Get image coordinates
        double icR = line.r();
        double icT = line.t();
        double icEP0 = line.ep0();
        double icEP1 = line.ep1();

        int houghIndex = line.index();
        int fieldIndex = line.fieldLine();

        // Get field coordinates
        double fcR = line.field().r();
        double fcT = line.field().t();
        double fcEP0 = line.field().ep0();
        double fcEP1 = line.field().ep1();

        // Java uses big endian representation
        endswap<double>(&icR);
        endswap<double>(&icT);
        endswap<double>(&icEP0);
        endswap<double>(&icEP1);
        endswap<int>(&houghIndex);
        endswap<int>(&fieldIndex);
        endswap<double>(&fcR);
        endswap<double>(&fcT);
        endswap<double>(&fcEP0);
        endswap<double>(&fcEP1);


        lineBuf.append((const char*) &icR, sizeof(double));
        lineBuf.append((const char*) &icT, sizeof(double));
        lineBuf.append((const char*) &icEP0, sizeof(double));
        lineBuf.append((const char*) &icEP1, sizeof(double));
        lineBuf.append((const char*) &houghIndex, sizeof(int));
        lineBuf.append((const char*) &fieldIndex, sizeof(int));
        lineBuf.append((const char*) &fcR, sizeof(double));
        lineBuf.append((const char*) &fcT, sizeof(double));
        lineBuf.append((const char*) &fcEP0, sizeof(double));
        lineBuf.append((const char*) &fcEP1, sizeof(double));

        if (debugLines)
            std::cout << line.print() << std::endl;
    }

    if (debugLines)
            std::cout << std::endl << "Hough lines in field coordinates:" << std::endl;

    int i = 0;
    for (auto it = lineList->begin(); it != lineList->end(); it++) {
        man::vision::HoughLine& line = *it;
        if (debugLines)
            std::cout << line.field().print() << std::endl;
    }

    if (debugLines) {
        std::cout << std::endl << "Field lines:" << std::endl;
        std::cout << "0.idx, 1.idx, id, idx" << std::endl;
    }
    man::vision::FieldLineList* fieldLineList = module.getFieldLines(topCamera);

    for (int i = 0; i < fieldLineList->size(); i++) {
        man::vision::FieldLine& line = (*fieldLineList)[i];
        if (debugLines)
            std::cout << line.print() << std::endl;
    }

    if (debugLines)
            std::cout << std::endl << "Goalbox and corner detection:" << std::endl;
    man::vision::GoalboxDetector* box = module.getBox(topCamera);
    man::vision::CornerDetector* corners = module.getCorners(topCamera);

    if (debugLines) {
        if (box->first != NULL)
            std::cout << box->print() << std::endl;
    }

    if (debugLines) {
        std::cout << "    line0, line1, type (concave, convex, T)" << std::endl;
        for (int i = 0; i < corners->size(); i++) {
            const man::vision::Corner& corner = (*corners)[i];
            std::cout << corner.print() << std::endl;
        }
    }

    retVec.push_back(Block{lineBuf, json::Object{}, "lineBuffer", "nbcross", 0, 0});

    //-----------
    //  BALL
    //-----------
//    man::vision::BallDetector* detector = module.getBallDetector(topCamera);
//    std::vector<man::vision::Ball> balls = detector->getBalls();
//
//    SExpr allBalls;
//    int count = 0;
//    for (auto i=balls.begin(); i!=balls.end(); i++) {
//        SExpr ballTree = treeFromBall(*i, width, height);
//        SExpr next = SExpr::keyValue("ball" + std::to_string(count), ballTree);
//        allBalls.append(next);
//        count++;
//    }
//    count = 0;
//    for (auto i=blobs.begin(); i!=blobs.end(); i++) {
//        if ((*i).firstPrincipalLength() < 8) {
//            SExpr blobTree = treeFromBlob(*i);
//            SExpr next = SExpr::keyValue("blob" + std::to_string(count), blobTree);
//            allBalls.append(next);
//            count++;
//        }
//    }

    man::vision::BallDetector* detector = module.getBallDetector(topCamera);

    std::vector<man::vision::Ball> balls = detector->getBalls();
	std::vector<man::vision::Spot> whiteSpots = detector->getWhiteSpots();
	std::vector<man::vision::Spot> blackSpots = detector->getBlackSpots();

    SExpr allBalls;
	SExpr allWhite;
	SExpr allBlack;
    int count = 0;
    for (auto i=balls.begin(); i!=balls.end(); i++) {
        SExpr ballTree = treeFromBall(*i, width, height);
        SExpr next = SExpr::keyValue("ball" + std::to_string(count), ballTree);
        allBalls.append(next);
        count++;
    }
	count = 0;
    for (auto i=whiteSpots.begin(); i!=whiteSpots.end(); i++) {
        SExpr spotTree = treeFromSpot(*i, width, height);
        SExpr next = SExpr::keyValue("whiteSpot" + std::to_string(count), spotTree);
        allWhite.append(next);
        count++;
    }
	count = 0;
    for (auto i=blackSpots.begin(); i!=blackSpots.end(); i++) {
        SExpr spotTree = treeFromSpot(*i, width, height);
        SExpr next = SExpr::keyValue("darkSpot" + std::to_string(count), spotTree);
        allBlack.append(next);
        count++;
    }

    retVec.push_back(Block{allBalls.serialize(), json::Object{},
				SharedConstants::SexprType_DEFAULT(), "nbcross-Vision-ball", 0, 0});
    retVec.push_back(Block{allWhite.serialize(), json::Object{},
				SharedConstants::SexprType_DEFAULT(), "nbcross-Vision-spot-white", 0, 0});
    retVec.push_back(Block{allBlack.serialize(), json::Object{},
				SharedConstants::SexprType_DEFAULT(), "nbcross-Vision-spot-black", 0, 0});

    //---------------
    // Center Circle
    //---------------
    
    man::vision::CenterCircleDetector* ccd = module.getCCD(topCamera);
    std::string pointsBuf;
    
    std::vector<std::pair<double, double>> points = ccd->getPotentials();
    for (std::pair<double, double> p : points) {
        endswap<double>(&(p.first));
        endswap<double>(&(p.second));
        pointsBuf.append((const char*) &(p.first), sizeof(double));
        pointsBuf.append((const char*) &(p.second), sizeof(double));
    }
    
    // Add 0,0 point if cc is off so tool doesn't display a rejected one
    std::pair<double, double> zero(0.0, 0.0);
    if (!ccd->on()) {
        pointsBuf.append((const char*) &(zero.first), sizeof(double));
        pointsBuf.append((const char*) &(zero.second), sizeof(double));
    }

    retVec.push_back(Block{pointsBuf, json::Object{}, "ccdBuffer", "nbcross", 0, 0});

    //-------------------
    //  DEBUG IMAGE
    //-------------------
    int debugImageLength = (width / 2) * (height / 2);
    retVec.push_back(Block{ std::string{ (const char *) module.getDebugImage(topCamera)->pixArray(), debugImageLength}, json::Object{}, "debugImage", "nbcross", 0, 0});

    RETURN(Log::explicitLog(retVec, json::Object{}, "VisionReturn"));
}

NBCROSS_FUNCTION(CalculateCameraOffsets, true, nbl::SharedConstants::LogClass_Tripoint())
    (const std::vector<nbl::logptr> &arguments)
{
    printf("CalculateCameraOffsets() %d logs\n", arguments.size());
    bumpLineFitThreshold();

    int failures = 0;
    double totalR = 0;
    double totalT = 0;

    man::vision::VisionModule& module = getModuleRef("none");

    for (logptr tripointLog : arguments) {
        module.reset();

        logptr theLog = tripointLog;

        Block& imageBlock = theLog->blocks[0];
        Block& inertialBlock = theLog->blocks[1];
        Block& jointsBlock = theLog->blocks[2];

        bool topCamera;
        if (imageBlock.whereFrom == "camera_TOP") {
            topCamera = true;
        } else if (imageBlock.whereFrom == "camera_BOT") {
            topCamera = false;
        } else {
            throw std::runtime_error("unknown camera in Vision()");
        }

        int width = 2 * imageBlock.dict.at(SharedConstants::LOG_BLOCK_IMAGE_WIDTH_PIXELS()).asConstNumber().asInt();
        int height = imageBlock.dict.at(SharedConstants::LOG_BLOCK_IMAGE_HEIGHT_PIXELS()).asConstNumber().asInt();

        imageSizeCheck(topCamera, width, height);

        std::string lbuf;
        messages::YUVImage realImage = imageBlock.copyAsYUVImage(lbuf);

        // Read number of bytes of image, inertials, and joints if exist
        messages::JointAngles joints;
        jointsBlock.parseAsProtobuf(joints);

        // Setup module
        portals::Message<messages::YUVImage> rImageMessage(&realImage);
        portals::Message<messages::YUVImage> eImageMessage(
                                                           topCamera ? &emptyBot : & emptyTop );

        portals::Message<messages::JointAngles> jointsMessage(&joints);

        if (topCamera) {
            module.topIn.setMessage(rImageMessage);
            module.bottomIn.setMessage(eImageMessage);
        }
        else {
            module.topIn.setMessage(eImageMessage);
            module.bottomIn.setMessage(rImageMessage);
        }

        module.jointsIn.setMessage(jointsMessage);

        //Calibration only makes sense on blackstar images...
        module.blackStar(true);
        
        // Run it!
        module.run();

        NBL_ASSERT_EQ(module.calibrationParams[0]->getRoll(), 0.0);
        NBL_ASSERT_EQ(module.calibrationParams[1]->getRoll(), 0.0);
        NBL_ASSERT_EQ(module.calibrationParams[0]->getTilt(), 0.0);
        NBL_ASSERT_EQ(module.calibrationParams[1]->getTilt(), 0.0);

        man::vision::FieldHomography* fh = module.getFieldHomography(topCamera);
        man::vision::HoughLineList* lineList = module.getHoughLines(topCamera);
        man::vision::FieldLineList* fieldLineList = module.getFieldLines(topCamera);

        double rollBefore, tiltBefore, rollAfter, tiltAfter;

        rollBefore = fh->roll();
        tiltBefore = fh->tilt();

        bool success = fh->calibrateFromStar(*module.getFieldLines(topCamera));

//        printf("... %s\n", success ? "SUCCESS" : "FAILURE");

        if (!success) {
            failures++;
        } else {
            rollAfter = fh->roll();
            tiltAfter = fh->tilt();
            totalR += rollAfter - rollBefore;
            totalT += tiltAfter - tiltBefore;
        }
    }

    int successes = (arguments.size() - failures);

    printf("Failed calibration %d times\n", failures);
    printf("Success calibrating %d times\n", successes);

    if (successes) {
        totalR /= (successes);
        totalT /= (successes);
    } else {
        totalR = -999;
        totalT = -999;
    }

    logptr retLog = Log::emptyLog();
    retLog->logClass = "CalibrationResults";

    retLog->topLevelDictionary["CalibrationNumSuccess"] = json::Number( successes );
    retLog->topLevelDictionary["CalibrationDeltaTilt"] = json::Number(totalT);
    retLog->topLevelDictionary["CalibrationDeltaRoll"] = json::Number(totalR);

    RETURN(retLog);
}

NBCROSS_FUNCTION(CheckCameraOffsets, false, nbl::SharedConstants::LogClass_Tripoint())
            (const std::vector<nbl::logptr> &arguments)
{
    printf("CheckCameraOffsets()\n");
    NBL_ASSERT(arguments.size() == 1);
    NBL_ASSERT(arguments[0]->blocks.size() > 2);

    bumpLineFitThreshold();
    logptr theLog = arguments[0];

    Block& imageBlock = theLog->blocks[0];
    Block& inertialBlock = theLog->blocks[1];
    Block& jointsBlock = theLog->blocks[2];

    bool topCamera;
    if (imageBlock.whereFrom == "camera_TOP") {
        topCamera = true;
    } else if (imageBlock.whereFrom == "camera_BOT") {
        topCamera = false;
    } else {
        throw std::runtime_error("unknown camera in Vision()");
    }

    int width = 2 * imageBlock.dict.at(SharedConstants::LOG_BLOCK_IMAGE_WIDTH_PIXELS()).asConstNumber().asInt();
    int height = imageBlock.dict.at(SharedConstants::LOG_BLOCK_IMAGE_HEIGHT_PIXELS()).asConstNumber().asInt();

    imageSizeCheck(topCamera, width, height);

    //    messages::YUVImage realImage = imageBlock.copyAsYUVImage();
    //    messages::YUVImage realImage = imageBlock.parseAsYUVImage();

    std::string lbuf;
    messages::YUVImage realImage = imageBlock.copyAsYUVImage(lbuf);

    printf("parsed image width=%d, height=%d\n", realImage.width(), realImage.height() );

    // Read number of bytes of image, inertials, and joints if exist
    messages::JointAngles joints;
    jointsBlock.parseAsProtobuf(joints);

    std::string rname = theLog->topLevelDictionary.at(SharedConstants::LOG_TOPLEVEL_HOST_NAME()).asString();

    man::vision::VisionModule& module = getModuleRef(rname);

    // Setup module
    portals::Message<messages::YUVImage> rImageMessage(&realImage);
    portals::Message<messages::YUVImage> eImageMessage(
                                                       topCamera ? &emptyBot : & emptyTop );

    portals::Message<messages::JointAngles> jointsMessage(&joints);

    if (topCamera) {
        module.topIn.setMessage(rImageMessage);
        module.bottomIn.setMessage(eImageMessage);
    }
    else {
        module.topIn.setMessage(eImageMessage);
        module.bottomIn.setMessage(rImageMessage);
    }

    module.jointsIn.setMessage(jointsMessage);

    module.blackStar(true);

    // Run it!
    module.run();

    man::vision::FieldHomography* fh = module.getFieldHomography(topCamera);
    bool success = fh->calibrateFromStar(*module.getFieldLines(topCamera));

    std::vector<Block> retVec;
    // -----------
    //   Y IMAGE
    // -----------
    man::vision::ImageFrontEnd* frontEnd = module.getFrontEnd(topCamera);

    // Create temp buffer and fill with yImage from FrontEnd
    int yLength = (width / 4) * (height / 2) * 2;

    uint8_t yBuf[yLength];
    memcpy(yBuf, frontEnd->yImage().pixelAddr(), yLength);

    // Convert to string and set log
    std::string yBuffer((const char*)yBuf, yLength);
    retVec.push_back(Block{yBuffer, json::Object{}, "yBuffer", "nbcross", 0, 0});

    // ---------------
    //   WHITE IMAGE
    // ---------------
    int whiteLength = (width / 4) * (height / 2);
    json::Object whiteDictionary;
//    whiteDictionary["ColorParams"] = json::String(getSExprFromSavedParams(VISION_WHITE, sexpPath, topCamera).serialize());
    whiteDictionary["ColorParams"] = module.latestUsedColorParams[!topCamera]["white"];
    retVec.push_back(Block{std::string{ (const char *) frontEnd->whiteImage().pixelAddr(), whiteLength}, whiteDictionary, "whiteRet", "nbcross", 0, 0});

    // ---------------
    //   GREEN IMAGE
    // ---------------

    int greenLength = (width / 4) * (height / 2);
    json::Object greenDictionary;
//    greenDictionary["ColorParams"] = json::String(getSExprFromSavedParams(VISION_GREEN, sexpPath, topCamera).serialize());
    greenDictionary["ColorParams"] = module.latestUsedColorParams[!topCamera]["green"];
    retVec.push_back(Block{std::string{ (const char *)  (const char *)frontEnd->greenImage().pixelAddr(), greenLength}, greenDictionary, "greenRet", "nbcross", 0, 0});

    // ----------------
    //   ORANGE IMAGE
    // ----------------
    int orangeLength = (width / 4) * (height / 2);
    json::Object orangeDictionary;
//    orangeDictionary["ColorParams"] = json::String(getSExprFromSavedParams(VISION_ORANGE, sexpPath, topCamera).serialize());
    orangeDictionary["ColorParams"] = module.latestUsedColorParams[!topCamera]["orange"];
    retVec.push_back(Block{std::string{ (const char *) frontEnd->orangeImage().pixelAddr(), orangeLength}, orangeDictionary, "orangeRet", "nbcross", 0, 0});

    //-------------------
    //  SEGMENTED IMAGE
    //-------------------
    int colorSegLength = (width / 4) * (height / 2);
    retVec.push_back(Block{std::string{ (const char *) frontEnd->colorImage().pixelAddr(), colorSegLength}, json::Object{}, "segBuffer", "nbcross", 0, 0});

    //-------------------
    //  EDGES
    //-------------------

    man::vision::EdgeList* edgeList = module.getEdges(topCamera);

    // Uncomment to display rejected edges used in center detection
    // man::vision::EdgeList* edgeList = module.getRejectedEdges(topCamera);

    std::string edgeBuf;
    man::vision::AngleBinsIterator<man::vision::Edge> abi(*edgeList);
    for (const man::vision::Edge* e = *abi; e; e = *++abi) {
        uint32_t x = htonl(e->x() + (width / 8));
        edgeBuf.append((const char*) &x, sizeof(uint32_t));
        uint32_t y = htonl(-e->y() + (height / 4));
        edgeBuf.append((const char*) &y, sizeof(uint32_t));
        uint32_t mag = htonl(e->mag());
        edgeBuf.append((const char*) &mag, sizeof(uint32_t));
        uint32_t angle = htonl(e->angle());
        edgeBuf.append((const char*) &angle, sizeof(uint32_t));
    }

    retVec.push_back(Block{edgeBuf, json::Object{}, "edgeBuffer", "nbcross", 0, 0});

    //-------------------
    //  LINES
    //-------------------
    man::vision::HoughLineList* lineList = module.getHoughLines(topCamera);

    std::string lineBuf;

    bool debugLines = false;
    if (debugLines)
    std::cout << std::endl << "Hough lines in image coordinates:" << std::endl;

    for (auto it = lineList->begin(); it != lineList->end(); it++) {
        man::vision::HoughLine& line = *it;

        // Get image coordinates
        double icR = line.r();
        double icT = line.t();
        double icEP0 = line.ep0();
        double icEP1 = line.ep1();

        int houghIndex = line.index();
        int fieldIndex = line.fieldLine();

        // Get field coordinates
        double fcR = line.field().r();
        double fcT = line.field().t();
        double fcEP0 = line.field().ep0();
        double fcEP1 = line.field().ep1();

        // Java uses big endian representation
        endswap<double>(&icR);
        endswap<double>(&icT);
        endswap<double>(&icEP0);
        endswap<double>(&icEP1);
        endswap<int>(&houghIndex);
        endswap<int>(&fieldIndex);
        endswap<double>(&fcR);
        endswap<double>(&fcT);
        endswap<double>(&fcEP0);
        endswap<double>(&fcEP1);


        lineBuf.append((const char*) &icR, sizeof(double));
        lineBuf.append((const char*) &icT, sizeof(double));
        lineBuf.append((const char*) &icEP0, sizeof(double));
        lineBuf.append((const char*) &icEP1, sizeof(double));
        lineBuf.append((const char*) &houghIndex, sizeof(int));
        lineBuf.append((const char*) &fieldIndex, sizeof(int));
        lineBuf.append((const char*) &fcR, sizeof(double));
        lineBuf.append((const char*) &fcT, sizeof(double));
        lineBuf.append((const char*) &fcEP0, sizeof(double));
        lineBuf.append((const char*) &fcEP1, sizeof(double));

        if (debugLines)
        std::cout << line.print() << std::endl;
    }

    if (debugLines)
    std::cout << std::endl << "Hough lines in field coordinates:" << std::endl;

    int i = 0;
    for (auto it = lineList->begin(); it != lineList->end(); it++) {
        man::vision::HoughLine& line = *it;
        if (debugLines)
        std::cout << line.field().print() << std::endl;
    }

    if (debugLines) {
        std::cout << std::endl << "Field lines:" << std::endl;
        std::cout << "0.idx, 1.idx, id, idx" << std::endl;
    }
    man::vision::FieldLineList* fieldLineList = module.getFieldLines(topCamera);

    for (int i = 0; i < fieldLineList->size(); i++) {
        man::vision::FieldLine& line = (*fieldLineList)[i];
        if (debugLines)
        std::cout << line.print() << std::endl;
    }

    if (debugLines)
    std::cout << std::endl << "Goalbox and corner detection:" << std::endl;
    man::vision::GoalboxDetector* box = module.getBox(topCamera);
    man::vision::CornerDetector* corners = module.getCorners(topCamera);

    if (debugLines) {
        if (box->first != NULL)
        std::cout << box->print() << std::endl;
    }

    if (debugLines) {
        std::cout << "    line0, line1, type (concave, convex, T)" << std::endl;
        for (int i = 0; i < corners->size(); i++) {
            const man::vision::Corner& corner = (*corners)[i];
            std::cout << corner.print() << std::endl;
        }
    }

    retVec.push_back(Block{lineBuf, json::Object{}, "lineBuffer", "nbcross", 0, 0});

    logptr retLog = Log::explicitLog(retVec, json::Object{}, "VisionReturn");
    retLog->topLevelDictionary["CalibrationSuccess"] = json::Boolean(success);
    RETURN(retLog);
}

#endif
