#include <fstream>
#include <limits>
#include <cctype>

#include "generator/DistributedGenerator.hpp"
#include "generator/ActionGenerator.hpp"
#include "generator/DeadGenerator.hpp"
#include "generator/HeadGenerator.hpp"
#include "generator/NullGenerator.hpp"
#include "generator/RefPickupGenerator.hpp"
#include "generator/StandGenerator.hpp"
#include "generator/WalkEnginePreProcessor.hpp"

#include "utils/body.hpp"
#include "utils/Logger.hpp"

using namespace std;
using ActionCommand::Body;
using boost::program_options::variables_map;

/*-----------------------------------------------------------------------------
 * Distributed Generator
 * ---------------------
 * This generator switches between all required generators as requested.
 *---------------------------------------------------------------------------*/
DistributedGenerator::DistributedGenerator()
   : isStopping(false),
     current_generator(Body::NONE),
     prev_generator(Body::NONE),
     requestedDive(Body::NONE) {
      std::cout << "Constructing distributed generator!!\n";
   // headGenerator = (Generator*)(new HeadGenerator());
   // if (!headGenerator)
   //    std::cout << "headGenerator is NULL!" << std::endl;

   // TODO(dpad): Rewrite these ugly //llogs to simply loop through bodyGenerators
   // and print out the string name
   bodyGenerators[Body::NONE] = (Generator*)(new NullGenerator());
   if (!bodyGenerators[Body::NONE])
      std::cout << "bodyGenerators[NONE] is NULL!" << std::endl;

   // bodyGenerators[Body::STAND] = (Generator*)(new ActionGenerator("initial"));
   // if (!bodyGenerators[Body::STAND])
   //    std::cout << "bodyGenerators[STAND] is NULL!" << std::endl;

   bodyGenerators[Body::STAND] = (Generator*)(new StandGenerator());
   std::cout << "Called stand generator from distributed generator!" << std::endl;
   if (!bodyGenerators[Body::STAND])
      std::cout << "bodyGenerators[STAND] is NULL!" << std::endl;

   // bodyGenerators[Body::MOTION_CALIBRATE] =
   //         (Generator*)(new ActionGenerator("standStraight"));
   // if (!bodyGenerators[Body::MOTION_CALIBRATE])
   //    std::cout << "bodyGenerators[MOTION_CALIBRATE] is NULL!" << std::endl;

   // bodyGenerators[Body::STAND_STRAIGHT] =
   //         (Generator*)(new ActionGenerator("standStraight"));
   // if (!bodyGenerators[Body::STAND_STRAIGHT])
   //    std::cout << "bodyGenerators[STAND_STRAIGHT] is NULL!" << std::endl;

   bodyGenerators[Body::WALK] = (Generator*)(new WalkEnginePreProcessor());
   if (!bodyGenerators[Body::WALK])
      std::cout << "bodyGenerators[WALK] is NULL!" << std::endl;

   bodyGenerators[Body::KICK] = bodyGenerators[Body::WALK];

   // bodyGenerators[Body::LINE_UP] = bodyGenerators[Body::WALK];

   // bodyGenerators[Body::DRIBBLE] = bodyGenerators[Body::WALK];

   // bodyGenerators[Body::GETUP_FRONT] = (Generator*)
   //                                     (new ActionGenerator("getupFront"));
   // if (!bodyGenerators[Body::GETUP_FRONT])
   //    std::cout << "bodyGenerators[GETUP_FRONT] is NULL!" << std::endl;

   // bodyGenerators[Body::GETUP_BACK] = (Generator*)
   //                                    (new ActionGenerator("getupBack"));
   // if (!bodyGenerators[Body::GETUP_BACK])
   //    std::cout << "bodyGenerators[GETUP_BACK] is NULL!" << std::endl;

   // bodyGenerators[Body::INITIAL] = (Generator*)
   //                                 (new ActionGenerator("initial"));
   // if (!bodyGenerators[Body::INITIAL])
   //    std::cout << "bodyGenerators[INITIAL] is NULL!" << std::endl;

   // bodyGenerators[Body::DEAD] = (Generator*)(new DeadGenerator());
   // if (!bodyGenerators[Body::DEAD])
   //    std::cout << "bodyGenerators[DEAD] is NULL!" << std::endl;

   // bodyGenerators[Body::REF_PICKUP] = (Generator*)(new RefPickupGenerator());
   // if (!bodyGenerators[Body::REF_PICKUP])
   //    std::cout << "bodyGenerators[REF_PICKUP] is NULL!" << std::endl;

   // bodyGenerators[Body::OPEN_FEET] = (Generator*)
   //                                   (new ActionGenerator("openFeet"));
   // if (!bodyGenerators[Body::OPEN_FEET])
   //    std::cout << "bodyGenerators[OPEN_FEET] is NULL!" << std::endl;

   // bodyGenerators[Body::THROW_IN] = (Generator*)
   //                                  (new ActionGenerator("throwIn"));
   // if (!bodyGenerators[Body::THROW_IN])
   //    std::cout << "bodyGenerators[THROW_IN] is NULL!" << std::endl;

   // bodyGenerators[Body::GOALIE_SIT] = (Generator*)
   //                                    (new ActionGenerator("goalieSit"));
   // if (!bodyGenerators[Body::GOALIE_SIT])
   //    std::cout << "bodyGenerators[GOALIE_SIT] is NULL!" << std::endl;

   // bodyGenerators[Body::GOALIE_DIVE_LEFT] = (Generator*)
   //                                          (new ActionGenerator("goalieDiveLeft"));
   // if (!bodyGenerators[Body::GOALIE_DIVE_LEFT])
   //    std::cout << "bodyGenerators[GOALIE_DIVE_LEFT] is NULL!" << std::endl;

   // bodyGenerators[Body::GOALIE_DIVE_RIGHT] = (Generator*)
   //                                           (new ActionGenerator("goalieDiveRight"));
   // if (!bodyGenerators[Body::GOALIE_DIVE_RIGHT])
   //    std::cout << "bodyGenerators[GOALIE_DIVE_RIGHT] is NULL!" << std::endl;

   // bodyGenerators[Body::GOALIE_CENTRE] = (Generator*)
   //                                           (new ActionGenerator("goalieCentre"));
   // if (!bodyGenerators[Body::GOALIE_CENTRE])
   //    std::cout << "bodyGenerators[GOALIE_CENTRE] is NULL!" << std::endl;

   // bodyGenerators[Body::GOALIE_UNCENTRE] = (Generator*)
   //                                           (new ActionGenerator("goalieUncentre"));
   // if (!bodyGenerators[Body::GOALIE_UNCENTRE])
   //    std::cout << "bodyGenerators[GOALIE_UNCENTRE] is NULL!" << std::endl;

   // bodyGenerators[Body::GOALIE_STAND] = (Generator*)
   //                                           (new ActionGenerator("goalieStand"));
   // if (!bodyGenerators[Body::GOALIE_STAND])
   //    std::cout << "bodyGenerators[GOALIE_STAND] is NULL!" << std::endl;

   // bodyGenerators[Body::GOALIE_INITIAL] = (Generator*)
   //                                           (new ActionGenerator("goalieInitial"));
   // if (!bodyGenerators[Body::GOALIE_INITIAL])
   //    std::cout << "bodyGenerators[GOALIE_INITIAL] is NULL!" << std::endl;

   // bodyGenerators[Body::GOALIE_AFTERSIT_INITIAL] = (Generator*)
   //                                           (new ActionGenerator("goalieInitial"));
   // if (!bodyGenerators[Body::GOALIE_AFTERSIT_INITIAL])
   //    std::cout << "bodyGenerators[GOALIE_AFTERSIT_INITIAL] is NULL!" << std::endl;

   // bodyGenerators[Body::DEFENDER_CENTRE] = (Generator*)
   //                                        (new ActionGenerator("defenderCentre"));
   // if (!bodyGenerators[Body::DEFENDER_CENTRE])
   //   std::cout << "bodyGenerators[DEFENDER_CENTRE] is NULL!" << std::endl;

   // bodyGenerators[Body::GOALIE_PICK_UP] = (Generator*)
   //                                        (new ActionGenerator("goaliePickup"));
   // if (!bodyGenerators[Body::GOALIE_PICK_UP])
   //   std::cout << "bodyGenerators[GOALIE_PICK_UP] is NULL!" << std::endl;

   // bodyGenerators[Body::GOALIE_FAST_SIT] = (Generator*)
   //                                        (new ActionGenerator("goalieFastSit"));
   // if (!bodyGenerators[Body::GOALIE_FAST_SIT])
   //   std::cout << "bodyGenerators[GOALIE_FAST_SIT] is NULL!" << std::endl;

   //llog(INFO) << "DistributedGenerator constructed" << std::endl;
   std::cout << "Calling readOptions in the distributed generator!" << std::endl;
   // this->readOptions("/home/nao/nbites/Config");
}

/*-----------------------------------------------------------------------------
 * Destructor
 *---------------------------------------------------------------------------*/
DistributedGenerator::~DistributedGenerator() {
   // delete headGenerator;
   for (uint8_t i = 0; i < NUM_NBITE_GENS; ++i)
      if (bodyGenerators[i]) {
         delete bodyGenerators[i];
         for (uint8_t j = i + 1; j < NUM_NBITE_GENS; ++j)
            if (bodyGenerators[j] == bodyGenerators[i])
               bodyGenerators[j] = NULL;
      }
   //llog(INFO) << "DistributedGenerator destroyed" << std::endl;
}

/*-----------------------------------------------------------------------------
 * makeJoints
 * Returns the joint values requested by whichever generator we're using
 *---------------------------------------------------------------------------*/
JointValues DistributedGenerator::makeJoints(ActionCommand::All* request,
                                             Odometry* odometry,
                                             const UNSWSensorValues &sensors,
                                             BodyModel &bodyModel,
                                             float ballX,
                                             float ballY) {

   // If we're requesting a dive, set requestedDive variable
   // if(requestedDive == Body::NONE
   //       && !(
   //          current_generator == Body::GOALIE_CENTRE ||
   //          current_generator == Body::GOALIE_DIVE_LEFT ||
   //          current_generator == Body::GOALIE_DIVE_RIGHT ||
   //          current_generator == Body::DEFENDER_CENTRE
   //          )
   //       && (
   //          request->body.actionType == Body::GOALIE_CENTRE ||
   //          request->body.actionType == Body::GOALIE_DIVE_LEFT ||
   //          request->body.actionType == Body::GOALIE_DIVE_RIGHT ||
   //          current_generator == Body::DEFENDER_CENTRE
   //          )) {
   //    requestedDive = request->body.actionType;
   // }

   // std::cout << "DISTRIBUTEDGEN making joints\n";
   // ((ActionGenerator*)bodyGenerators[Body::STAND])->NBSetStand();

   JointValues fromBody;
   bool usesHead = false;

   // Check the priority of the requested action compared to the current action
   if (ActionCommand::priorities[request->body.actionType] >
       ActionCommand::priorities[current_generator]) {
      reset();
      isStopping = false;
   }

   if (!bodyGenerators[current_generator]->isActive()) {
      if (bodyGenerators[current_generator] != bodyGenerators[request->body.actionType]
            || isStopping
            || (current_generator == Body::GETUP_FRONT && request->body.actionType == Body::GETUP_FRONT)
            || (current_generator == Body::GETUP_BACK && request->body.actionType == Body::GETUP_BACK)) {
         bodyGenerators[current_generator]->reset();
      }
      // special case for goalie sit
//       if (current_generator == Body::GOALIE_SIT || current_generator == Body::GOALIE_FAST_SIT){  //or fast sit
// //            && request->body.actionType != Body::GOALIE_SIT
//          if(requestedDive != Body::NONE){
// //         if (request->body.actionType == Body::GOALIE_DIVE_LEFT ||
// //             request->body.actionType == Body::GOALIE_CENTRE ||
// //             request->body.actionType == Body::GOALIE_DIVE_RIGHT) {
//             current_generator = requestedDive;
//          } else if(request->body.actionType != Body::GOALIE_SIT){
//             current_generator = Body::GOALIE_AFTERSIT_INITIAL;
//          }
//       // goalie after pick up should transition back through goalie uncentre
//       } else if (current_generator == Body::GOALIE_PICK_UP && request->body.actionType != Body::GOALIE_PICK_UP) {
// 	current_generator = Body::GOALIE_UNCENTRE;   //Body::GOALIE_PICK_UP;
//       } else if (current_generator == Body::GOALIE_CENTRE && request->body.actionType != Body::GOALIE_CENTRE) {
//          current_generator = Body::GOALIE_UNCENTRE;
//       // and goalie uncentre should transition back through goalie sit
//       } else if (current_generator == Body::GOALIE_UNCENTRE && request->body.actionType != Body::GOALIE_UNCENTRE && request->body.actionType != Body::GOALIE_CENTRE) {
//          current_generator = Body::GOALIE_SIT;
//       // defender centre should transition back through goalie uncentre
//       } else if (current_generator == Body::DEFENDER_CENTRE && request->body.actionType != Body::DEFENDER_CENTRE) {
//         current_generator = Body::GOALIE_UNCENTRE;
//       // anything else should transition through goalie sit to goalie centre 
//       // commit to it
//       } else if ((current_generator != Body::GOALIE_CENTRE &&
//                  request->body.actionType == Body::GOALIE_CENTRE)
// //                 || (current_generator != Body::GOALIE_DIVE_LEFT &&
// //                       request->body.actionType == Body::GOALIE_DIVE_LEFT)
// //                 || (current_generator != Body::GOALIE_DIVE_RIGHT &&
// //                       request->body.actionType == Body::GOALIE_DIVE_RIGHT)
//                  ) {
// //         current_generator = Body::GOALIE_SIT;
//          current_generator = Body::GOALIE_FAST_SIT;
//       } else if (current_generator == Body::GOALIE_STAND && request->body.actionType != Body::GOALIE_STAND) {
//          current_generator = Body::GOALIE_INITIAL;
//       } else {
      current_generator = request->body.actionType;
      // }
      isStopping = false;
   } 
   // else if (bodyGenerators[current_generator]->isActive() &&
   //            bodyGenerators[current_generator] !=
   //            bodyGenerators[request->body.actionType]) {
   //    // Special case to let kicks continue instead of being interrupted by stand
   //   // std::cout << current_generator << " " << request->body.actionType << std::endl;
   //    if (current_generator != Body::KICK || request->body.actionType != Body::STAND) {
   //       bodyGenerators[current_generator]->stop();
   //       isStopping = true;
   //    }
   // }
  // std::cout << "active " << bodyGenerators[current_generator]->isActive() << " " << current_generator << std::endl;

   // if(current_generator == requestedDive){
   //    requestedDive = Body::NONE;
   // }

   current_generator = request->body.actionType;
   // std::cout << "setting current generator!\n";

   if (request->body.actionType == ActionCommand::Body::STAND) {
      std::cout << "Current generator is STAND COMMAND!" << std::endl;
      this->readOptions("/home/nao/nbites/Config");
   }

   switch (current_generator) {
   case Body::NONE:             usesHead = false; break;
   case Body::STAND:            usesHead = false; break;
   case Body::WALK:             usesHead = false; break;
   // case Body::GETUP_FRONT:      usesHead = true;  break;
   // case Body::GETUP_BACK:       usesHead = true;  break;
   // case Body::INITIAL:          usesHead = true;  break;
   case Body::KICK:             usesHead = false; break;
   // case Body::DRIBBLE:          usesHead = false; break;
   // case Body::DEAD:             usesHead = true;  break;
   // case Body::REF_PICKUP:       usesHead = false; break;
   // case Body::OPEN_FEET:        usesHead = true;  break;
   // case Body::THROW_IN:         usesHead = false; break;
   // case Body::GOALIE_SIT:       usesHead = false; break;
   // case Body::GOALIE_DIVE_LEFT: usesHead = true; break;
   // case Body::GOALIE_DIVE_RIGHT: usesHead = true; break;
   // case Body::GOALIE_CENTRE:    usesHead = false; break;
   // case Body::GOALIE_UNCENTRE:  usesHead = false; break;
   // case Body::GOALIE_STAND:     usesHead = false; break;
   // case Body::GOALIE_INITIAL:   usesHead = false; break;
   // case Body::GOALIE_AFTERSIT_INITIAL: usesHead = false; break;
   // case Body::GOALIE_PICK_UP:   usesHead = true;  break;
   // case Body::DEFENDER_CENTRE:  usesHead = false; break;
   // case Body::GOALIE_FAST_SIT:  usesHead = false; break;
   // case Body::MOTION_CALIBRATE: usesHead = false; break;
   // case Body::STAND_STRAIGHT:   usesHead = false; break;
   // case Body::LINE_UP:          usesHead = false; break;
   // case NUM_NBITE_GENS: usesHead = false; break;
   }
   //check for dives and update odometry
   // float turn = 0;
   // int dir = 0;
   // if(current_generator == Body::GETUP_FRONT){
   //    dir = 1;
   // } else if(current_generator == Body::GETUP_BACK){
   //    dir = -1;
   // }
   // if(prev_generator == Body::GOALIE_DIVE_LEFT){
   //    turn = UNSWDEG2RAD(dir*80);
   // } else if (prev_generator == Body::GOALIE_DIVE_RIGHT){
   //    turn = UNSWDEG2RAD(-dir*80);
   // }
   // *odometry = *odometry + Odometry(0, 0, turn);
   // std::cout << "IN DISTRIBUTED GENERATOR\n";
   fromBody = bodyGenerators[current_generator]->
              makeJoints(request, odometry, sensors, bodyModel, ballX, ballY);

   if(current_generator == Body::KICK && request->body.actionType == Body::WALK) {
      current_generator = Body::WALK;
   }

   // if (!usesHead) {
   //    JointValues fromHead = headGenerator->
   //                           makeJoints(request, odometry, sensors, bodyModel, ballX, ballY);
   //    for (uint8_t i = Joints::HeadYaw; i <= Joints::HeadPitch; ++i) {
   //       fromBody.angles[i] = fromHead.angles[i];
   //       fromBody.stiffnesses[i] = fromHead.stiffnesses[i];
   //    }
   // }
   prev_generator = current_generator;
   return fromBody;
}

bool DistributedGenerator::isActive() {
   return true;
}

bool DistributedGenerator::isStanding() {
   return ((WalkEnginePreProcessor*)bodyGenerators[Body::WALK])->isStanding();
}

void DistributedGenerator::reset() {
   for (uint8_t i = 0; i < NUM_NBITE_GENS; ++i) {
      bodyGenerators[i]->reset();
   }
   // headGenerator->reset();
   current_generator = ActionCommand::Body::NONE;
}

// INTERPOLATE METHOD ORIGINALLY DEFINED IN THE ACTION GENERATOR
void DistributedGenerator::interpolate(JointValues newJoint, int duration) {
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

// TRY TO MAKE THE ROBOT STAND WITHOUT CALLING THE STAND GENERATOR
void DistributedGenerator::constructPose(std::string path) {
   // ifstream in(string(path + ".pos").c_str());
   ifstream in(string(path + "/stand.pos").c_str());
   std::cout << "Path is " << path << std::endl;
   std::cout << "DistributedGenerator(stand) creating" << endl;

   if (!in.is_open()) {
      std::cout << "DistributedGenerator can not open " << file_name << endl;
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
               std::cout << "You're missing a joint value in " << file_name << ".pos" << std::endl;
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
            std::cout << "You're missing a duration in " << file_name << ".pos" << std::endl;
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
      std::cout << "DistributedGenerator(stand) created" << endl;
   }
}


void DistributedGenerator::readOptions(std::string path) {

   if (current_generator == ActionCommand::Body::STAND) {
      std::cout << "readOptions received a STAND COMMAND!" << std::endl;
      this->constructPose(path);
      return;
   }

   for (uint8_t i = 0; i < NUM_NBITE_GENS; ++i) {
      bodyGenerators[i]->readOptions(path);
   }
   // headGenerator->readOptions(config);
}
