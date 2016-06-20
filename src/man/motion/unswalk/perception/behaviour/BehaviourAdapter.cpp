#include <limits>
#include <utility>
#include <vector>
#include <sstream>
#include <string>
#include <cmath>
#include "perception/behaviour/BehaviourAdapter.hpp"
#include "perception/behaviour/BehaviourHelpers.hpp"

#include "ReadySkillPositionAllocation.hpp"
#include "blackboard/Blackboard.hpp"
#include "utils/Logger.hpp"
#include "types/BehaviourRequest.hpp"
#include "utils/body.hpp"
#include "utils/speech.hpp"
#include "utils/basic_maths.hpp"
#include "types/UNSWSensorValues.hpp"
#include "perception/behaviour/python/PythonSkill.hpp"

#include <boost/python.hpp>


using namespace std;
using namespace boost::python;

BehaviourAdapter::BehaviourAdapter(Blackboard *bb) : Adapter(bb), calibrationSkill(bb) {
   llog(INFO) << "Constructing BehaviourAdapter" << endl;
   pythonSkill = new PythonSkill(bb);

   std::string colour = (BehaviourHelpers::onRedTeam(blackboard) ? "red" : "blue");
   std::stringstream startupSpeech;
   startupSpeech << std::string("Player ") << BehaviourHelpers::playerNumber(blackboard) <<
      " team " << BehaviourHelpers::teamNumber(blackboard) << " " << colour;
   SAY(startupSpeech.str());
}

BehaviourAdapter::~BehaviourAdapter() {
}

void BehaviourAdapter::tick() {
   BehaviourRequest behaviourRequest;
   // Run the python skills
   behaviourRequest = pythonSkill->execute();

   if (behaviourRequest.needsClamping) {
      // If the behaviour requested clamping, clamp the params.
      ellipsoidClampWalk(behaviourRequest.actions.body.forward,
                         behaviourRequest.actions.body.left,
                         behaviourRequest.actions.body.turn);
   }
   // run current skill, or kinematics calibrator
   if (readFrom(kinematics, isCalibrating)) {
      behaviourRequest = calibrationSkill.execute();
   } else {
      //behaviourRequest = skillInstance->tick(blackboard); //, Skill::NULL_FLAGS);
   }
   
   // override camera from offnao if necessary
   string whichCamera = blackboard->config["default.whichCamera"].as<string>();
   if (whichCamera != "BEHAVIOUR") {
      if (whichCamera == "TOP_CAMERA") {
         behaviourRequest.whichCamera = TOP_CAMERA;
      } else if (whichCamera == "BOTTOM_CAMERA") {
         behaviourRequest.whichCamera = BOTTOM_CAMERA;
      }
   }
   
   // Write ActionCommands to blackboard
   int writeBuf = (readFrom(behaviour, readBuf) + 1) % 2;
   writeTo(behaviour, request[writeBuf], safetySkill.wrapRequest(behaviourRequest, readFrom(motion, sensors)));
   writeTo(behaviour, readBuf, writeBuf);

   if (behaviourRequest.readyPositionAllocation0 > 0) {
      std::vector<int> readyAllocation;
      readyAllocation.push_back(behaviourRequest.readyPositionAllocation0);
      readyAllocation.push_back(behaviourRequest.readyPositionAllocation1);
      readyAllocation.push_back(behaviourRequest.readyPositionAllocation2);
      readyAllocation.push_back(behaviourRequest.readyPositionAllocation3);
      readyAllocation.push_back(behaviourRequest.readyPositionAllocation4);
      
      ReadySkillPositionAllocation newPositionAllocation(
            readFrom(gameController, player_number), readyAllocation);
      
      ReadySkillPositionAllocation currentPositionAllocation =
            readFrom(behaviour, behaviourSharedData).readyPositionAllocation;
      if (newPositionAllocation.canOverride(currentPositionAllocation)) {
         writeTo(behaviour, behaviourSharedData.readyPositionAllocation, newPositionAllocation);
      }
   }
   
   // Write GoalieAttacking request to behaviour blackboard, from where it'll be broadcast to the team
   writeTo(behaviour, behaviourSharedData.goalieAttacking, behaviourRequest.goalieAttacking);
   writeTo(behaviour, behaviourSharedData.doingBallLineUp, behaviourRequest.doingBallLineUp);
   writeTo(behaviour, behaviourSharedData.isInReadyMode, behaviourRequest.isInReadyMode);
   writeTo(behaviour, behaviourSharedData.timeToReachBall, behaviourRequest.timeToReachBall);
   writeTo(behaviour, behaviourSharedData.timeToReachDefender, behaviourRequest.timeToReachDefender);
   writeTo(behaviour, behaviourSharedData.timeToReachMidfielder, behaviourRequest.timeToReachMidfielder);
   writeTo(behaviour, behaviourSharedData.timeToReachUpfielder, behaviourRequest.timeToReachUpfielder);
   writeTo(behaviour, behaviourSharedData.currentRole, behaviourRequest.currentRole);
}

void BehaviourAdapter::ellipsoidClampWalk(int &forward, int &left, float &turn) {
   float MAX_FORWARD = 300.0;
   float MAX_LEFT = 150.0;
   float MAX_TURN = 0.87;

   float clampedForward = crop((float)forward, -MAX_FORWARD, MAX_FORWARD);
   float clampedLeft = crop((float)left, -MAX_LEFT, MAX_LEFT);
   float clampedTurn = crop(turn, -MAX_TURN, MAX_TURN);

   // Values in range [-1..1]
   float forwardAmount = clampedForward / MAX_FORWARD;
   float leftAmount = clampedLeft / MAX_LEFT;
   float turnAmount = clampedTurn / MAX_TURN;

   clampToSurface(forwardAmount, leftAmount, turnAmount);
   
   forward = (int)(clampedForward * fabs(forwardAmount));
   left = (int)(clampedLeft * fabs(leftAmount));
   turn = (float)(clampedTurn * fabs(turnAmount));
}

// Forward/left/turn should be in the range [-1...1]
void BehaviourAdapter::clampToSurface(float &forward, float &left, float &turn) {
   float x = fabs(forward);
   float y = fabs(left);
   float z = fabs(turn);

   // see if the point we are given is already inside the allowed walk params volume
   if (evaluateWalkVolume(x, y, z) < 1.0) {
      return;
   }

   float scale = 0.5;
   float high = 1.0;
   float low = 0.0;
   
   // This is basically a binary search to find the point on the surface.
   for (unsigned i = 0; i < 10; i++) {
      x = fabs(forward) * scale;
      y = fabs(left) * scale;
      z = fabs(turn) * scale;
      
      if (evaluateWalkVolume(x, y, z) > 1.0) {
         float newScale = (scale + low) / 2.0;
         high = scale;
         scale = newScale;
      } else {
         float newScale = (scale + high) / 2.0;
         low = scale;
         scale = newScale;
      }
   }

   forward *= scale;
   left *= scale;
   turn *= scale;
}

// x = forward, y = left, z = turn
float BehaviourAdapter::evaluateWalkVolume(float x, float y, float z) {
   // his affects the relationship between forward and left.
   float e = 1.25;
   
   // lower value allows turn to be higher with a high forward/left, higher values dont allow a high turn
   float n = 1.2;

   float r = 2.0 / e;
   float t = 2.0 / n;

   return pow(pow(x, r) + pow(y, r), (t / r)) + pow(z, t);
}
