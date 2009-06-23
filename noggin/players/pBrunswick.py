
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
from . import GoalieTransitions
from . import PenaltyKickStates
from . import ChaseBallTransitions
from . import KickingConstants
from .. import NogginConstants
from . import ChaseBallConstants as ChaseConstants
from man.motion import SweetMoves
from ..util import MyMath
from math import sin, cos, radians

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(PenaltyKickStates)
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
        self.subRole = PBConstants.INIT_SUB_ROLE
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
        self.shouldApproachWithoutLoc = 0

        self.notAtPositionCounter = 0
        self.changeOmniGoToCounter = 0
        self.shouldRelocalizeCounter = 0

        self.angleToAlign = 0.0
        self.orbitAngle = 0.0
        self.ballRelY = 0.0
        self.ballRelX = 0.0

        self.kickObjective = None

        # Penalty kick player variables
        self.penaltyKicking = False
        self.penaltyMadeFirstKick = False
        self.penaltyMadeSecondKick = False


    def run(self):
        if self.currentState == 'afterKick' or \
                self.lastDiffState == 'afterKick':
            self.justKicked = True
        else:
            self.justKicked = False

        if self.brain.gameController.currentState == 'gamePlaying':
            roleState = self.getNextState()

            if self.currentRole == PBConstants.GOALIE:
                GoalieTransitions.goalieRunChecks(self)

            if roleState != self.currentState:
                self.switchTo(roleState)

        SoccerFSA.SoccerFSA.run(self)

    def getNextState(self):
        playbookRole = self.brain.playbook.role
        playbookSubRole = self.brain.playbook.subRole
        if playbookSubRole == self.subRole:
            return self.currentState
        # We don't stop chasing if we are in certain roles
        elif (self.currentRole == PBConstants.CHASER and
              ChaseBallTransitions.shouldntStopChasing(self)):
            return self.currentState
        else:
            self.currentRole = playbookRole
            self.subRole = playbookSubRole
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
            return ChaseConstants.TURN_RIGHT
        elif self.chosenKick == SweetMoves.RIGHT_FAR_KICK:
            return ChaseConstants.TURN_LEFT
        else :
            return ChaseConstants.TURN_LEFT


    def inOppCorner(self):
        my = self.brain.my
        return my.x > KickingConstants.OPP_CORNER_LEFT_X and \
            (my.y < KickingConstants.BOTTOM_OPP_CORNER_SLOPE * \
                 (my.x - KickingConstants.OPP_CORNER_LEFT_X) or
             my.y > KickingConstants.TOP_OPP_CORNER_SLOPE * \
                 (my.x - KickingConstants.OPP_CORNER_LEFT_X) + \
                 KickingConstants.TOP_OPP_CORNER_Y )

    def inFrontOfBall(self):
        ball = self.brain.ball
        my = self.brain.my
        return my.x > ball.x and \
            my.y < ChaseConstants.IN_FRONT_SLOPE*(my.x - ball.x) + ball.y and \
            my.y > -ChaseConstants.IN_FRONT_SLOPE*(my.x-ball.x) + ball.y

    def getApproachPosition(self):
        ball = self.brain.ball
        my = self.brain.my

        if self.penaltyKicking:
            destKickLocX, destKickLocY =  \
                self.getPenaltyKickingBallDest()
            destH = MyMath.getRelativeBearing(ball.x,
                                              ball.y,
                                              NogginConstants.
                                              OPP_GOAL_HEADING,
                                              destKickLocX,
                                              destKickLocY)

        elif self.inFrontOfBall():
            destH = self.getApproachHeadingFromFront()
        else :
            destH = self.getApproachHeadingFromBehind()

        destX = ball.x - ChaseConstants.APPROACH_DIST_TO_BALL * \
            cos(radians(destH))
        destY = ball.y - ChaseConstants.APPROACH_DIST_TO_BALL * \
            sin(radians(destH))
        return destX, destY, destH

    def getApproachHeadingFromBehind(self):
        ball = self.brain.ball
        ballBearingToGoal = MyMath.getRelativeBearing(ball.x, ball.y,
                                                      NogginConstants.
                                                      OPP_GOAL_HEADING,
                                                      NogginConstants.
                                                      OPP_GOALBOX_RIGHT_X,
                                                      NogginConstants.
                                                      OPP_GOALBOX_MIDDLE_Y)
        return ballBearingToGoal

    def getApproachHeadingFromFront(self):
        ball = self.brain.ball
        my = self.brain.my
        kickDest = self.getKickGoalDest()
        ballBearingToKickDest = MyMath.getRelativeBearing(ball.x,
                                                          ball.y,
                                                          NogginConstants.
                                                          OPP_GOAL_HEADING,
                                                          kickDest[0],
                                                          kickDest[1] )
        if my.y > ball.y:
            destH = ballBearingToKickDest - 90
        else :
            destH = ballBearingToKickDest + 90
        return destH

    def getKickGoalDest(self):
        return NogginConstants.OPP_GOALBOX_RIGHT_X, \
            NogginConstants.OPP_GOALBOX_MIDDLE_Y

    def getPenaltyKickingBallDest(self):
        if not self.penaltyMadeFirstKick:
            return (NogginConstants.FIELD_WIDTH * 3/4,
                    NogginConstants.FIELD_HEIGHT /4)

        return (NogginConstants.OPP_GOAL_MIDPOINT[0],
                NogginConstants.OPP_GOAL_MIDPOINT[1] )

    def ballInOppGoalBox(self):
        ball = self.brain.ball
        return NogginConstants.OPP_GOALBOX_LEFT_X < ball.x < \
            NogginConstants.OPP_GOALBOX_RIGHT_X and \
            NogginConstants.OPP_GOALBOX_TOP_Y > ball.y > \
            NogginConstants.OPP_GOALBOX_BOTTOM_Y
