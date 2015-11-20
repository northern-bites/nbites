#include "motion/generator/HeadGenerator.hpp"
#include "utils/basic_maths.hpp"
#include "utils/clip.hpp"
#include "utils/Logger.hpp"

HeadGenerator::HeadGenerator()
   : yaw(0.0f),
     pitch(0.0f) {
   llog(INFO) << "HeadGenerator constructed" << std::endl;
}

HeadGenerator::~HeadGenerator() {
   llog(INFO) << "HeadGenerator destroyed" << std::endl;
}

/* Borrowed from Aaron's 2009 code. Uses a PID controller. */
JointValues HeadGenerator::makeJoints(ActionCommand::All* request,
                                      Odometry* odometry,
                                      const SensorValues &sensors,
                                      BodyModel &bodyModel,
                                      float ballX,
                                      float ballY) {
   // Simple state of the head angles
   float desY = 0.0, desP = 0.0;    // Current requested angles

   // Boundary checking (NAN values)
   if (isnan(request->head.yaw) || isnan(request->head.pitch)) {
      llog(INFO) << "HeadGenerator: NAN angles - Doing Nothing" << std::endl;
      return sensors.joints;
   }
   // TODO(stuartr): init these variables above.
   /* This seemed to not work as well as just remember where we should be */
   // pitch = sensors.joints.angles[Joints::HeadPitch];
   // yaw = sensors.joints.angles[Joints::HeadYaw];

   if (request->head.isRelative) {
      desY = yaw + request->head.yaw;
      //desP = pitch + request->head.pitch;
   } else {
      desY = request->head.yaw;
   }
   desP = request->head.pitch;

   float diffY = desY - yaw;
   float diffP = desP - pitch;
   diffY = CLIP<float>(diffY, Joints::Radians::HeadYawSpeed *
                       request->head.yawSpeed);
   diffP = CLIP<float>(diffP, Joints::Radians::HeadPitchSpeed *
                       request->head.pitchSpeed);
   yaw += diffY;
   pitch += diffP;

   // Return JointValues with head angles changed, all others same
   JointValues j = sensors.joints;
   j.angles[Joints::HeadYaw] = yaw;
   j.angles[Joints::HeadPitch] = pitch;
   j.stiffnesses[Joints::HeadYaw] = 0.8f;
   j.stiffnesses[Joints::HeadPitch] = 0.8f;
   return j;
}

bool HeadGenerator::isActive() {
   return true;
}

void HeadGenerator::reset() {
   yaw = 0.0f;
   pitch = 0.0f;
}
