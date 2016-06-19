#include "generator/ClippedGenerator.hpp"
#include "libagent/AgentData.hpp"
#include "utils/body.hpp"
#include "utils/clip.hpp"
#include "utils/Logger.hpp"

using boost::program_options::variables_map;

ClippedGenerator::ClippedGenerator(Generator* g)
   : generator(g),
     old_exists(false) {
   std::cout << "ClippedGenerator constructed" << std::endl;
}

ClippedGenerator::~ClippedGenerator() {
   delete generator;
   std::cout << "ClippedGenerator destroyed" << std::endl;
}

bool ClippedGenerator::isActive() {
   return generator->isActive();
}

void ClippedGenerator::reset() {
   generator->reset();
   old_exists = false;
}

void ClippedGenerator::readOptions(std::string path) {
   generator->readOptions(path);
}

JointValues ClippedGenerator::makeJoints(ActionCommand::All* request,
                                         Odometry* odometry,
                                         const UNSWSensorValues &sensors,
                                         BodyModel &bodyModel,
                                         float ballX,
                                         float ballY) {
   JointValues j = generator->makeJoints(request, odometry, sensors, bodyModel, ballX, ballY);
   for (uint8_t i = 0; i < Joints::NUMBER_OF_JOINTS; ++i) {
      // Clip stifnesses
      if (j.stiffnesses[i] >= 0.0f) {

         j.stiffnesses[i] = CLIP(j.stiffnesses[i], 0.0f, 1.0f);
      } else {

         j.stiffnesses[i] = -1.0f;
      }

      // Clip angles
      if (!isnan(j.angles[i])) {
        // std::cout << "Clipping angle: Original: " << j.angles[i];
         j.angles[i] = Joints::limitJointRadians(Joints::jointCodes[i],
                                                 j.angles[i]);
        // std::cout << " Final: " << j.angles[i] << std::endl;
      }

      // Clip velocities
      if (old_exists) {

         j.angles[i] = CLIP(j.angles[i],
                            old_j.angles[i] - Joints::Radians::MaxSpeed[i],
                            old_j.angles[i] + Joints::Radians::MaxSpeed[i]);
      }
      // std::cout << "CLIPPED: Joint " << Joints::jointNames[i] << " angle: " << j.angles[i] << " stiffness: " << j.stiffnesses[i] << std::endl;
   }
   old_exists = true;
   old_j = j;

   return j;
}
