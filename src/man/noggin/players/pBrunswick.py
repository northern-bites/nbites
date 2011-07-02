from . import SoccerFSA
from . import ChaseBallStates
from . import PositionStates
from . import PenaltyStates
from . import FindBallStates
from . import KickingStates
from . import PenaltyKickStates
from . import GoaliePositionStates
from . import GoalieSaveStates
from . import BrunswickStates

import noggin_constants as NogginConstants
from ..playbook import PBConstants

from objects import Location

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(PenaltyKickStates)
        self.addStates(GoaliePositionStates)
        self.addStates(GoalieSaveStates)
        self.addStates(PositionStates)
        self.addStates(PenaltyStates)
        self.addStates(FindBallStates)
        self.addStates(KickingStates)
        self.addStates(ChaseBallStates)
        self.addStates(BrunswickStates)

        self.setName('pBrunswick')

        self.inKickingState = False

        #GOALIE COUNTERS AND BOOLEANS
        # Counters for goalie dive decision making
        self.counterRightSave = 0
        self.counterLeftSave = 0
        self.counterCenterSave = 0

        self.isSaving = False
        self.shouldSaveCounter = 0

        #END GOALIE COUNTERS AND BOOLEANS

        self.frameCounter = 0

        self.shouldRelocalizeCounter = 0

        # Orbiting
        self.angleToOrbit = 0.0

    def run(self):
        self.play = self.brain.play
        gcState = self.brain.gameController.currentState

        if (gcState == 'gamePlaying'):
            roleState = self.getNextState()

        # Goalie Penalty Kicking
        if (gcState == 'penaltyShotsGamePlaying'
                 and self.play.isRole(PBConstants.GOALIE)):
            self.penaltyKicking = True
            roleState = self.getNextState()

            if roleState != self.currentState:
                self.switchTo(roleState)

        SoccerFSA.SoccerFSA.run(self)

    def getNextState(self):
        if self.brain.playbook.subRoleUnchanged():
            return self.currentState

        elif self.inKickingState:
            return self.currentState

        else:
            return self.getRoleState()

    def getRoleState(self):
        if self.play.isRole(PBConstants.CHASER):
            return 'chase'
        elif self.play.isRole(PBConstants.PENALTY_ROLE):
            return 'gamePenalized'
        if self.play.isRole(PBConstants.GOALIE):
            return self.getRoleStateGoalie()
        else:
            return 'playbookPosition'

    def getRoleStateGoalie(self):
        if self.play.isSubRole(PBConstants.GOALIE_KICKOFF):
            return 'kickOffPosition'
        elif self.play.isSubRole(PBConstants.GOALIE_CHASER):
            return 'goalieChase'
        elif self.play.isSubRole(PBConstants.GOALIE_SAVE):
            return 'goalieSave'
        else:
            return 'goaliePosition'

    ###### HELPER METHODS ######
    def getPenaltyKickingBallDest(self):
        if not self.penaltyMadeFirstKick:
            return Location(NogginConstants.FIELD_WIDTH * 3./4.,
                            NogginConstants.FIELD_HEIGHT /4.)

        return Location(NogginConstants.OPP_GOAL_MIDPOINT[0],
                        NogginConstants.OPP_GOAL_MIDPOINT[1] )
