#pragma once

#include "types/ActionCommand.hpp"
#include "types/AbsCoord.hpp"
#include "utils/SPLDefs.hpp"
#include "utils/basic_maths.hpp"

class Blackboard;
class BehaviourRequest;
class PostInfo;
class RRCoord;

namespace BehaviourHelpers {
   bool ourKickOff(Blackboard *blackboard);
   bool onBlueTeam(Blackboard *blackboard);
   bool onRedTeam(Blackboard *blackboard);
   int playerNumber(Blackboard *blackboard);
   int teamNumber(Blackboard *blackboard);
   bool isOwnPost(Blackboard *blackboard, const PostInfo &post);
   float angleToGoal(const AbsCoord &robotPos);
   bool isNotLocalised(Blackboard *blackboard, float max_xy_var = 1e6,
         float max_theta_var = SQUARE(UNSWDEG2RAD(12)));
   bool isLocalised(Blackboard *blackboard, float min_xy_var = 6e4,
         float min_theta_var = UNSWDEG2RAD(3));
   RRCoord absToRr(const AbsCoord &fromPos, const AbsCoord &toPos);
   float angleToPoint(Blackboard *blackboard,
         const AbsCoord &point = AbsCoord(FIELD_LENGTH/2, 0, 0));
   float ballAngleToPoint(Blackboard *blackboard,
         const AbsCoord &point = AbsCoord(FIELD_LENGTH/2, 0, 0));
   bool isAlignedWithTarget(Blackboard *blackboard, float angle = 0,
         const AbsCoord &target = AbsCoord(FIELD_LENGTH/2, 0, 0));
   bool isAlignedWithGoal(Blackboard *blackboard, float angle = 0, float safety = 0.5);
   std::pair<RRCoord, float>  getBestShootPointRr(Blackboard *blackboard,
         std::vector<float> angles = std::vector<float>(0.0, 1),
         const AbsCoord &point = AbsCoord(FIELD_LENGTH/2, 0, 0),
         int xOffset = -250, float bias=1.2);
   RRCoord getShootPointRr(Blackboard *blackboard, float angle = 0,
         const AbsCoord &point = AbsCoord(FIELD_LENGTH/2, 0, 0),
         int offset = -250);
   AbsCoord getBallPosAbs(Blackboard *blackboard);

   bool isGoalieAttacking(Blackboard *blackboard);
   float myWeightedDistanceToBall(Blackboard *blackboard);
   std::pair<float, int> minWeightedDistanceToBall(Blackboard *blackboard);

   int findNumIncapacitated(Blackboard *blackboard);

   AbsCoord getTeamBall(Blackboard *blackboard);

   extern const ActionCommand::rgb LED_OFF;
   extern const ActionCommand::rgb LED_RED;
   extern const ActionCommand::rgb LED_GREEN;
   extern const ActionCommand::rgb LED_BLUE;
   extern const ActionCommand::rgb LED_YELLOW;
   extern const ActionCommand::rgb LED_CYAN;
   extern const ActionCommand::rgb LED_MAGENTA;
   extern const ActionCommand::rgb LED_WHITE;
};
