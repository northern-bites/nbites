#include "motion/generator/ClippedGenerator.hpp"
#include "libagent/AgentData.hpp"
#include "utils/body.hpp"
#include "utils/clip.hpp"
#include "utils/Logger.hpp"

using boost::program_options::variables_map;

ClippedGenerator::ClippedGenerator(Generator* g)
   : generator(g),
     old_exists(false) {
   llog(INFO) << "ClippedGenerator constructed" << std::endl;
}

ClippedGenerator::~ClippedGenerator() {
   delete generator;
   llog(INFO) << "ClippedGenerator destroyed" << std::endl;
}

bool ClippedGenerator::isActive() {
   return generator->isActive();
}

void ClippedGenerator::reset() {
   generator->reset();
   old_exists = false;
}

void ClippedGenerator::readOptions(const boost::program_options::variables_map &config) {
   generator->readOptions(config);
}

JointValues ClippedGenerator::makeJoints(ActionCommand::All* request,
                                         Odometry* odometry,
                                         const SensorValues &sensors,
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
         j.angles[i] = Joints::limitJointRadians(Joints::jointCodes[i],
                                                 j.angles[i]);
      }

      // Clip velocities
      if (old_exists) {
         j.angles[i] = CLIP(j.angles[i],
                            old_j.angles[i] - Joints::Radians::MaxSpeed[i],
                            old_j.angles[i] + Joints::Radians::MaxSpeed[i]);
      }
   }
   old_exists = true;
   old_j = j;
   return j;
}
