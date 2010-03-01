from . import SoccerFSA
from . import ChaseBallStates
from . import PositionStates
from . import FindBallStates
from . import KickingStates
from . import PenaltyKickStates
from . import GoaliePositionStates
from . import GoalieSaveStates
from . import SquatPositionStates
from . import BrunswickStates

from . import GoalieTransitions
from . import ChaseBallTransitions
from . import KickingHelpers

from . import KickingConstants
from .. import NogginConstants
from ..playbook import PBConstants
from . import ChaseBallConstants as ChaseConstants

from man.motion import SweetMoves
from man.noggin.typeDefs.Location import Location, RobotLocation

from ..util import MyMath
from math import sin, cos, radians

class SoccerPlayer(SoccerFSA.SoccerFSA):
    def __init__(self, brain):
        SoccerFSA.SoccerFSA.__init__(self,brain)
        self.addStates(PenaltyKickStates)
        self.addStates(GoaliePositionStates)
        self.addStates(GoalieSaveStates)
        self.addStates(SquatPositionStates)
        self.addStates(PositionStates)
        self.addStates(FindBallStates)
        self.addStates(KickingStates)
        self.addStates(ChaseBallStates)
        self.addStates(BrunswickStates)

        self.setName('pBrunswick')
        self.pb = None # must be init'd later, depends on player init
        self.stoppedWalk = False
        self.currentSpinDir = None
        self.currentGait = None
        self.trackingBall = False

        self.chosenKick = None
        self.kickDecider = None
        self.justKicked = False
        self.inKickingState = False

        self.shouldSaveCounter = 0
        self.shouldChaseCounter = 0
        self.shouldStopCaseCounter = 0
        self.posForSaveCounter = 0
        self.framesFromCenter = 0
        self.stepsOffCenter = 0
        self.ballRelY = 0.0
        self.ballRelX = 0.0
        self.isChasing = False
        self.saving = False

        self.shouldAvoidObstacleRightCounter = 0
        self.shouldAvoidObstacleLeftCounter = 0
        self.doneAvoidingCounter = 0
        self.shouldApproachWithoutLoc = 0

        self.notAtPositionCounter = 0
        self.changeOmniGoToCounter = 0
        self.shouldRelocalizeCounter = 0
        self.shouldChaseAroundBox = 0
        self.shouldNotChaseAroundBox = 0

        self.angleToAlign = 0.0
        self.orbitAngle = 0.0
        self.hasAlignedOnce = False
        self.bigKick = False

        self.kickObjective = None

        # Penalty kick player variables
        self.penaltyKicking = False
        self.penaltyMadeFirstKick = True
        self.penaltyMadeSecondKick = False

        # Kickoff kick
        self.hasKickedOffKick = True

        # Goalie squat save
        self.squatting = False


    def run(self):
        self.play = self.brain.play
        if self.currentState == 'afterKick' or \
                self.lastDiffState == 'afterKick':
            self.justKicked = True
        else:
            self.justKicked = False

        gcState = self.brain.gameController.currentState
        if gcState == 'gamePlaying' or\
                (gcState == 'penaltyShotsGamePlaying'
                 and self.play.isRole(PBConstants.GOALIE)):
            roleState = self.getNextState()

            if roleState != self.currentState:
                self.brain.CoA.setRobotGait(self.brain.motion)
                self.switchTo(roleState)

        SoccerFSA.SoccerFSA.run(self)

    def getNextState(self):
        if not self.brain.playbook.subRoleChanged():
            if self.play.isRole(PBConstants.GOALIE):
                state = GoalieTransitions.goalieRunChecks(self)
                return state
            return self.currentState
        # We don't stop chasing if we are in certain roles
        elif (self.play.isRole(PBConstants.CHASER) and
              ChaseBallTransitions.shouldntStopChasing(self)):
            return self.currentState
        else:
            return self.getRoleState()

    def getRoleState(self):
        if self.play.isRole(PBConstants.CHASER):
            return 'chase'
        elif ( self.play.isRole(PBConstants.OFFENDER) or
               self.play.isRole(PBConstants.DEFENDER) ):
            return 'playbookPosition'
        elif self.play.isRole(PBConstants.GOALIE):
            if (self.lastDiffState == 'gamePenalized' or
                self.lastDiffState == 'fallen'):
                return 'goaliePosition'
            elif self.squatting:
                return 'squatted'
            return 'squat'
        elif self.play.isRole(PBConstants.PENALTY_ROLE):
            return 'gamePenalized'
        elif self.play.isRole(PBConstants.SEARCHER):
            return 'scanFindBall'
        else:
            return 'scanFindBall'





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
            destKickLoc = self.getPenaltyKickingBallDest()
            ballLoc = RobotLocation(ball.x, ball.y, NogginConstants.OPP_GOAL_HEADING)
            destH = MyMath.getRelativeBearing(destKickLoc)

        elif self.shouldMoveAroundBall():
            return self.getPointToMoveAroundBall()
        elif self.inFrontOfBall():
            destH = self.getApproachHeadingFromFront()
        else :
            destH = self.getApproachHeadingFromBehind()

        destX = ball.x - ChaseConstants.APPROACH_DIST_TO_BALL * \
            cos(radians(destH))
        destY = ball.y - ChaseConstants.APPROACH_DIST_TO_BALL * \
            sin(radians(destH))
        return RobotLocation(destX, destY, destH)

    def getApproachHeadingFromBehind(self):
        ball = self.brain.ball
        aimPoint = KickingHelpers.getShotFarAimPoint(self)
        ballLoc = RobotLocation(ball.x, ball.y, NogginConstants.OPP_GOAL_HEADING)
        ballBearingToGoal = ballLoc.getRelativeBearing(aimPoint)
        return ballBearingToGoal

    def getApproachHeadingFromFront(self):
        ball = self.brain.ball
        my = self.brain.my
        kickDest = KickingHelpers.getShotFarAimPoint(self)
        ballLoc = RobotLocation(ball.x, ball.y, NogginConstants.OPP_GOAL_HEADING)
        ballBearingToKickDest = ballLoc.getRelativeBearing(kickDest)
        if my.y > ball.y:
            destH = ballBearingToKickDest - 90
        else :
            destH = ballBearingToKickDest + 90
        return destH

    def getPenaltyKickingBallDest(self):
        if not self.penaltyMadeFirstKick:
            return (NogginConstants.FIELD_WIDTH * 3/4,
                    NogginConstants.FIELD_HEIGHT /4)

        return Location(NogginConstants.OPP_GOAL_MIDPOINT[0],
                        NogginConstants.OPP_GOAL_MIDPOINT[1] )

    def ballInOppGoalBox(self):
        ball = self.brain.ball
        return NogginConstants.OPP_GOALBOX_LEFT_X < ball.x < \
            NogginConstants.OPP_GOALBOX_RIGHT_X and \
            NogginConstants.OPP_GOALBOX_TOP_Y > ball.y > \
            NogginConstants.OPP_GOALBOX_BOTTOM_Y

    def ballInMyGoalBox(self):
        ball = self.brain.ball
        return NogginConstants.MY_GOALBOX_LEFT_X < ball.x < \
            NogginConstants.MY_GOALBOX_RIGHT_X and \
            NogginConstants.MY_GOALBOX_TOP_Y > ball.y > \
            NogginConstants.MY_GOALBOX_BOTTOM_Y

    def lookPostKick(self):
        tracker = self.brain.tracker
        if self.chosenKick == SweetMoves.LEFT_FAR_KICK or \
                self.chosenKick == SweetMoves.RIGHT_FAR_KICK:
            tracker.lookToDir('up')
        elif self.chosenKick == SweetMoves.RIGHT_SIDE_KICK:
            tracker.lookToDir('left')
        elif self.chosenKick == SweetMoves.LEFT_SIDE_KICK:
            tracker.lookToDir('right')

    def getNextOrbitPos(self):
        relX = -ChaseConstants.ORBIT_OFFSET_DIST * \
            cos(radians(ChaseConstants.ORBIT_STEP_ANGLE)) + self.brain.ball.relX
        relY =  -ChaseConstants.ORBIT_OFFSET_DIST * \
            sin(radians(ChaseConstants.ORBIT_STEP_ANGLE)) + self.brain.ball.relY
        relTheta = ChaseConstants.ORBIT_STEP_ANGLE * 2 + self.brain.ball.bearing
        return RobotLocation(relX, relY, relTheta)

    def shouldMoveAroundBall(self):
        return (self.brain.ball.x < self.brain.my.x
                and (self.brain.my.x - self.brain.ball.x) <
                75.0 )

    def getPointToMoveAroundBall(self):
        ball = self.brain.ball
        x = ball.x

        if ball.y > self.brain.my.y:
            y = self.brain.ball.y - 75.0
            destH = 90.0
        else:
            y = self.brain.ball.y + 75.0
            destH = -90.0

        return RobotLocation(x, y, destH)
