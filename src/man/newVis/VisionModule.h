#pragma once


#include "RoboGrams.h"
#include "ThresholdedImage.h"
#include "newVision.h"
#include <boost/shared_ptr.hpp>


namespace man {
  namespace newVis{


    class VisionModule : public portals::Module {

    public:
      VisionModule();
      virtual ~VisionModule();

      portals::InPortal<messages::ThresholdedImage> topImageIn;
      portals::InPortal<messages::ThresholdedImage> bottomImageIn;
      
    protected:
      virtual void run_();
      newVision* newVis;
      
    };
  }
}
