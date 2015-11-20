#include "motion/generator/DistributedGenerator.hpp"
#include "motion/generator/ActionGenerator.hpp"
#include "motion/generator/DeadGenerator.hpp"
#include "motion/generator/HeadGenerator.hpp"
#include "motion/generator/NullGenerator.hpp"
#include "motion/generator/RefPickupGenerator.hpp"
#include "motion/generator/StandGenerator.hpp"
#include "motion/generator/WalkEnginePreProcessor.hpp"

#include "utils/body.hpp"
#include "utils/Logger.hpp"

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

   headGenerator = (Generator*)(new HeadGenerator());
   if (!headGenerator)
      llog(FATAL) << "headGenerator is NULL!" << std::endl;

   // TODO(dpad): Rewrite these ugly llogs to simply loop through bodyGenerators
   // and print out the string name
   bodyGenerators[Body::NONE] = (Generator*)(new NullGenerator());
   if (!bodyGenerators[Body::NONE])
      llog(FATAL) << "bodyGenerators[NONE] is NULL!" << std::endl;

   bodyGenerators[Body::STAND] = (Generator*)(new StandGenerator());
   if (!bodyGenerators[Body::STAND])
      llog(FATAL) << "bodyGenerators[STAND] is NULL!" << std::endl;

   bodyGenerators[Body::MOTION_CALIBRATE] =
           (Generator*)(new ActionGenerator("standStraight"));
   if (!bodyGenerators[Body::MOTION_CALIBRATE])
      llog(FATAL) << "bodyGenerators[MOTION_CALIBRATE] is NULL!" << std::endl;

   bodyGenerators[Body::STAND_STRAIGHT] =
           (Generator*)(new ActionGenerator("standStraight"));
   if (!bodyGenerators[Body::STAND_STRAIGHT])
      llog(FATAL) << "bodyGenerators[STAND_STRAIGHT] is NULL!" << std::endl;

   bodyGenerators[Body::WALK] = (Generator*)(new WalkEnginePreProcessor());
   if (!bodyGenerators[Body::WALK])
      llog(FATAL) << "bodyGenerators[WALK] is NULL!" << std::endl;

   bodyGenerators[Body::KICK] = bodyGenerators[Body::WALK];

   bodyGenerators[Body::LINE_UP] = bodyGenerators[Body::WALK];

   bodyGenerators[Body::DRIBBLE] = bodyGenerators[Body::WALK];

   bodyGenerators[Body::GETUP_FRONT] = (Generator*)
                                       (new ActionGenerator("getupFront"));
   if (!bodyGenerators[Body::GETUP_FRONT])
      llog(FATAL) << "bodyGenerators[GETUP_FRONT] is NULL!" << std::endl;

   bodyGenerators[Body::GETUP_BACK] = (Generator*)
                                      (new ActionGenerator("getupBack"));
   if (!bodyGenerators[Body::GETUP_BACK])
      llog(FATAL) << "bodyGenerators[GETUP_BACK] is NULL!" << std::endl;

   bodyGenerators[Body::INITIAL] = (Generator*)
                                   (new ActionGenerator("initial"));
   if (!bodyGenerators[Body::INITIAL])
      llog(FATAL) << "bodyGenerators[INITIAL] is NULL!" << std::endl;

   bodyGenerators[Body::DEAD] = (Generator*)(new DeadGenerator());
   if (!bodyGenerators[Body::DEAD])
      llog(FATAL) << "bodyGenerators[DEAD] is NULL!" << std::endl;

   bodyGenerators[Body::REF_PICKUP] = (Generator*)(new RefPickupGenerator());
   if (!bodyGenerators[Body::REF_PICKUP])
      llog(FATAL) << "bodyGenerators[REF_PICKUP] is NULL!" << std::endl;

   bodyGenerators[Body::OPEN_FEET] = (Generator*)
                                     (new ActionGenerator("openFeet"));
   if (!bodyGenerators[Body::OPEN_FEET])
      llog(FATAL) << "bodyGenerators[OPEN_FEET] is NULL!" << std::endl;

   bodyGenerators[Body::THROW_IN] = (Generator*)
                                    (new ActionGenerator("throwIn"));
   if (!bodyGenerators[Body::THROW_IN])
      llog(FATAL) << "bodyGenerators[THROW_IN] is NULL!" << std::endl;

   bodyGenerators[Body::GOALIE_SIT] = (Generator*)
                                      (new ActionGenerator("goalieSit"));
   if (!bodyGenerators[Body::GOALIE_SIT])
      llog(FATAL) << "bodyGenerators[GOALIE_SIT] is NULL!" << std::endl;

   bodyGenerators[Body::GOALIE_DIVE_LEFT] = (Generator*)
                                            (new ActionGenerator("goalieDiveLeft"));
   if (!bodyGenerators[Body::GOALIE_DIVE_LEFT])
      llog(FATAL) << "bodyGenerators[GOALIE_DIVE_LEFT] is NULL!" << std::endl;

   bodyGenerators[Body::GOALIE_DIVE_RIGHT] = (Generator*)
                                             (new ActionGenerator("goalieDiveRight"));
   if (!bodyGenerators[Body::GOALIE_DIVE_RIGHT])
      llog(FATAL) << "bodyGenerators[GOALIE_DIVE_RIGHT] is NULL!" << std::endl;

   bodyGenerators[Body::GOALIE_CENTRE] = (Generator*)
                                             (new ActionGenerator("goalieCentre"));
   if (!bodyGenerators[Body::GOALIE_CENTRE])
      llog(FATAL) << "bodyGenerators[GOALIE_CENTRE] is NULL!" << std::endl;

   bodyGenerators[Body::GOALIE_UNCENTRE] = (Generator*)
                                             (new ActionGenerator("goalieUncentre"));
   if (!bodyGenerators[Body::GOALIE_UNCENTRE])
      llog(FATAL) << "bodyGenerators[GOALIE_UNCENTRE] is NULL!" << std::endl;

   bodyGenerators[Body::GOALIE_STAND] = (Generator*)
                                             (new ActionGenerator("goalieStand"));
   if (!bodyGenerators[Body::GOALIE_STAND])
      llog(FATAL) << "bodyGenerators[GOALIE_STAND] is NULL!" << std::endl;

   bodyGenerators[Body::GOALIE_INITIAL] = (Generator*)
                                             (new ActionGenerator("goalieInitial"));
   if (!bodyGenerators[Body::GOALIE_INITIAL])
      llog(FATAL) << "bodyGenerators[GOALIE_INITIAL] is NULL!" << std::endl;

   bodyGenerators[Body::GOALIE_AFTERSIT_INITIAL] = (Generator*)
                                             (new ActionGenerator("goalieInitial"));
   if (!bodyGenerators[Body::GOALIE_AFTERSIT_INITIAL])
      llog(FATAL) << "bodyGenerators[GOALIE_AFTERSIT_INITIAL] is NULL!" << std::endl;

   bodyGenerators[Body::DEFENDER_CENTRE] = (Generator*)
                                          (new ActionGenerator("defenderCentre"));
   if (!bodyGenerators[Body::DEFENDER_CENTRE])
     llog(FATAL) << "bodyGenerators[DEFENDER_CENTRE] is NULL!" << std::endl;

   bodyGenerators[Body::GOALIE_PICK_UP] = (Generator*)
                                          (new ActionGenerator("goaliePickup"));
   if (!bodyGenerators[Body::GOALIE_PICK_UP])
     llog(FATAL) << "bodyGenerators[GOALIE_PICK_UP] is NULL!" << std::endl;

   bodyGenerators[Body::GOALIE_FAST_SIT] = (Generator*)
                                          (new ActionGenerator("goalieFastSit"));
   if (!bodyGenerators[Body::GOALIE_FAST_SIT])
     llog(FATAL) << "bodyGenerators[GOALIE_FAST_SIT] is NULL!" << std::endl;

   llog(INFO) << "DistributedGenerator constructed" << std::endl;
}

/*-----------------------------------------------------------------------------
 * Destructor
 *---------------------------------------------------------------------------*/
DistributedGenerator::~DistributedGenerator() {
   delete headGenerator;
   for (uint8_t i = 0; i < Body::NUM_ACTION_TYPES; ++i)
      if (bodyGenerators[i]) {
         delete bodyGenerators[i];
         for (uint8_t j = i + 1; j < Body::NUM_ACTION_TYPES; ++j)
            if (bodyGenerators[j] == bodyGenerators[i])
               bodyGenerators[j] = NULL;
      }
   llog(INFO) << "DistributedGenerator destroyed" << std::endl;
}

/*-----------------------------------------------------------------------------
 * makeJoints
 * Returns the joint values requested by whichever generator we're using
 *---------------------------------------------------------------------------*/
JointValues DistributedGenerator::makeJoints(ActionCommand::All* request,
                                             Odometry* odometry,
                                             const SensorValues &sensors,
                                             BodyModel &bodyModel,
                                             float ballX,
                                             float ballY) {

   // If we're requesting a dive, set requestedDive variable
   if(requestedDive == Body::NONE
         && !(
            current_generator == Body::GOALIE_CENTRE ||
            current_generator == Body::GOALIE_DIVE_LEFT ||
            current_generator == Body::GOALIE_DIVE_RIGHT ||
            current_generator == Body::DEFENDER_CENTRE
            )
         && (
            request->body.actionType == Body::GOALIE_CENTRE ||
            request->body.actionType == Body::GOALIE_DIVE_LEFT ||
            request->body.actionType == Body::GOALIE_DIVE_RIGHT ||
            current_generator == Body::DEFENDER_CENTRE
            )) {
      requestedDive = request->body.actionType;
   }

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
      if (current_generator == Body::GOALIE_SIT || current_generator == Body::GOALIE_FAST_SIT){  //or fast sit
//            && request->body.actionType != Body::GOALIE_SIT
         if(requestedDive != Body::NONE){
//         if (request->body.actionType == Body::GOALIE_DIVE_LEFT ||
//             request->body.actionType == Body::GOALIE_CENTRE ||
//             request->body.actionType == Body::GOALIE_DIVE_RIGHT) {
            current_generator = requestedDive;
         } else if(request->body.actionType != Body::GOALIE_SIT){
            current_generator = Body::GOALIE_AFTERSIT_INITIAL;
         }
      // goalie after pick up should transition back through goalie uncentre
      } else if (current_generator == Body::GOALIE_PICK_UP && request->body.actionType != Body::GOALIE_PICK_UP) {
	current_generator = Body::GOALIE_UNCENTRE;   //Body::GOALIE_PICK_UP;
      } else if (current_generator == Body::GOALIE_CENTRE && request->body.actionType != Body::GOALIE_CENTRE) {
         current_generator = Body::GOALIE_UNCENTRE;
      // and goalie uncentre should transition back through goalie sit
      } else if (current_generator == Body::GOALIE_UNCENTRE && request->body.actionType != Body::GOALIE_UNCENTRE && request->body.actionType != Body::GOALIE_CENTRE) {
         current_generator = Body::GOALIE_SIT;
      // defender centre should transition back through goalie uncentre
      } else if (current_generator == Body::DEFENDER_CENTRE && request->body.actionType != Body::DEFENDER_CENTRE) {
        current_generator = Body::GOALIE_UNCENTRE;
      // anything else should transition through goalie sit to goalie centre 
      // commit to it
      } else if ((current_generator != Body::GOALIE_CENTRE &&
                 request->body.actionType == Body::GOALIE_CENTRE)
//                 || (current_generator != Body::GOALIE_DIVE_LEFT &&
//                       request->body.actionType == Body::GOALIE_DIVE_LEFT)
//                 || (current_generator != Body::GOALIE_DIVE_RIGHT &&
//                       request->body.actionType == Body::GOALIE_DIVE_RIGHT)
                 ) {
//         current_generator = Body::GOALIE_SIT;
         current_generator = Body::GOALIE_FAST_SIT;
      } else if (current_generator == Body::GOALIE_STAND && request->body.actionType != Body::GOALIE_STAND) {
         current_generator = Body::GOALIE_INITIAL;
      } else {
         current_generator = request->body.actionType;
      }
      isStopping = false;
   } else if (bodyGenerators[current_generator]->isActive() &&
              bodyGenerators[current_generator] !=
              bodyGenerators[request->body.actionType]) {
      // Special case to let kicks continue instead of being interrupted by stand
//      std::cout << current_generator << " " << request->body.actionType << std::endl;
      if (current_generator != Body::KICK || request->body.actionType != Body::STAND) {
         bodyGenerators[current_generator]->stop();
         isStopping = true;
      }
   }
//   std::cout << "active " << bodyGenerators[current_generator]->isActive() << " " << current_generator << std::endl;

   if(current_generator == requestedDive){
      requestedDive = Body::NONE;
   }

   switch (current_generator) {
   case Body::NONE:             usesHead = false; break;
   case Body::STAND:            usesHead = false; break;
   case Body::WALK:             usesHead = false; break;
   case Body::GETUP_FRONT:      usesHead = true;  break;
   case Body::GETUP_BACK:       usesHead = true;  break;
   case Body::INITIAL:          usesHead = true;  break;
   case Body::KICK:             usesHead = false; break;
   case Body::DRIBBLE:          usesHead = false; break;
   case Body::DEAD:             usesHead = true;  break;
   case Body::REF_PICKUP:       usesHead = false; break;
   case Body::OPEN_FEET:        usesHead = true;  break;
   case Body::THROW_IN:         usesHead = false; break;
   case Body::GOALIE_SIT:       usesHead = false; break;
   case Body::GOALIE_DIVE_LEFT: usesHead = true; break;
   case Body::GOALIE_DIVE_RIGHT: usesHead = true; break;
   case Body::GOALIE_CENTRE:    usesHead = false; break;
   case Body::GOALIE_UNCENTRE:  usesHead = false; break;
   case Body::GOALIE_STAND:     usesHead = false; break;
   case Body::GOALIE_INITIAL:   usesHead = false; break;
   case Body::GOALIE_AFTERSIT_INITIAL: usesHead = false; break;
   case Body::GOALIE_PICK_UP:   usesHead = true;  break;
   case Body::DEFENDER_CENTRE:  usesHead = false; break;
   case Body::GOALIE_FAST_SIT:  usesHead = false; break;
   case Body::MOTION_CALIBRATE: usesHead = false; break;
   case Body::STAND_STRAIGHT:   usesHead = false; break;
   case Body::LINE_UP:          usesHead = false; break;
   case Body::NUM_ACTION_TYPES: usesHead = false; break;
   }
   //check for dives and update odometry
   float turn = 0;
   int dir = 0;
   if(current_generator == Body::GETUP_FRONT){
      dir = 1;
   } else if(current_generator == Body::GETUP_BACK){
      dir = -1;
   }
   if(prev_generator == Body::GOALIE_DIVE_LEFT){
      turn = DEG2RAD(dir*80);
   } else if (prev_generator == Body::GOALIE_DIVE_RIGHT){
      turn = DEG2RAD(-dir*80);
   }
   *odometry = *odometry + Odometry(0, 0, turn);
   fromBody = bodyGenerators[current_generator]->
              makeJoints(request, odometry, sensors, bodyModel, ballX, ballY);

   if(current_generator == Body::KICK && request->body.actionType == Body::WALK) {
      current_generator = Body::WALK;
   }

   if (!usesHead) {
      JointValues fromHead = headGenerator->
                             makeJoints(request, odometry, sensors, bodyModel, ballX, ballY);
      for (uint8_t i = Joints::HeadYaw; i <= Joints::HeadPitch; ++i) {
         fromBody.angles[i] = fromHead.angles[i];
         fromBody.stiffnesses[i] = fromHead.stiffnesses[i];
      }
   }
   prev_generator = current_generator;
   return fromBody;
}

bool DistributedGenerator::isActive() {
   return true;
}

void DistributedGenerator::reset() {
   for (uint8_t i = 0; i < Body::NUM_ACTION_TYPES; ++i) {
      bodyGenerators[i]->reset();
   }
   headGenerator->reset();
   current_generator = ActionCommand::Body::NONE;
}

void DistributedGenerator::readOptions(const boost::program_options::variables_map &config) {
   for (uint8_t i = 0; i < Body::NUM_ACTION_TYPES; ++i) {
      bodyGenerators[i]->readOptions(config);
   }
   headGenerator->readOptions(config);
}
