
from . import SoccerFSA
from . import ChaseBallStates
from . import PositionStates
from . import FindBallStates
from . import KickingStates
from ..playbook import PBConstants
from . import BrunswickStates
from . import GoaliePositionStates
from . import GoalieChaseBallStates
from . import GoalieSaveStates
from . import ChaseBallTransitions
from . import KickingConstants
from .. import NogginConstants
from . import ChaseBallConstants
from man.motion import SweetMoves
from ..util import MyMath

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(GoaliePositionStates)
        self.addStates(GoalieChaseBallStates)
        self.addStates(GoalieSaveStates)
        self.addStates(PositionStates)
        self.addStates(FindBallStates)
        self.addStates(KickingStates)
        self.addStates(ChaseBallStates)
        self.addStates(BrunswickStates)

        self.setName('pBrunswick')
        self.currentRole = PBConstants.INIT_ROLE
        self.stoppedWalk = False
        self.currentSpinDir = None
        self.currentGait = None
        self.trackingBall = False

        self.chosenKick = None
        self.kickDecider = None
        self.justKicked = False

        self.shouldSaveCounter = 0
        self.shouldChaseCounter = 0
        self.stepsOffCenter = 0

        self.shouldAvoidObstacleRightCounter = 0
        self.shouldAvoidObstacleLeftCounter = 0
        self.doneAvoidingCounter = 0

        self.notAtPositionCounter = 0
        self.shouldRelocalizeCounter = 0

        self.angleToAlign = 0.0
        self.orbitAngle = 0.0

        self.kickObjective = None

    def run(self):
        if self.lastDiffState == 'afterKick':
            self.justKicked = True
        else:
            self.justKicked = False

        if self.brain.gameController.currentState == 'gamePlaying':
            roleState = self.getNextState()

            if roleState != self.currentState:
                self.switchTo(roleState)

        SoccerFSA.SoccerFSA.run(self)

    def getNextState(self):
        playbookRole = self.brain.playbook.currentRole
        if playbookRole == self.currentRole:
            return self.currentState
        # We don't stop chasing if we are in certain roles
        elif (self.currentRole == PBConstants.CHASER and
              ChaseBallTransitions.shouldntStopChasing(self)):
            return self.currentState
        else:
            self.currentRole = playbookRole
            return self.getRoleState(playbookRole)

    def getRoleState(self,role):
        if role == PBConstants.CHASER:
            return 'chase'
        elif role == PBConstants.OFFENDER:
            return 'playbookPosition'
        elif role == PBConstants.DEFENDER:
            return 'playbookPosition'
        elif role == PBConstants.GOALIE:
            return 'goaliePosition'
        elif role == PBConstants.PENALTY_ROLE:
            return 'gamePenalized'
        elif role == PBConstants.SEARCHER:
            return 'scanFindBall'
        else:
            return 'scanFindBall'


    def getKickObjective(self):
        """
        Figure out what to do with the ball
        """
        kickDecider = self.kickDecider
        avgOppGoalDist = 0.0

        my = self.brain.my

        if my.x < NogginConstants.FIELD_WIDTH / 2:
            return KickingConstants.OBJECTIVE_CLEAR

        elif MyMath.dist(my.x, my.y,
                       NogginConstants.OPP_GOALBOX_RIGHT_X,
                       NogginConstants.OPP_GOALBOX_MIDDLE_Y ) > \
                       NogginConstants.FIELD_WIDTH / 3 :
                       return KickingConstants.OBJECTIVE_CENTER
        elif self.inOppCorner():
            return KickingConstants.OBJECTIVE_CENTER
        else :
            return KickingConstants.OBJECTIVE_SHOOT



    ###### HELPER METHODS ######
    def getSpinDirAfterKick(self):
        if self.chosenKick == SweetMoves.LEFT_SIDE_KICK:
            return ChaseBallConstants.TURN_RIGHT
        elif self.chosenKick == SweetMoves.RIGHT_FAR_KICK:
            return ChaseBallConstants.TURN_LEFT
        else :
            return ChaseBallConstants.TURN_LEFT


    def inOppCorner(self):
        my = self.brain.my
        return my.x > KickingConstants.OPP_CORNER_LEFT_X and \
            (my.y < KickingConstants.BOTTOM_OPP_CORNER_SLOPE * \
                 (my.x - KickingConstants.OPP_CORNER_LEFT_X) or
             my.y > KickingConstants.TOP_OPP_CORNER_SLOPE * \
                 (my.x - KickingConstants.OPP_CORNER_LEFT_X) + \
                 KickingConstants.TOP_OPP_CORNER_Y )
