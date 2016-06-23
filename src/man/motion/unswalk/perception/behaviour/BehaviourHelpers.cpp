#include "BehaviourHelpers.hpp"

#include <cmath>

#include "blackboard/Blackboard.hpp"
#include "utils/basic_maths.hpp"
#include "types/AbsCoord.hpp"
#include "types/RRCoord.hpp"
#include "gamecontroller/RoboCupGameControlData.hpp"

namespace BehaviourHelpers {
   bool ourKickOff(Blackboard *blackboard) {
      return (readFrom(gameController, our_team.teamColour) == readFrom(gameController, data.kickOffTeam));
   }

   const ActionCommand::rgb LED_OFF(false, false, false);
   const ActionCommand::rgb LED_RED(true, false, false);
   const ActionCommand::rgb LED_GREEN(false, true, false);
   const ActionCommand::rgb LED_BLUE(false, false, true);
   const ActionCommand::rgb LED_YELLOW(true, true, false);
   const ActionCommand::rgb LED_CYAN(false, true, true);
   const ActionCommand::rgb LED_MAGENTA(true, false, true);
   const ActionCommand::rgb LED_WHITE(true, true, true);

   bool onBlueTeam(Blackboard *blackboard) {
      return (readFrom(gameController, our_team.teamColour) == TEAM_BLUE);
   }

   bool onRedTeam(Blackboard *blackboard) {
      return (readFrom(gameController, our_team.teamColour) == TEAM_RED);
   }

   int playerNumber(Blackboard *blackboard) {
      return (readFrom(gameController, player_number));
   }

   int teamNumber(Blackboard *blackboard) {
      return (readFrom(gameController, our_team.teamNumber));
   }

   bool isOwnPost(Blackboard *blackboard, const PostInfo &post) {
      /*
      bool posts_blue = (post.type & PostInfo::pBlue);
      bool team_red = onRedTeam(blackboard);
      return (posts_blue ^ team_red);
      */
      return false;
   }

   float angleToGoal(const AbsCoord &robotPos) {
      float goalX = FIELD_LENGTH/2.0;
      float goalY = 0.0;
      float phi = atan2f(goalY - robotPos.y(), goalX - robotPos.x());
      return NORMALISE(phi - robotPos.theta());
   }

   bool isNotLocalised(Blackboard *blackboard, float max_xy_var,
         float max_theta_var) {
      const AbsCoord &pos = readFrom(localisation, robotPos);
      return ((pos.getVar(0,0) > max_xy_var)
            || (pos.getVar(1,1) > max_xy_var)
            || (pos.getVar(2,2) > max_theta_var));
   }

   bool isLocalised(Blackboard *blackboard, float min_xy_var,
         float min_theta_var) {
      const AbsCoord &pos = readFrom(localisation, robotPos);
      return ((pos.getVar(0,0) < min_xy_var)
            && (pos.getVar(1,1) < min_xy_var)
            && (pos.getVar(2,2) < min_theta_var));
   }

   RRCoord absToRr(const AbsCoord &fromPos, const AbsCoord &toPos) {
      float dist = hypot(toPos.x()-fromPos.x(), toPos.y()-fromPos.y());
      float heading = NORMALISE(atan2f(toPos.y()-fromPos.y(), toPos.x()-fromPos.x())-fromPos.theta());
      return RRCoord(dist, heading);
   }

   float angleToPoint(Blackboard *blackboard, const AbsCoord &point) {
      const AbsCoord &robotPos = readFrom(localisation, robotPos);
      return NORMALISE(atan2f(point.y() - robotPos.y(), point.x() - robotPos.x()));
   }

   float ballAngleToPoint(Blackboard *blackboard, const AbsCoord &point) {
      const AbsCoord &ballPos = readFrom(localisation, ballPos);
      return NORMALISE(atan2f(point.y() - ballPos.y(), point.x() - ballPos.x()));
   }


   bool isAlignedWithTarget(Blackboard *blackboard, float angle, const AbsCoord &target) {
      int xOffset;
      int yOffset;
      if (target.x() == 2999) {
         xOffset = 0;
         yOffset = 100;
      } else {
         xOffset = 200;
         yOffset = 0;
      }
      const AbsCoord &robotPos = readFrom(localisation, robotPos);
      float angle1 = NORMALISE(BehaviourHelpers::angleToPoint(blackboard,
               AbsCoord(target.x() + xOffset, target.y() + yOffset, 0)) - robotPos.theta());
      float angle2 = NORMALISE(angleToPoint(blackboard,
               AbsCoord(target.x() - xOffset, target.y() - yOffset, 0)) - robotPos.theta());

      // adjust for shoot angle
      angle1 -= angle;
      angle2 -= angle;
      angle1 = NORMALISE(angle1);
      angle2 = NORMALISE(angle2);

      return ((angle1 > 0 && angle2 < 0) || (angle1 < 0 && angle2 > 0));

   }

   bool isAlignedWithGoal(Blackboard *blackboard, float angle, float safety) {
      const AbsCoord &robotPos = readFrom(localisation, robotPos);
      float leftGoal = NORMALISE(ballAngleToPoint(blackboard,
               AbsCoord(FIELD_LENGTH/2, safety * GOAL_WIDTH/2, 0)) - robotPos.theta());
      float rightGoal = NORMALISE(ballAngleToPoint(blackboard,
               AbsCoord(FIELD_LENGTH/2, safety * -GOAL_WIDTH/2, 0)) - robotPos.theta());

      //adjust for shoot angle
      leftGoal -= angle;
      rightGoal -= angle;
      leftGoal = NORMALISE(leftGoal);
      rightGoal = NORMALISE(rightGoal);

      return (leftGoal > 0 && rightGoal < 0) || (leftGoal < 0 && rightGoal > 0);
   }

   std::pair<RRCoord, float> getBestShootPointRr(Blackboard *blackboard, 
         std::vector<float> angles, const AbsCoord &point, int xOffset, float bias) {
      std::vector<RRCoord> points;
      for (std::vector<float>::iterator i = angles.begin(); i != angles.end(); ++i) {
         points.push_back(BehaviourHelpers::getShootPointRr(blackboard, *i, point, xOffset));
      }
      int bestPoint = 0;
      for (unsigned i = 0; i < points.size(); ++i) {
         if ((points[i].distance() + bias * ABS(RAD2DEG(angles[i]))) <
             (points[bestPoint].distance() + bias *  ABS(RAD2DEG(angles[bestPoint])))) {
            bestPoint = i;
         }
      }
      return std::pair<RRCoord, float>(points[bestPoint], angles[bestPoint]);
   }

   RRCoord getShootPointRr(Blackboard *blackboard, float angle,
         const AbsCoord &point, int offset) {

      AbsCoord ballPos = BehaviourHelpers::getBallPosAbs(blackboard);

      Eigen::Vector2f vector(point.x() - ballPos.x(), point.y() - ballPos.y());
      vector.normalize();

      // adjust for lineup
      Eigen::Vector2f lineup(40 * vector[1], - 40 * vector[0]);

      // scale
      vector *= offset;

      // add lineup
      //if (angle > 0) {
      //   vector -= lineup;
      //} else {
      //   vector += lineup;
     // }

      Eigen::Vector2f lineUpVector(vector[0] * cosf(-angle) - vector[1] * sinf(-angle),
                                   vector[0] * sinf(-angle) + vector[1] * cosf(-angle));

      const AbsCoord &pos = readFrom(localisation, robotPos);
      return absToRr(AbsCoord(pos.x(), pos.y(), pos.theta()),
                     AbsCoord(ballPos.x() + lineUpVector[0], ballPos.y() + lineUpVector[1], 0));
   }

   AbsCoord getBallPosAbs(Blackboard *blackboard) {
      return readFrom(localisation, ballPos);

//      const AbsCoord &robotPos = readFrom(localisation, robotPos)[0];
//      float ballX = readFrom(localisation, ballPosRRC).x();
//      float ballY = readFrom(localisation, ballPosRRC).y();
//      float x = robotPos.x() + cosf(robotPos.theta()) * ballX - sinf(robotPos.theta()) * ballY;
//      float y = robotPos.y() + sinf(robotPos.theta()) * ballX + cosf(robotPos.theta()) * ballY;
//      return AbsCoord(x, y, 0.0);
   }
   bool isGoalieAttacking(Blackboard *blackboard) {
      const bool *incapacitated = readFrom(receiver, incapacitated);
      const BroadcastData *teamData = readFrom(receiver, data);
      return (!incapacitated[0] && teamData[0].behaviourSharedData.goalieAttacking);
   }


   float myWeightedDistanceToBall(Blackboard *blackboard) {
      int lostCount = readFrom(localisation, ballLostCount);
      AbsCoord robotPos = readFrom(localisation, robotPos);
      
      AbsCoord ballPosAbs = readFrom(localisation, ballPos);
      RRCoord ballPos = ballPosAbs.convertToRobotRelative(robotPos);
      
      float turnAngle = ABS(NORMALISE(BehaviourHelpers::angleToGoal(robotPos)));
      float weightedDist = ballPos.distance() + turnAngle*(1000.0/M_PI);
      if (lostCount > 210) {
         weightedDist += 1000;
      }
      if (lostCount > 300) {
         weightedDist += 2000;
      }
      return weightedDist;
   }

   int findNumIncapacitated(Blackboard *blackboard) {
      const bool *incapacitated = readFrom(receiver, incapacitated);
      int incapCount = 0;
      for (int i = 0; i < ROBOTS_PER_TEAM; ++i) {
         if (incapacitated[i]) {
            ++incapCount;
         }
      }
      return incapCount;
   }

   AbsCoord getTeamBall(Blackboard *blackboard) {
      // TODO(sushkov): we might implement a distinction between a global ball and
      // a self-only ball filter. If/when this is done, change this function.
      // For now, simply return the ball position.
      return readFrom(localisation, ballPos);
   }

};
