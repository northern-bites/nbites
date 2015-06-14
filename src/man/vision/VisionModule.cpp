#include "VisionModule.h"
#include "Edge.h"
#include "HighResTimer.h"
#include "NBMath.h"

#include <iostream>

namespace man {
namespace vision {

// TODO constants and parameters
VisionModule::VisionModule()
    : Module(),
      topIn(),
      bottomIn(),
      jointsIn()
{
    // NOTE Constructed on heap because some of the objects below do
    //      not have default constructors, all class members must be initialized
    //      after the initializer list is run, which requires calling default
    //      constructors in the case of C-style arrays, limitation theoretically

    //      removed in C++11

    // TODO: RobotPath to config folder
    std:: string sexpPath;
    #ifdef OFFLINE
        sexpPath = std::string(getenv("NBITES_DIR"));
        sexpPath += "/src/man/config/colorParams.txt";
    #else
        sexPath = ''; // TODO
    #endif

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
    nblog::SExpr* colors = nblog::SExpr::read((const std::string)sexpText);
    
    // Set module pointers for top then bottom images
    for (int i = 0; i < 2; i++) {
        colorParams[i] = getColorsFromLisp(colors, i);
        frontEnd[i] = new ImageFrontEnd();
        edgeDetector[i] = new EdgeDetector();
        edges[i] = new EdgeList(32000);
        houghLines[i] = new HoughLineList(128);
        kinematics[i] = new Kinematics(i == 0);
        homography[i] = new FieldHomography();
        fieldLines[i] = new FieldLineList();
        boxDetector[i] = new GoalboxDetector();

        // TODO set width and height dynamically
        if (i == 0) {
          hough[i] = new HoughSpace(320, 240);
          cornerDetector[i] = new CornerDetector(320, 240);
        } else {
          hough[i] = new HoughSpace(160, 120);
          cornerDetector[i] = new CornerDetector(160, 120);
        }

        // TODO flag
        bool fast = true;
        frontEnd[i]->fast(false);
        edgeDetector[i]->fast(fast);
        hough[i]->fast(fast);
    }
}

VisionModule::~VisionModule()
{
    for (int i = 0; i < 2; i++) {
        delete colorParams[i];
        delete frontEnd[i];
        delete edgeDetector[i];
        delete edges[i];
        delete houghLines[i];
        delete hough[i];
        delete kinematics[i];
        delete homography[i];
        delete fieldLines[i];
    }
}

// TODO bug in assembly front end green image
// TODO use horizon on top image
void VisionModule::run_()
{
    // Get messages from inPortals
    topIn.latch();
    bottomIn.latch();
    jointsIn.latch();

    // Setup
    std::vector<const messages::YUVImage*> images { &topIn.message(),
                                                    &bottomIn.message() };

    // Time vision module
    double topTimes[6];
    double bottomTimes[6];
    double* times[2] = { topTimes, bottomTimes };

    // Loop over top and bottom image and run line detection system
    for (int i = 0; i < images.size(); i++) {
        // Get image
        const messages::YUVImage* image = images[i];

        // Construct YuvLite object for use in vision system
        YuvLite yuvLite(image->width() / 4,
                        image->height() / 2,
                        image->rowPitch(),
                        image->pixelAddress(0, 0));

        HighResTimer timer;

        // Run front end
        frontEnd[i]->run(yuvLite, colorParams[i]);
        ImageLiteU16 yImage(frontEnd[i]->yImage());
        ImageLiteU8 greenImage(frontEnd[i]->greenImage());

        times[i][0] = timer.end();

        // Calculate kinematics and adjust homography
        std::cout << "Top camera: " << (i == 0) << std::endl;
        kinematics[i]->joints(jointsIn.message());
        homography[i]->wz0(kinematics[i]->wz0());
        homography[i]->tilt(kinematics[i]->tilt() - 3.965*TO_RAD);

        std::cout << "tilt: " << kinematics[i]->tilt() << std::endl;
        homography[i]->roll(homography[i]->roll()-2.21*TO_RAD);
        // homography[i]->azimuth(kinematics[i]->azimuth());

        // Approximate brightness gradient
        edgeDetector[i]->gradient(yImage);
        
        times[i][1] = timer.end();

        // Run edge detection
        edgeDetector[i]->edgeDetect(greenImage, *(edges[i]));

        times[i][2] = timer.end();

        // Run hough line detection
        hough[i]->run(*(edges[i]), *(houghLines[i]));

        times[i][3] = timer.end();

        // Find field lines
        houghLines[i]->mapToField(*(homography[i]));
        fieldLines[i]->find(*(houghLines[i]));

        times[i][4] = timer.end();

        // Classify field lines
        fieldLines[i]->classify(*(boxDetector[i]), *(cornerDetector[i]));

        times[i][5] = timer.end();
    }

    
    for (int i = 0; i < 2; i++) {
        if (i == 0)
            std::cout << "From top camera:" << std::endl;
        else
            std::cout << std::endl << "From bottom camera:" << std::endl;
        std::cout << "Front end: " << times[i][0] << std::endl;
        std::cout << "Gradient: " << times[i][1] << std::endl;
        std::cout << "Edge detection: " << times[i][2] << std::endl;
        std::cout << "Hough: " << times[i][3] << std::endl;
        std::cout << "Field lines detection: " << times[i][4] << std::endl;
        std::cout << "Field lines classification: " << times[i][5] << std::endl;
    }
    

}

/*
 Lisp data in config/colorParams.txt stores 32 parameters. Read lisp and
  load the three compoenets of a Colors struct, white, green, and orange,
  from the 18 values for either the top or bottom image. 
*/
Colors* VisionModule::getColorsFromLisp(nblog::SExpr* colors, int camera) {
    Colors* ret = new man::vision::Colors;
    nblog::SExpr* params;

    if (camera == 0) {
        params = colors->get(1)->find("Top")->get(1);
    } else if (camera == 1) {
        params = colors->get(1)->find("Bottom")->get(1);
    } else {
        params = colors->get(1);
    }

    colors = params->get(0)->get(1);

    
    ret->white. load(std::stof(colors->get(0)->get(1)->serialize()),
                     std::stof(colors->get(1)->get(1)->serialize()),
                     std::stof(colors->get(2)->get(1)->serialize()),
                     std::stof(colors->get(3)->get(1)->serialize()),
                     std::stof(colors->get(4)->get(1)->serialize()),
                     std::stof(colors->get(5)->get(1)->serialize())); 
    
    colors = params->get(1)->get(1);

    ret->green. load(std::stof(colors->get(0)->get(1)->serialize()),
                     std::stof(colors->get(1)->get(1)->serialize()),
                     std::stof(colors->get(2)->get(1)->serialize()),
                     std::stof(colors->get(3)->get(1)->serialize()),
                     std::stof(colors->get(4)->get(1)->serialize()),
                     std::stof(colors->get(5)->get(1)->serialize()));  
    
    colors = params->get(2)->get(1);

    ret->orange.load(std::stof(colors->get(0)->get(1)->serialize()),
                     std::stof(colors->get(1)->get(1)->serialize()),
                     std::stof(colors->get(2)->get(1)->serialize()),
                     std::stof(colors->get(3)->get(1)->serialize()),
                     std::stof(colors->get(4)->get(1)->serialize()),
                     std::stof(colors->get(5)->get(1)->serialize()));

    return ret;
}

}
}
