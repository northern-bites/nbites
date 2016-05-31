#pragma once

#include "types/AbsCoord.hpp"
#include "types/RRCoord.hpp"
#include "types/ActionCommand.hpp"
#include "types/BehaviourSharedData.hpp"
#include "perception/localisation/SharedLocalisationUpdateBundle.hpp"

class BroadcastData {
   public:
      BroadcastData()
         : playerNum(0), team(0), robotPos(0.0, 0.0, 0.0), 
           ballPosAbs(), ballPosRR(), lostCount(0),
           sharedLocalisationBundle(), behaviourSharedData(),
           acB(ActionCommand::Body::DEAD), uptime(0.0) {}

      BroadcastData(const BroadcastData& bd)
         : playerNum(bd.playerNum),
           team(bd.team),
           robotPos(bd.robotPos),
           ballPosAbs(bd.ballPosAbs),
           ballPosRR(bd.ballPosRR),
           lostCount(bd.lostCount), // TODO: this really should be "ballLostCount"
           sharedLocalisationBundle(bd.sharedLocalisationBundle),
           behaviourSharedData(bd.behaviourSharedData),
           acB(bd.acB),
           uptime(bd.uptime) {}

      BroadcastData(const int &playerNum, const int &team,
                    const AbsCoord &robotPos,
                    const AbsCoord &ballPosAbs,
                    const RRCoord &ballPosRR,
                    const uint32_t &lostCount,
                    const SharedLocalisationUpdateBundle &sharedLocalisationBundle,
                    const BehaviourSharedData &behaviourSharedData,
                    const ActionCommand::Body::ActionType &acB,
                    const float &uptime)
         : playerNum(playerNum),
           team(team),
           robotPos(robotPos),
           ballPosAbs(ballPosAbs),
           ballPosRR(ballPosRR),
           lostCount(lostCount),
           sharedLocalisationBundle(sharedLocalisationBundle),
           behaviourSharedData(behaviourSharedData),
           acB(acB),
           uptime(uptime) {}
      
      int playerNum;
      int team;
      
      AbsCoord robotPos;
      AbsCoord ballPosAbs;
      RRCoord ballPosRR;
      uint32_t lostCount;
      
      SharedLocalisationUpdateBundle sharedLocalisationBundle;
      
      // Data set by the Python behaviours that is shared with other robots.
      BehaviourSharedData behaviourSharedData;

      ActionCommand::Body::ActionType acB;
      float uptime;

      template<class Archive>
      void serialize(Archive &ar, const unsigned int file_version) {
         ar & playerNum;
         ar & team;
         ar & robotPos;
         ar & ballPosAbs;
         ar & ballPosRR;
         ar & lostCount;
         ar & sharedLocalisationBundle;
         ar & behaviourSharedData;
         ar & acB;
         ar & uptime;
      }
};
