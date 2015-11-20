#include "Oracle.h"
#include "utils/Logger.hpp"
#include "motion/effector/SimEffector.hpp"

using namespace std;

SimEffector::SimEffector() {
   llog(INFO) << "Sim Effector initialised" << endl;
}

SimEffector::~SimEffector() {
   llog(INFO) << "Sim Effector destroyed" << endl;
}

void SimEffector::actuate(JointValues joints, ActionCommand::LED leds,
                          float sonar) {
   oracle->setJointAngles(joints);
}
