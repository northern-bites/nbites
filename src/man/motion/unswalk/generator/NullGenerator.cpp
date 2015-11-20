#include "motion/generator/NullGenerator.hpp"
#include "utils/Logger.hpp"

NullGenerator::NullGenerator() {
   llog(INFO) << "NullGenerator created" << std::endl;
}

NullGenerator::~NullGenerator() {
   llog(INFO) << "NullGenerator destroyed" << std::endl;
}

JointValues NullGenerator::makeJoints(ActionCommand::All* request,
                                      Odometry* odometry,
                                      const SensorValues &sensors,
                                      BodyModel &bodyModel,
                                      float ballX,
                                      float ballY) {
   JointValues j = sensors.joints;
   for (uint8_t i = 0; i < Joints::NUMBER_OF_JOINTS; ++i)
      j.stiffnesses[i] = 0.0f;
   return j;
}

bool NullGenerator::isActive() {
   return false;
}

void NullGenerator::reset() {}
