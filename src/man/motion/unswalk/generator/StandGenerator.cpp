#include "motion/generator/StandGenerator.hpp"
#include "motion/generator/ActionGenerator.hpp"
#include "utils/angles.hpp"
#include "utils/body.hpp"
#include "utils/Logger.hpp"

using boost::program_options::variables_map;

StandGenerator::StandGenerator()
   : phi(DEG2RAD(0.0f)) {    //was 15, using 20 knee bend to prevent jerky transition between walk and stand
   llog(INFO) << "StandGenerator constructed" << std::endl;
   posGen = (Generator*)(new ActionGenerator("stand"));
   if (!posGen)
      llog(FATAL) << "stand generator is NULL!" << std::endl;
}

StandGenerator::~StandGenerator() {
   llog(INFO) << "StandGenerator destroyed" << std::endl;
   delete posGen;
}

JointValues StandGenerator::makeJoints(ActionCommand::All* request,
                                       Odometry* odometry,
                                       const SensorValues &sensors,
                                       BodyModel &bodyModel,
                                       float ballX,
                                       float ballY) {
//   JointValues joints = sensors.joints;
//   uint8_t i = Joints::HeadYaw;
//   joints.stiffnesses[i++] = 0.0f;  // HeadYaw
//   joints.stiffnesses[i++] = 0.0f;  // HeadPitch
//   joints.stiffnesses[i++] = 0.0f;  // LShoulderPitch
//   joints.stiffnesses[i++] = 0.0f;  // LShoulderRoll
//   joints.stiffnesses[i++] = 0.0f;  // LElbowYaw
//   joints.stiffnesses[i++] = 0.0f;  // LElbowRoll
//   joints.angles[i] = 0.0f,  joints.stiffnesses[i++] = 0.66f;  // LHipYawPitch
//   joints.angles[i] = 0.0f,  joints.stiffnesses[i++] = 0.66f;  // LHipRoll
//   interpolate(joints.angles[i], -phi),  joints.stiffnesses[i++] = 0.66f;  // LHipPitch
//   interpolate(joints.angles[i], 2 * phi), joints.stiffnesses[i++] = 0.66f;  // LKneePitch
//   interpolate(joints.angles[i], -phi),  joints.stiffnesses[i++] = 0.66f;  // LAnklePitch
//   joints.angles[i] = 0.0f,  joints.stiffnesses[i++] = 0.66f;  // LAnkleRoll
//   joints.angles[i] = 0.0f,  joints.stiffnesses[i++] = 0.66f;  // RHipRoll
//   interpolate(joints.angles[i], -phi),  joints.stiffnesses[i++] = 0.66f;  // RHipPitch
//   interpolate(joints.angles[i], 2 * phi), joints.stiffnesses[i++] = 0.66f;  // RKneePitch
//   interpolate(joints.angles[i], -phi),  joints.stiffnesses[i++] = 0.66f;  // RAnklePitch
//   joints.angles[i] = 0.0f,  joints.stiffnesses[i++] = 0.66f;  // RAnkleRoll
//   joints.stiffnesses[i++] = 0.0f;  // RShoulderPitch
//   joints.stiffnesses[i++] = 0.0f;  // RShoulderRoll
//   joints.stiffnesses[i++] = 0.0f;  // RElbowYaw
//   joints.stiffnesses[i++] = 0.0f;  // RElbowRoll
   return posGen->makeJoints(request, odometry, sensors, bodyModel, ballX, ballY);
}

bool StandGenerator::isActive() {
   return posGen->isActive();
}

void StandGenerator::reset() {
   posGen->reset();
}

void StandGenerator::stop() {
   posGen->stop();
}

void StandGenerator::readOptions(const boost::program_options::variables_map &config) {
   posGen->readOptions(config);
//   phi = DEG2RAD(config["walk.b"].as<float>());
//   llog(INFO) << "Successfully changed stand generator options" << std::endl;
//   llog(INFO) << "Stand generator options ignored" << std::endl;
}
