#pragma once

#include "Nao.hpp"
#include <string>
#include "blackboard/Adapter.hpp"

class NaturalLandmarksTransmitter : Adapter, NaoTransmitter {
   public:
      /**
       * Constructor.  Opens a socket for listening.
       */
      NaturalLandmarksTransmitter(Blackboard *blackboard);

      /**
       * One cycle of this thread
       */
      void tick();
};
