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
from . import GoalieChanges
from . import GoalieTransitions

from .. import NogginConstants
from ..playbook import PBConstants
from . import ChaseBallConstants as ChaseConstants

from man.noggin.typeDefs.Location import Location

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

        self.justKicked = False
        self.inKickingState = False

        #GOALIE COUNTERS AND BOOLEANS
        # Counters for goalie dive decision making
        self.counterRightSave = 0
        self.counterLeftSave = 0
        self.counterCenterSave = 0
        self.shouldSaveCounter = 0

        #Counters for goalie positioning
        self.shouldPositionRightCounter = 0
        self.shouldPositionLeftCounter = 0
        self.shouldPositionCenterCounter = 0

        #Goalie Chasing Counters
        self.shouldChaseCounter = 0
        self.shouldStopChaseCounter = 0

        #Boolean for Goalie Current State
        self.isChasing = False
        self.isSaving = False
        self.isPositioning = False

        #END GOALIE COUNTERS AND BOOLEANS

        self.frameCounter = 0

        self.shouldRelocalizeCounter = 0

        # Penalty kick player variables
        self.penaltyKicking = False
        self.penaltyMadeFirstKick = True
        self.penaltyMadeSecondKick = False

        # Kickoff kick
        self.hasKickedOffKick = True

        self.lastKickScanCounter = 0
        self.hasAlignedOnce = True
        self.angleToOrbit = 0.0

    def run(self):
        self.play = self.brain.play
        if self.currentState == 'afterKick' or \
               self.lastDiffState == 'afterKick':
            self.hasKickedOffKick = True
            self.justKicked = True
        else:
            self.justKicked = False

        gcState = self.brain.gameController.currentState
        if not self.firstFrame() and (gcState == 'gamePlaying' or\
               (gcState == 'penaltyShotsGamePlaying'
                and self.play.isRole(PBConstants.GOALIE))):
            roleState = self.getNextState()

            if roleState != self.currentState:
                self.switchTo(roleState)

        SoccerFSA.SoccerFSA.run(self)

    def getNextState(self):
        if self.play.isRole(PBConstants.GOALIE):
            return GoalieChanges.goalieStateChoice(self)

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
        if self.brain.my.playerNumber == 1:
            return 'goaliePosition'
        else:
            return 'playbookPosition'


    ###### HELPER METHODS ######
    def inFrontOfBall(self):
        ball = self.brain.ball
        my = self.brain.my
        return my.x > ball.x and \
            my.y < ChaseConstants.IN_FRONT_SLOPE*(my.x - ball.x) + ball.y and \
            my.y > -ChaseConstants.IN_FRONT_SLOPE*(my.x-ball.x) + ball.y

    def getPenaltyKickingBallDest(self):
        if not self.penaltyMadeFirstKick:
            return Location(NogginConstants.FIELD_WIDTH * 3./4.,
                            NogginConstants.FIELD_HEIGHT /4.)

        return Location(NogginConstants.OPP_GOAL_MIDPOINT[0],
                        NogginConstants.OPP_GOAL_MIDPOINT[1] )
