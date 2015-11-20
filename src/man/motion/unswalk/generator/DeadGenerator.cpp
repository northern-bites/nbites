#include "motion/generator/DeadGenerator.hpp"
#include "utils/Logger.hpp"

DeadGenerator::DeadGenerator() : ticks(0) {
   llog(INFO) << "DeadGenerator created" << std::endl;
}

DeadGenerator::~DeadGenerator() {
   llog(INFO) << "DeadGenerator destroyed" << std::endl;
}

JointValues DeadGenerator::makeJoints(ActionCommand::All* request,
                                      Odometry* odometry,
                                      const SensorValues &sensors,
                                      BodyModel &bodyModel,
                                      float ballX,
                                      float ballY) {
   ++ticks;
   JointValues j = sensors.joints;
   for (uint8_t i = 0; i < Joints::NUMBER_OF_JOINTS; ++i)
      j.stiffnesses[i] = -1.0f;
   return j;
}

bool DeadGenerator::isActive() {
   return ticks <= MIN_TICKS;
}

void DeadGenerator::reset() {
   ticks = 0;
}
