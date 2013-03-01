#include "VisionModule.h"
#include <iostream>


using namespace portals;


namespace man{
namespace newVis{

VisionModule::VisionModule() : Module()
{
  newVis = new newVision();
}
  
  void VisionModule::run_()
  {
    topImageIn.latch();
    bottomImageIn.latch();
    std::cout << "the topImageIn is latched!\n";
    newVis->memory->setImages_old(topImageIn.message().get_image(), bottomImageIn.message().get_image());
    newVis->visionLoop();
  }
  
  
}
}
