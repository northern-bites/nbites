#include "nbfuncs.h"

#include "RoboGrams.h"
#include "Images.h"
#include "vision/VisionModule.h"
#include "vision/FrontEnd.h"
#include "vision/Homography.h"
#include "ParamReader.h"
#include "NBMath.h"

#include <cstdlib>
#include <netinet/in.h>
#include <assert.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

using nblog::Log;
using nblog::SExpr;

//man::vision::Colors* getColorsFromSExpr(SExpr* params);
void getCalibrationOffsets(Log* l, double* r, double* p, int w, int h, bool t);
void updateSavedColorParams(std::string sexpPath, SExpr* params, bool top);
SExpr getSExprFromSavedParams(int color, std::string sexpPath, bool top);
std::string getSExprStringFromColorJSonNode(boost::property_tree::ptree tree);
SExpr treeFromBall(man::vision::Ball& b);
SExpr treeFromBlob(man::vision::Blob& b);

messages::YUVImage emptyTop(
    man::vision::DEFAULT_TOP_IMAGE_WIDTH * 2,
    man::vision::DEFAULT_TOP_IMAGE_HEIGHT
);

messages::YUVImage emptyBot(
    man::vision::DEFAULT_TOP_IMAGE_WIDTH,
    man::vision::DEFAULT_TOP_IMAGE_HEIGHT / 2
);

void imageSizeCheck(bool top, int width, int height) {
    if (top) {
        if (width != 2 * man::vision::DEFAULT_TOP_IMAGE_WIDTH ||
            height != man::vision::DEFAULT_TOP_IMAGE_HEIGHT ) {
            printf("WARNING! topCamera dimensions (%i, %i) NOT DEFAULT, VisionModule results undefined!\n",
                   width, height);
        }
    } else {
        //bot
        if ( // 2 / 2 == 1
            width != man::vision::DEFAULT_TOP_IMAGE_WIDTH ||
            height != ( man::vision::DEFAULT_TOP_IMAGE_HEIGHT / 2 ) ) {
            printf("WARNING! botCamera dimensions (%i, %i) NOT DEFAULT, VisionModule results undefined!\n",
                   width, height);
        }
    }
}

//robotName may be empty ("").
man::vision::VisionModule& getModuleRef(const std::string robotName);

int Vision_func() {

    assert(args.size() == 1);

    printf("Vision_func()\n");

    Log* copy = new Log(args[0]);
    size_t length = copy->data().size();
    uint8_t buf[length];
    memcpy(buf, copy->data().data(), length);

    // Parse YUVImage S-expression
    // Determine if we are looking at a top or bottom image from log description
    bool topCamera = copy->description().find("camera_TOP") != std::string::npos;
    int width, height;
    std::vector<SExpr*> vec = copy->tree().recursiveFind("width");
    if (vec.size() != 0) {
        SExpr* s = vec.at(vec.size()-1);
        width = 2*s->get(1)->valueAsInt();
    } else {
        std::cout << "Could not get width from description!\n";
    }
    vec = copy->tree().recursiveFind("height");
    if (vec.size() != 0) {
        SExpr* s = vec.at(vec.size()-1);
        height = s->get(1)->valueAsInt();
    } else {
        std::cout << "Could not get height from description!\n";
    }
    
    imageSizeCheck(topCamera, width, height);

    // Location of lisp text file with color params
    std::string sexpPath = std::string(getenv("NBITES_DIR"));
    sexpPath += "/src/man/config/colorParams.txt";

    // Read number of bytes of image, inertials, and joints if exist
    messages::JointAngles joints;
    if (copy->tree().find("contents")->get(2)) {
        int numBytes[3];
        for (int i = 0; i < 3; i++)
            numBytes[i] = atoi(copy->tree().find("contents")->get(i+1)->
                                            find("nbytes")->get(1)->value().c_str());
        uint8_t* ptToJoints = buf + (numBytes[0] + numBytes[1]);
        joints.ParseFromArray((void *) ptToJoints, numBytes[2]);
    }

    // If log includes robot name (which it always should), pass to module
    SExpr* robotName = args[0]->tree().find("from_address");
    std::string rname;
    if (robotName != NULL) {
        rname = robotName->get(1)->value();
    }

    //man::vision::VisionModule module(width / 2, height, rname);
    man::vision::VisionModule& module = getModuleRef(rname);

    // Images to pass to vision module, top & bottom
    messages::YUVImage realImage(buf, width, height, width);

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

    // If log includes color parameters in description, have module use those
    SExpr* colParams = args[0]->tree().find("Params");
    if (colParams != NULL) {

        // Set new parameters as frontEnd colorParams
        man::vision::Colors* c = module.getColorsFromLisp(colParams, 2);
        module.setColorParams(c, topCamera);

        // Look for atom value "SaveParams", i.e. "save" button press
        SExpr* save = colParams->get(1)->find("SaveParams");
        if (save != NULL) {
            // Save attached parameters to txt file
            updateSavedColorParams(sexpPath, colParams, topCamera);
        }
    }

    // If log includes calibration parameters in description, have module use those
    std::vector<SExpr*> calParamsVec = args[0]->tree().recursiveFind("CalibrationParams");
    if (calParamsVec.size() != 0) {
        SExpr* calParams = calParamsVec.at(calParamsVec.size()-2);
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

	// if log specified debug drawing parameters then set them
	SExpr* debugDrawing = args[0]->tree().find("DebugDrawing");
	if (debugDrawing != NULL) {
		module.setDebugDrawingParameters(debugDrawing);
	}

    // If log includes "BlackStar," set flag
    std::vector<SExpr*> blackStarVec = args[0]->tree().recursiveFind("BlackStar");
    if (blackStarVec.size() != 0)
        module.blackStar(true);
    
    // Run it!
    module.run();

    // -----------
    //   Y IMAGE
    // -----------
    man::vision::ImageFrontEnd* frontEnd = module.getFrontEnd(topCamera);

    Log* yRet = new Log();
    int yLength = (width / 4) * (height / 2) * 2;

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
    int whiteLength = (width / 4) * (height / 2);

    // Create temp buffer and fill with white image 
    uint8_t whiteBuf[whiteLength];
    memcpy(whiteBuf, frontEnd->whiteImage().pixelAddr(), whiteLength);

    // Convert to string and set log
    std::string whiteBuffer((const char*)whiteBuf, whiteLength);
    whiteRet->setData(whiteBuffer);

    // Read params from Lisp and attach to image 
    whiteRet->setTree(getSExprFromSavedParams(0, sexpPath, topCamera));

    rets.push_back(whiteRet);

    // ---------------
    //   GREEN IMAGE
    // ---------------
    Log* greenRet = new Log();
    int greenLength = (width / 4) * (height / 2);

    // Create temp buffer and fill with gree image 
    uint8_t greenBuf[greenLength];
    memcpy(greenBuf, frontEnd->greenImage().pixelAddr(), greenLength);

    // Convert to string and set log
    std::string greenBuffer((const char*)greenBuf, greenLength);
    greenRet->setData(greenBuffer);

    // Read params from JSon and attach to image 
    greenRet->setTree(getSExprFromSavedParams(1, sexpPath, topCamera));

    rets.push_back(greenRet);

    // ----------------
    //   ORANGE IMAGE
    // ----------------
    Log* orangeRet = new Log();
    int orangeLength = (width / 4) * (height / 2);

    // Create temp buffer and fill with orange image 
    uint8_t orangeBuf[orangeLength];
    memcpy(orangeBuf, frontEnd->orangeImage().pixelAddr(), orangeLength);

    // Convert to string and set log
    std::string orangeBuffer((const char*)orangeBuf, orangeLength);
    orangeRet->setData(orangeBuffer);

    // Read params from JSon and attach to image 
    SExpr oTree = getSExprFromSavedParams(2, sexpPath, topCamera);
    oTree.append(SExpr::keyValue("width", width / 4));
    oTree.append(SExpr::keyValue("height", height / 2));

    orangeRet->setTree(oTree);

    rets.push_back(orangeRet);

    //-------------------
    //  SEGMENTED IMAGE
    //-------------------
    Log* colorSegRet = new Log();
    int colorSegLength = (width / 4) * (height / 2);

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
    
    // Uncomment to display rejected edges used in center detection
    // man::vision::EdgeList* edgeList = module.getRejectedEdges(topCamera);

    Log* edgeRet = new Log();
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

    edgeRet->setData(edgeBuf);
    rets.push_back(edgeRet);

    //-------------------
    //  LINES
    //-------------------
    man::vision::HoughLineList* lineList = module.getHoughLines(topCamera);

    Log* lineRet = new Log();
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

    lineRet->setData(lineBuf);
    rets.push_back(lineRet);

    //-----------
    //  BALL
    //-----------
    man::vision::BallDetector* detector = module.getBallDetector(topCamera);

    Log* ballRet = new Log();
    std::vector<man::vision::Ball> balls = detector->getBalls();
    std::list<man::vision::Blob> blobs = detector->getBlobber()->blobs;

    SExpr allBalls;
    int count = 0;
    for (auto i=balls.begin(); i!=balls.end(); i++) {
        SExpr ballTree = treeFromBall(*i);
        SExpr next = SExpr::keyValue("ball" + std::to_string(count), ballTree);
        allBalls.append(next);
        count++;
    }
    count = 0;
    for (auto i=blobs.begin(); i!=blobs.end(); i++) {
        SExpr blobTree = treeFromBlob(*i);
        SExpr next = SExpr::keyValue("blob" + std::to_string(count), blobTree);
        allBalls.append(next);
        count++;
    }

    ballRet->setTree(allBalls);
    rets.push_back(ballRet);

    //---------------
    // Center Circle
    //---------------

    man::vision::CenterCircleDetector* ccd = module.getCCD(topCamera);
    Log* ccdRet = new Log();
    std::string pointsBuf;

    std::vector<std::pair<double, double>> points = ccd->getPotentials();
    for (std::pair<double, double> p : points) {
        endswap<double>(&(p.first));
        endswap<double>(&(p.second));
        pointsBuf.append((const char*) &(p.first), sizeof(double));
        pointsBuf.append((const char*) &(p.second), sizeof(double));
    }

    // Add 0,0 point if cc if off so tool doesn't display a rejected one
    std::pair<double, double> zero(0.0, 0.0);
    if (!ccd->on()) {
        pointsBuf.append((const char*) &(zero.first), sizeof(double));
        pointsBuf.append((const char*) &(zero.second), sizeof(double));
    }

    ccdRet->setData(pointsBuf);
    rets.push_back(ccdRet);

	std::cout << "Debug image" << std::endl;
    //-------------------
    //  DEBUG IMAGE
    //-------------------
    Log* debugImage = new Log();
    int debugImageLength = (width / 2) * (height / 2);

    // Create temp buffer and fill with debug image
    uint8_t debBuf[debugImageLength];
    memcpy(debBuf, module.getDebugImage(topCamera)->pixArray(), debugImageLength);

    // Convert to string and set log
    std::string debBuffer((const char*)debBuf, debugImageLength);
    debugImage->setData(debBuffer);

    rets.push_back(debugImage);

    return 0;
}

int CameraCalibration_func() {
    printf("CameraCalibrate_func()\n");

    int failures = 0;
    double totalR = 0;
    double totalT = 0;
    
    man::vision::VisionModule& module = getModuleRef("");

    // Repeat for each log
    for (int i = 0; i < 7; i++) {
        module.reset();
        
        Log* l = new Log(args[i]);

        size_t length = l->data().size();
        uint8_t buf[length];
        memcpy(buf, l->data().data(), length);

        // Determine description
        bool top = l->description().find("camera_TOP") != std::string::npos;
        
        int width = 2*atoi(l->tree().find("contents")->get(1)->
                                        find("width")->get(1)->value().c_str());
        int height = atoi(l->tree().find("contents")->get(1)->
                                       find("height")->get(1)->value().c_str());

        imageSizeCheck(top, width, height);
        
        double rollChange, pitchChange;

        // Read number of bytes of image, inertials, and joints if exist
        int numBytes[3];
        std::vector<SExpr*> vec = l->tree().recursiveFind("YUVImage");
        if (vec.size() != 0) {
            SExpr* s = vec.at(vec.size()-2)->find("nbytes");
            if (s != NULL) {
                numBytes[0] = s->get(1)->valueAsInt();
            }
        }

        vec = l->tree().recursiveFind("InertialState");
        if (vec.size() != 0) {
            SExpr* s = vec.at(vec.size()-2)->find("nbytes");
            if (s != NULL) {
                numBytes[1] = s->get(1)->valueAsInt();
            }
        }

        messages::JointAngles joints;
        vec = l->tree().recursiveFind("JointAngles");
        if (vec.size() != 0) {
            SExpr* s = vec.at(vec.size()-2)->find("nbytes");
            if (s != NULL) {
                numBytes[2] = s->get(1)->valueAsInt();
                uint8_t* ptToJoints = buf + (numBytes[0] + numBytes[1]);
                joints.ParseFromArray((void *) ptToJoints, numBytes[2]);
            } else {
                std::cout << "Could not load joints from description.\n";
                rets.push_back(new Log("((failure))"));
                return 0;
            }
        }

        // If log includes "BlackStar," set flag
        std::vector<SExpr*> blackStarVec = args[0]->tree().recursiveFind("BlackStar");
        if (blackStarVec.size() != 0)
            module.blackStar(true);
        
        // Create messages
        messages::YUVImage image(buf, width, height, width);
        portals::Message<messages::YUVImage> imageMessage(&image);
        portals::Message<messages::JointAngles> jointsMessage(&joints);

        if (top) {
            portals::Message<messages::YUVImage> emptyMessage(&emptyBot);
            module.topIn.setMessage(imageMessage);
            module.bottomIn.setMessage(emptyMessage);
        } else {
            portals::Message<messages::YUVImage> emptyMessage(&emptyTop);
            module.topIn.setMessage(emptyMessage);
            module.bottomIn.setMessage(imageMessage);
        }
        
        module.jointsIn.setMessage(jointsMessage);

        module.run();

        man::vision::FieldHomography* fh = module.getFieldHomography(top);

        double rollBefore, tiltBefore, rollAfter, tiltAfter;

        rollBefore = fh->roll();
        tiltBefore = fh->tilt();

        std::cout << "Calibrating log " << i+1 << ": "; 

        bool success = fh->calibrateFromStar(*module.getFieldLines(top));

        if (!success) {
            failures++;
        } else {
            rollAfter = fh->roll();
            tiltAfter = fh->tilt();
            totalR += rollAfter - rollBefore;
            totalT += tiltAfter - tiltBefore;
        }
    }

    if (failures > 4) {
        // Handle failure
        printf("Failed calibration %d times\n", failures);
        rets.push_back(new Log("(failure)"));
    } else {
        printf("Success calibrating %d times\n", 7 - failures);

        totalR /= (args.size() - failures);
        totalT /= (args.size() - failures);

        // Pass back averaged offsets to Tool
        std::string sexp = "((roll " + std::to_string(totalR) + ")(tilt " + std::to_string(totalT) + "))";
        rets.push_back(new Log(sexp));
    }
}


int Synthetics_func() {
    assert(args.size() == 1);

    printf("Synthetics_func()\n");
    
    double x = std::stod(args[0]->tree().find("contents")->get(1)->find("params")->get(1)->value().c_str());
    double y = std::stod(args[0]->tree().find("contents")->get(1)->find("params")->get(2)->value().c_str());
    double h = std::stod(args[0]->tree().find("contents")->get(1)->find("params")->get(3)->value().c_str());
    bool fullres = args[0]->tree().find("contents")->get(1)->find("params")->get(4)->value() == "true";
    bool top = args[0]->tree().find("contents")->get(1)->find("params")->get(5)->value() == "true";

    int wd = (fullres ? 320 : 160);
    int ht = (fullres ? 240 : 120);
    double flen = (fullres ? 544 : 272);

    int size = wd*4*ht*2;
    uint8_t pixels[size];
    man::vision::YuvLite synthetic(wd, ht, wd*4, pixels);

    man::vision::FieldHomography homography(top);
    homography.wx0(x);
    homography.wy0(y);
    homography.azimuth(h*TO_RAD);
    homography.flen(flen);

    man::vision::syntheticField(synthetic, homography);

    std::string sexpr("(nblog (version 6) (contents ((type YUVImage) ");
    sexpr += top ? "(from camera_TOP) " : "(from camera_BOT) ";
    sexpr += "(nbytes ";
    sexpr += std::to_string(size);
    sexpr += ") (width " + std::to_string(2*wd);
    sexpr += ") (height " + std::to_string(2*ht);
    sexpr += ") (encoding \"[Y8(U8/V8)]\"))))";

    Log* log = new Log(sexpr);
    std::string buf((const char*)pixels, size);
    log->setData(buf);

    rets.push_back(log);
}

// Save the new color params to the colorParams.txt file
void updateSavedColorParams(std::string sexpPath, SExpr* params, bool top) {
    std::cout << "Saving params!" << std::endl;
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

SExpr treeFromBall(man::vision::Ball& b)
{
    SExpr x(b.x_rel);
    SExpr y(b.y_rel);
    SExpr p = SExpr::list({x, y});
    SExpr bl = treeFromBlob(b.getBlob());

    SExpr rel = SExpr::keyValue("rel", p);
    SExpr blob = SExpr::keyValue("blob", bl);
    SExpr exDiam = SExpr::keyValue("expectedDiam", b.expectedDiam);
    SExpr toRet = SExpr::list({rel, blob, exDiam});

    return toRet;
}

SExpr treeFromBlob(man::vision::Blob& b)
{
    SExpr x(b.centerX());
    SExpr y(b.centerY());
    SExpr p = SExpr::list({x, y});

    SExpr center = SExpr::keyValue("center", p);
    SExpr area = SExpr::keyValue("area", b.area());
    SExpr count = SExpr::keyValue("count", b.count());
    SExpr len1 = SExpr::keyValue("len1", b.firstPrincipalLength());
    SExpr len2 = SExpr::keyValue("len2", b.secondPrincipalLength());
    SExpr ang1 = SExpr::keyValue("ang1", b.firstPrincipalAngle());
    SExpr ang2 = SExpr::keyValue("ang2", b.secondPrincipalAngle());
    SExpr toRet = SExpr::list({center, area, count, len1, len2, ang1, ang2});

    return toRet;
}

int Scratch_func() {}

#include <map>
std::map<const std::string, man::vision::VisionModule *> vmRefMap;

man::vision::VisionModule& getModuleRef(const std::string robotName) {
    if (vmRefMap.find(robotName) != vmRefMap.end()) {
        printf("nbcross-getModuleRef REUSING MODULE [%s]\n",
               robotName.c_str() );
		man::vision::VisionModule* module = vmRefMap[robotName];
		module->reset();
        return *module;
        
    } else {
        printf("nbcross-getModuleRef CREATING NEW MODULE [%s]\n",
               robotName.c_str() );
        man::vision::VisionModule * newInst =
            new man::vision::VisionModule(man::vision::DEFAULT_TOP_IMAGE_WIDTH,
                                      man::vision::DEFAULT_TOP_IMAGE_HEIGHT, robotName);
        vmRefMap[robotName] = newInst;
        return *newInst;
    }
}
