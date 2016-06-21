#include <fstream>
#include <limits>
#include <cctype>
#include "generator/ActionGenerator.hpp"
#include "utils/Logger.hpp"
#include "utils/angles.hpp"

using namespace std;
using boost::program_options::variables_map;

bool setStand = false;
const float INITIAL_BODY_POSE_ANGLES[] {
   1.57f, 0.17f, 0.0f, 0.0f,
   0.0f, 0.0f,
   0.0f, 0.0017f, -0.488f, 0.87f, -0.436f, 0.0f,
   0.0017f, -0.488f, 0.87f, -0.436f, 0.0f,
   1.57f, -0.17f, 0.0f, 0.0f,
   0.f,0.f,
};

ActionGenerator::ActionGenerator(std::string filename) : file_name(filename) {
   max_iter = 0;
   current_time = NOT_RUNNING;
   joints.clear();

};

ActionGenerator::~ActionGenerator() {
   //std::cout << "ActionGenerator destroyed" << std::endl;
};

bool ActionGenerator::isActive() {
   return current_time != NOT_RUNNING;
};

void ActionGenerator::reset() {
   current_time = 0;
}

void ActionGenerator::NBSetStand() {
   //std::cout << "Action Gen CONSTRUCTING STAND POSE!\n";
   setStand = true;
   JointValues newJoint;
   newJoint.angles[Joints::HeadYaw] = 0.0;
   newJoint.angles[Joints::HeadPitch] = 0.0;

   newJoint.angles[Joints::LShoulderPitch] = INITIAL_BODY_POSE_ANGLES[0];
   newJoint.angles[Joints::LShoulderRoll] = INITIAL_BODY_POSE_ANGLES[1];
   newJoint.angles[Joints::LElbowYaw] = INITIAL_BODY_POSE_ANGLES[2];
   newJoint.angles[Joints::LElbowRoll] = INITIAL_BODY_POSE_ANGLES[3];
   newJoint.angles[Joints::LWristYaw] = INITIAL_BODY_POSE_ANGLES[4];
   newJoint.angles[Joints::LHand] = INITIAL_BODY_POSE_ANGLES[5];

   newJoint.angles[Joints::LHipYawPitch] = INITIAL_BODY_POSE_ANGLES[6];
   newJoint.angles[Joints::LHipRoll] = INITIAL_BODY_POSE_ANGLES[7];
   newJoint.angles[Joints::LHipPitch] = INITIAL_BODY_POSE_ANGLES[8];
   newJoint.angles[Joints::LKneePitch] = INITIAL_BODY_POSE_ANGLES[9];
   newJoint.angles[Joints::LAnklePitch] = INITIAL_BODY_POSE_ANGLES[10];
   newJoint.angles[Joints::LAnkleRoll] =  INITIAL_BODY_POSE_ANGLES[11];

   newJoint.angles[Joints::RHipRoll] = INITIAL_BODY_POSE_ANGLES[12];
   newJoint.angles[Joints::RHipPitch] = INITIAL_BODY_POSE_ANGLES[13];
   newJoint.angles[Joints::RKneePitch] = INITIAL_BODY_POSE_ANGLES[14];
   newJoint.angles[Joints::RAnklePitch] = INITIAL_BODY_POSE_ANGLES[15];
   newJoint.angles[Joints::RAnkleRoll] = INITIAL_BODY_POSE_ANGLES[16];

   newJoint.angles[Joints::RShoulderPitch] = INITIAL_BODY_POSE_ANGLES[17];
   newJoint.angles[Joints::RShoulderRoll] = INITIAL_BODY_POSE_ANGLES[18];
   newJoint.angles[Joints::RElbowYaw] = INITIAL_BODY_POSE_ANGLES[19];
   newJoint.angles[Joints::RElbowRoll] = INITIAL_BODY_POSE_ANGLES[20];
   newJoint.angles[Joints::RWristYaw] = INITIAL_BODY_POSE_ANGLES[21];
   newJoint.angles[Joints::RHand] = INITIAL_BODY_POSE_ANGLES[22];

   for (int i = 0; i < Joints::NUMBER_OF_JOINTS; i++) {
      newJoint.stiffnesses[i] = 1.0;
      // newJoint.angles[i] = UNSWDEG2RAD(newJoint.angles[i]);
   }

   newJoint.stiffnesses[Joints::LShoulderPitch] = 0.0;
   newJoint.stiffnesses[Joints::LShoulderRoll] = 0.0;
   newJoint.stiffnesses[Joints::LElbowYaw] =  0.0;
   newJoint.stiffnesses[Joints::LElbowRoll] =  0.0;
   newJoint.stiffnesses[Joints::LWristYaw] = 0.0;
   newJoint.stiffnesses[Joints::LHand] = 0.0;

   newJoint.stiffnesses[Joints::RShoulderPitch] = 0.0;
   newJoint.stiffnesses[Joints::RShoulderRoll] = 0.0;
   newJoint.stiffnesses[Joints::RElbowYaw] = 0.0;
   newJoint.stiffnesses[Joints::RElbowRoll] = 0.0;
   newJoint.stiffnesses[Joints::RWristYaw] = 0.0;
   newJoint.stiffnesses[Joints::RHand] = 0.0;



   int duration = 3;
   interpolate(newJoint);

}

void ActionGenerator::stop(){
}

JointValues ActionGenerator::makeJoints(ActionCommand::All* request,
                                        Odometry* odometry,
                                        const UNSWSensorValues &sensors,
                                        BodyModel &bodyModel,
                                        float ballX,
                                        float ballY) {

   if (request->body.actionType = ActionCommand::Body::STAND) {
      //std::cout << "Actiongen Stand requested! " << std::endl;
      NBSetStand();
   }

   //std::cout << "Joint size: " << joints.size() << std::endl;
   JointValues j;
   if (current_time == NOT_RUNNING) {
      // current_time = 0;
      active = request->body;
      j = joints[joints.size() - 1];
   } else {
      JointValues newJoints = sensors.joints;
      for (int i = 0; i < Joints::NUMBER_OF_JOINTS; i++) {
         newJoints.stiffnesses[i] = 1.0f;
      }
      if (current_time == 0) interpolate(newJoints);
      j = joints[current_time++];
      if (current_time == (signed int)joints.size())  // if we just did last action
         current_time = NOT_RUNNING;
   }
   // JointValues j = joints[current_time++];
   return j;
};

void ActionGenerator::interpolate(JointValues newJoint, int duration) {
   if (joints.empty()) {
      max_iter = duration / 10;
      // Reserve space for the interpolation when the generator
      // first called
      for (int i = 0; i < max_iter; i++) {
         joints.push_back(newJoint);
      }
      joints.push_back(newJoint);
   } else {
      int inTime = 0;
      float offset[Joints::NUMBER_OF_JOINTS];

      if (duration != 0) {

         inTime = duration / 10;
         JointValues currentJoint = joints.back();

         // Calculate the difference between new joint and the previous joint
         for (int i = 0; i < Joints::NUMBER_OF_JOINTS; i++) {
            offset[i] = (newJoint.angles[i] - currentJoint.angles[i]) / inTime;
         }


         for (int i = 0; i < inTime; i++) {
            JointValues inJoint;
            for (int j = 0; j < Joints::NUMBER_OF_JOINTS; j++) {
               inJoint.angles[j] = joints.back().angles[j] + offset[j];
               inJoint.stiffnesses[j] = newJoint.stiffnesses[j];
            }
            joints.push_back(inJoint);
         }
      } else {

         JointValues firstJoint = joints.at(max_iter);
         // Calculate the difference between the joint at MAX_ITER position
         // with the new joint
         for (int i = 0; i < Joints::NUMBER_OF_JOINTS; i++) {
            offset[i] = (firstJoint.angles[i] - newJoint.angles[i]) / max_iter;
         }

         joints[0] = newJoint;
         for (int i = 1; i < max_iter; i++) {
            for (int j = 0; j < Joints::NUMBER_OF_JOINTS; j++) {
               joints[i].angles[j] = joints[i - 1].angles[j] + offset[j];
               joints[i].stiffnesses[j] = firstJoint.stiffnesses[j];
            }
         }
      }
   }
}

void ActionGenerator::constructPose(std::string path) {
   ifstream in(string(path + "/" + file_name + ".pos").c_str());
   std::cout << "ActionGenerator(" << file_name << ") creating" << endl;

   if (!in.is_open()) {
      //std::cout << "ActionGenerator can not open " << file_name << endl;
   } else {
      int duration = 0;
      float stiffness = 1.0;
      float angles = 0.0;
      while (isspace(in.peek())) {
         in.ignore();
      }
      while (!in.eof()) {
         // Ignore comments, newlines and ensure not eof
         if (in.peek() == '#' || in.peek() == '\n' || in.peek() == EOF) {
            in.ignore(std::numeric_limits<int>::max(), '\n');
            continue;
         }
         JointValues newJoint;
         // Read the angles in the file to create a new JointValue
         for (int i = 0; i < Joints::NUMBER_OF_JOINTS; i++) {
            while (isspace(in.peek())) {
               in.ignore();
            }
            if (in.peek() == '#' || in.peek() == '$' || in.peek() == '\n' || in.peek() == EOF) {
               //std::cout << "You're missing a joint value in " << file_name << ".pos" << std::endl;
               exit(1);
            }
            in >> angles;
            // Convert degree to radian because the values in the file
            // are in degree
            newJoint.angles[i] = UNSWDEG2RAD(angles);
            newJoint.stiffnesses[i] = 1.0;
         }

         // read in the duration
         while (isspace(in.peek())) {
            in.ignore();
         }
         if (in.peek() == '#' || in.peek() == '$' || in.peek() == '\n' || in.peek() == EOF) {
            //std::cout << "You're missing a duration in " << file_name << ".pos" << std::endl;
            exit(1);
         }
         in >> duration;

         // Ignore comments, newlines and ensure not eof
         while (isspace(in.peek())) {
            in.ignore();
         }
         while (in.peek() == '#') {
            in.ignore(std::numeric_limits<int>::max(), '\n');
         }
         while (isspace(in.peek())) {
            in.ignore();
         }

         // Stiffnesses are specified by a line beginning with "$"
         if (in.peek() == '$') {
            in.ignore(std::numeric_limits<int>::max(), '$');
            for (int i = 0; i < Joints::NUMBER_OF_JOINTS; i++) {
               while (isspace(in.peek())) {
                  in.ignore();
               }
               if (in.peek() == '#' || in.peek() == '\n' || in.peek() == EOF) {
                  //std::cout << "You're missing a stiffness value in " << file_name << ".pos" << std::endl;
                  exit(1);
               }
               in >> stiffness;
               newJoint.stiffnesses[i] = stiffness;
            }
            // Ignore comments, newlines and ensure not eof
            while (isspace(in.peek())) {
               in.ignore();
            }
            while (in.peek() == '#') {
               in.ignore(std::numeric_limits<int>::max(), '\n');
            }
            while (isspace(in.peek())) {
               in.ignore();
            }
         }
         interpolate(newJoint, duration);
         while (isspace(in.peek())) {
            in.ignore();
         }
      }
      in.close();
   }
   //std::cout << "ActionGenerator(" << file_name << ") created" << endl;
}

void ActionGenerator::readOptions(std::string path) {
   // std::string path = config["motion.path"].as<std::string>();
   constructPose(path);
}
