
from . import SoccerFSA
from . import ChaseBallStates
from . import PositionStates
from . import FindBallStates
from . import KickingStates
from ..playbook import PBConstants
from .. import NogginConstants
from math import hypot
from ..util.MyMath import safe_atan2
from . import BrunswickStates
from . import GoaliePositionStates
from . import GoalieChaseBallStates

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(GoaliePositionStates)
        self.addStates(GoalieChaseBallStates)
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
        self.sawOwnGoal = False
        self.sawOppGoal = False
        self.oppGoalLeftPostBearings = []
        self.oppGoalRightPostBearings = []
        self.myGoalLeftPostBearings = []
        self.myGoalRightPostBearings = []
        self.trackingBall = False

    def run(self):
        if self.brain.gameController.currentState == 'gamePlaying':
            roleState = self.getNextState()

            if roleState != self.currentState:
                self.switchTo(roleState)

        SoccerFSA.SoccerFSA.run(self)

    def getNextState(self):
        playbookRole = self.brain.playbook.currentRole
        if playbookRole == self.currentRole:
            return self.currentState
        else :
            self.currentRole = playbookRole
            return self.getRoleState(playbookRole)

    def getRoleState(self,role):
        if role == PBConstants.CHASER:
            return 'chase'
            #return 'decideKick'
        elif role == PBConstants.OFFENDER:
            #return 'playbookPosition'
            return 'spinFindBallPosition'
        elif role == PBConstants.DEFENDER:
            #return 'playbookPosition'
            return 'spinFindBallPosition'
        elif role == PBConstants.GOALIE:
            #return 'goaliePosition'
            return 'goaliePosition'
        elif role == PBConstants.PENALTY_ROLE:
            return 'gamePenalized'
        elif role == PBConstants.SEARCHER:
            return 'scanFindBall'
        else:
            return 'scanFindBall'

    def getBehindBallPosition(self):
        dist_from_ball = 30

        ball = self.brain.ball

        delta_y = ball.y - NogginConstants.OPP_GOALBOX_MIDDLE_Y
        delta_x = ball.x - NogginConstants.OPP_GOALBOX_LEFT_X

        pos_x = ball.x - (dist_from_ball/
                                     hypot(delta_x,delta_y))*delta_x
        pos_y = ball.y + (dist_from_ball/
                                     hypot(delta_x,delta_y))*delta_y
        heading = -safe_atan2(delta_y,delta_x)

        return pos_x,pos_y,heading
