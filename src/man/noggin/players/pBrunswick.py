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

        self.shouldSaveCounter = 0
        self.shouldChaseCounter = 0
        self.shouldStopCaseCounter = 0
        self.posForSaveCounter = 0
        self.framesFromCenter = 0
        self.stepsOffCenter = 0
        self.isChasing = False
        self.saving = False

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


        if NogginConstants.SAVE_FRAMES:
            if gcState == 'gamePlaying':
                self.brain.sensors.saveFrame()
            ##### IF 3000 IMAGES TAKEN, GO TO FINISH #####
        SoccerFSA.SoccerFSA.run(self)

    def getNextState(self):
        if self.brain.my.playerNumber == 1:
            state = GoalieTransitions.goalieRunChecks(self)
            return state

        elif self.brain.playbook.subRoleUnchanged():
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
