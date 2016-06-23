#pragma once

#include <string>

#include "perception/vision/Vision.hpp"
#include "utils/Timer.hpp"
#include "blackboard/Adapter.hpp"
#include "VisualOdometry.hpp"

namespace VisionTest
{
   class FrameResult;
   class DumpResult;
}

/* Adapter that allows Vision to communicate with the Blackboard */
class VisionAdapter : Adapter {
   public:
   /* Constructor */
   VisionAdapter(Blackboard *bb);
   /* Destructor */
   ~VisionAdapter();
   /* One cycle of this thread */
   void tick();
   private:
   /* Vision module instance */
   Vision V;
   /* A wall clock */
   Timer timer;

   friend class VisionTest::DumpResult;
   friend class VisionTest::FrameResult;
   friend class PerceptionThread;
};
