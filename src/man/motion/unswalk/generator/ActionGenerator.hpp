#pragma once

#include <string>
#include <vector>
#include "motion/generator/Generator.hpp"

/* Determine whether the class is just called */
#define NOT_RUNNING -1

/* Hack the stiffness */
#define MAX_STIFF 1.0

class ActionGenerator : Generator {
   public:
      explicit ActionGenerator(std::string filename);
      ~ActionGenerator();
      
      virtual JointValues makeJoints(ActionCommand::All* request,
                                     Odometry* odometry,
                                     const SensorValues &sensors,
                                     BodyModel &bodyModel,
                                     float ballX,
                                     float ballY);
      virtual bool isActive();
      void reset();
      void stop();
      void readOptions(const boost::program_options::variables_map &config);

   private:
      int current_time;
      std::string file_name;
      std::vector<JointValues> joints;
      ActionCommand::Body active;

      /**
       * Determine the duration before the robot
       * actually begins to execute the sequence
       * of poses
       */
      int max_iter;

      /**
       * Interpolate the time that are determined by the
       * duration between the new joints with the previous
       * joints that are read in the file
       * @param newJoint the value of the joint that need to be
       * interpolated with the previous value
       * @param duration if duration = 0, it will do the interpolation
       * between the newJoint value with the joint at MAX_ITER position.
       * Otherwise, it will interpolate between the new joint and the previous
       * joint.
       */
      void interpolate(JointValues newJoint, int duration = 0);

      /**
       * Reading the file from the provided path and construct
       * the pose
       * @param path the directory to read the pose file
       */
      void constructPose(std::string path);
};
