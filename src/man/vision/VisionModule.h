#pragma once


#include "RoboGrams.h"
#include "ThresholdedImage.h"
#include "JointAngles.pb.h"
#include "InertialState.pb.h"
#include "Vision.h"
#include <boost/shared_ptr.hpp>


namespace man {
  namespace vision{


    class VisionModule : public portals::Module {

    public:
      VisionModule();
      virtual ~VisionModule();

      portals::InPortal<messages::ThresholdedImage> topImageIn;
      portals::InPortal<messages::ThresholdedImage> bottomImageIn;
      portals::InPortal<messages::JointAngles> joint_angles;
      portals::InPortal<messages::InertialState> inertial_state;

    protected:
      virtual void run_();
      boost::shared_ptr<Vision> vision;
      boost::shared_ptr<NaoPose> pose;
    };
  }
}
 
