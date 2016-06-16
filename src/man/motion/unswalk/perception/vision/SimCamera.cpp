#include "Oracle.h"
#include "utils/Logger.hpp"
#include "perception/vision/SimCamera.hpp"

using namespace std;

SimCamera::SimCamera() {
   imageSize = oracle->getImageWidth() * oracle->getImageHeight() * 8;
   llog(INFO) << "Sim Camera initialised" << endl;
}

SimCamera::~SimCamera() {
   llog(INFO) << "Sim Camera destroyed" << endl;
}

const uint8_t* SimCamera::get(const int colourSpace) {
   const uint8_t* image = oracle->getImage();
   writeFrame(image);
   return image;
}

bool SimCamera::setCamera(WhichCamera whichCamera) {
   return true;
}

WhichCamera SimCamera::getCamera() {
   return BOTTOM_CAMERA;
}
