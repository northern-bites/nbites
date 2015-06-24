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


int Vision_func() {
    assert(args.size() == 1);

    printf("Vision_func()\n");

    Log* copy = new Log(args[0]);
    size_t length = copy->data().size();
    uint8_t buf[length];
    memcpy(buf, copy->data().data(), length);

    // Parse YUVImage S-expression
   
    // Determine if we are looking at a top or bottom image from log description
    bool top = copy->description().find("TOP") != std::string::npos;
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
    
    // Create messages
    // Empty messages to pass to Vision Module so it doesn't freak
    messages::YUVImage image(buf, width, height, width);

    portals::Message<messages::YUVImage> imageMessage(&image);
    portals::Message<messages::JointAngles> jointsMessage(&joints);

    
    // Look for robot name and pass to module if found
    SExpr* robotName = args[0]->tree().find("from_address");
    std::string rname;
    if (robotName != NULL) {
        rname = robotName->get(1)->value();
    }

    man::vision::VisionModule module(width / 2, height, rname);

    module.topIn.setMessage(imageMessage);
    module.bottomIn.setMessage(imageMessage);
    module.jointsIn.setMessage(jointsMessage);

    // If log includes color parameters in description, have module use those
    SExpr* colParams = args[0]->tree().find("Params");
    if (colParams != NULL) {

        // Set new parameters as frontEnd colorParams
        man::vision::Colors* c = module.getColorsFromLisp(colParams, 2);
        module.setColorParams(c, top);

        // Look for atom value "SaveParams", i.e. "save" button press
        SExpr* save = colParams->get(1)->find("SaveParams");
        if (save != NULL) {

            // Save attached parameters to txt file
            updateSavedColorParams(sexpPath, colParams, top);
        }
    }

    // If log includes calibration parameters in description, have madule use those
    std::vector<SExpr* > calParamsVec = args[0]->tree().recursiveFind("CalibrationParams");
    if (calParamsVec.size() != 0) {
        SExpr* calParams = calParamsVec.at(calParamsVec.size()-2);
        calParams = top ? calParams->find("camera_TOP") : calParams->find("camera_BOT");
        if (calParams != NULL) {
            std::cout << "Found and using calibration params in log description: "
            << "Roll: " << calParams->get(1)->valueAsDouble() << " Tilt: " <<  calParams->get(2)->valueAsDouble()<< std::endl;
            man::vision::CalibrationParams* ncp =
            new man::vision::CalibrationParams(calParams->get(1)->valueAsDouble(),
                                           calParams->get(2)->valueAsDouble());

            module.setCalibrationParams(ncp, top);
        }
    }

    module.run();
    man::vision::ImageFrontEnd* frontEnd = module.getFrontEnd(top);

    // -----------
    //   Y IMAGE
    // -----------
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
    whiteRet->setTree(getSExprFromSavedParams(0, sexpPath, top));

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
    greenRet->setTree(getSExprFromSavedParams(1, sexpPath, top));

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
    orangeRet->setTree(getSExprFromSavedParams(2, sexpPath, top));

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
    man::vision::EdgeList* edgeList = module.getEdges(top);

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
    man::vision::HoughLineList* lineList = module.getHoughLines(top);

    Log* lineRet = new Log();
    std::string lineBuf;

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

        std::cout << line.print() << std::endl;
    }

    std::cout << std::endl << "Hough lines in field coordinates:" << std::endl;
    
    for (auto it = lineList->begin(); it != lineList->end(); it++) {
        man::vision::HoughLine& line = *it;

        std::cout << line.field().print() << std::endl;
    }

    std::cout << std::endl << "Field lines:" << std::endl;
    man::vision::FieldLineList* fieldLineList = module.getFieldLines(top);
    for (int i = 0; i < fieldLineList->size(); i++) {
        man::vision::FieldLine& line = (*fieldLineList)[i];
        std::cout << line.print() << std::endl;
    }

    std::cout << std::endl << "Goalbox and corner detection:" << std::endl;
    man::vision::GoalboxDetector* box = module.getBox(top);
    man::vision::CornerDetector* corners = module.getCorners(top);
    if (box->first != NULL)
        std::cout << box->print() << std::endl;
    for (int i = 0; i < corners->size(); i++) {
        const man::vision::Corner& corner = (*corners)[i];
        std::cout << corner.print() << std::endl;
    }

    lineRet->setData(lineBuf);
    rets.push_back(lineRet);

    // std::cout << "SCRATCH" << std::endl;
    // man::vision::GeoLine test1;
    // test1.set(75, M_PI / 2, -45, 15);
    // std::cout << test1.print() << std::endl;

    // test1.translateRotate(0, 0, -(M_PI / 2));
    // test1.translateRotate(300, 200, (M_PI));
    // std::cout << test1.print() << std::endl;

    // std::cout << "SCRATCH" << std::endl;
    // man::vision::GeoLine test2;
    // test2.set(100, M_PI / 2, -50, 50);
    // std::cout << test2.print() << std::endl;

    // test2.translateRotate(0, 0, -(M_PI / 2));
    // test2.translateRotate(400, 200, (M_PI / 2));
    // std::cout << test2.print() << std::endl;

    return 0;
}

int CameraCalibration_func() {
    printf("CameraCalibrate_func()\n");

    int failures = 0;
    double totalR = 0;
    double totalT = 0;

    // Repeat for each log
    for (int i = 0; i < 7; i++) {

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

        double rollChange, pitchChange;
        
        // Init vision module with offsets of 0.0
        man::vision::VisionModule module(width / 2, height);


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
        
        // Create messages
        messages::YUVImage image(buf, width, height, width);
        portals::Message<messages::YUVImage> imageMessage(&image);
        portals::Message<messages::JointAngles> jointsMessage(&joints);

        module.topIn.setMessage(imageMessage);
        module.bottomIn.setMessage(imageMessage);
        module.jointsIn.setMessage(jointsMessage);

        module.run();

        man::vision::FieldHomography* fh = module.getFieldHomography(top);

        double rollBefore, tiltBefore, rollAfter, tiltAfter;

        rollBefore = fh->roll();
        tiltBefore = fh->tilt();

        bool success = fh->calibrateFromStar(*module.getFieldLines(top));

        if (!success) {
            failures++;
        } else {
            rollAfter = fh->roll();
            tiltAfter = fh->tilt();

            totalR += rollAfter - rollBefore;
            totalT += tiltAfter - tiltBefore;

        //    std::cout << "Tilt before: " << tiltBefore << " Tilt after: " << tiltAfter << std::endl;
        }
    }

    if (failures > 2) {
        // Handle failure
        printf("FAILED: %d times\n", failures);
        rets.push_back(new Log("(failure)"));
    } else {
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
    uint8_t* pixels = new uint8_t[size];
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

int Scratch_func() {}


// Save the new color params to the colorParams.txt file
void updateSavedColorParams(std::string sexpPath, SExpr* params, bool top) {
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