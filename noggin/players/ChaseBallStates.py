"""
Here we house all of the state methods used for chasing the ball
"""
from man.noggin.util import MyMath
from man.motion import SweetMoves
import ChaseBallConstants as constants
import ChaseBallTransitions as transitions
import GoalieTransitions as goalTran
import PositionConstants
from .. import NogginConstants
from ..playbook import PBConstants as pbc
from math import fabs
import man.motion.RobotGaits as RobotGaits

def chase(player):
    """
    Method to determine which chase state should be used.
    We dump the robot into this state when we our switching from something else.
    """
    if player.currentRole == pbc.GOALIE:
        if transitions.shouldScanFindBall(player):
            return player.goNow('goalieScanFindBall')
        elif transitions.shouldApproachBall(player):
            return player.goNow('approachBall')
        elif transitions.shouldKick(player):
            return player.goNow('waitBeforeKick')
        elif transitions.shouldTurnToBall_ApproachBall(player):
            return player.goNow('turnToBall')
        elif transitions.shouldSpinFindBall(player):
            return player.goNow('spinFindBall')
        else:
            return player.goNow('goalieScanFindBall')

    if transitions.shouldScanFindBall(player):
        return player.goNow('scanFindBall')
    elif transitions.shouldApproachBallWithLoc(player):
        return player.goNow('approachBallWithLoc')
    elif transitions.shouldApproachBall(player):
        return player.goNow('approachBall')
    elif transitions.shouldKick(player):
        return player.goNow('waitBeforeKick')
    elif transitions.shouldTurnToBall_ApproachBall(player):
        return player.goNow('turnToBall')
    elif transitions.shouldSpinFindBall(player):
        return player.goNow('spinFindBall')
    else:
        return player.goNow('scanFindBall')

def chaseAfterKick(player):
    if player.firstFrame():
        player.brain.CoA.setRobotGait(player.brain.motion)

        player.brain.tracker.trackBall()

        if player.chosenKick == SweetMoves.LEFT_FAR_KICK or \
                player.chosenKick == SweetMoves.RIGHT_FAR_KICK:
            return player.goLater('chase')

        if player.chosenKick == SweetMoves.LEFT_SIDE_KICK:
            turnDir = constants.TURN_RIGHT

        elif player.chosenKick == SweetMoves.RIGHT_SIDE_KICK:
            turnDir = constants.TURN_LEFT

        player.setSpeed(0, 0, turnDir * constants.BALL_SPIN_SPEED)
        return player.stay()

    if player.brain.ball.framesOn > constants.BALL_ON_THRESH:
        return player.goLater('chase')
    elif player.counter > constants.CHASE_AFTER_KICK_FRAMES:
        return player.goLater('spinFindBall')
    return player.stay()

def turnToBall(player):
    """
    Rotate to align with the ball. When we get close, we will approach it
    """
    ball = player.brain.ball

    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.brain.CoA.setRobotGait(player.brain.motion)

    # Determine the speed to turn to the ball
    turnRate = MyMath.clip(ball.bearing*constants.BALL_SPIN_GAIN,
                           -constants.BALL_SPIN_SPEED,
                           constants.BALL_SPIN_SPEED)

    # Avoid spinning so slowly that we step in place
    if fabs(turnRate) < constants.MIN_BALL_SPIN_MAGNITUDE:
        turnRate = MyMath.sign(turnRate)*constants.MIN_BALL_SPIN_MAGNITUDE

    if player.currentRole == pbc.GOALIE:
        if transitions.shouldKick(player):
            return player.goNow('waitBeforeKick')
        elif transitions.shouldPositionForKick(player):
            return player.goNow('positionForKick')
        elif transitions.shouldApproachBall(player):
            return player.goLater('approachBall')
        elif transitions.shouldScanFindBall(player):
            return player.goLater('goalieScanFindBall')
    else:
        if transitions.shouldKick(player):
            return player.goNow('waitBeforeKick')
        elif transitions.shouldPositionForKick(player):
            return player.goNow('positionForKick')
        elif transitions.shouldApproachBall(player):
            return player.goLater('approachBall')
        elif transitions.shouldScanFindBall(player):
            return player.goLater('scanFindBall')

    if ball.on:
        player.setSpeed(x=0,y=0,theta=turnRate)

    return player.stay()

def approachBallWithLoc(player):
    if player.firstFrame():
        player.brain.CoA.setRobotGait(player.brain.motion)

    nav = player.brain.nav
    my = player.brain.my
    if player.currentRole == pbc.GOALIE:
        if transitions.shouldKick(player):
            return player.goNow('waitBeforeKick')
        elif transitions.shouldPositionForKickFromApproachLoc(player):
            return player.goLater('positionForKick')
        elif transitions.shouldAvoidObstacleDuringApproachBall(player):
            return player.goLater('avoidObstacle')
        elif my.locScoreFramesBad > constants.APPROACH_NO_LOC_THRESH:
            return player.goLater('approachBall')
        elif not player.brain.tracker.activeLocOn and \
                transitions.shouldScanFindBall(player):
            return player.goLater('scanFindBall')
        elif player.brain.tracker.activeLocOn and \
                transitions.shouldScanFindBallActiveLoc(player):
            return player.goLater('scanFindBall')
    else:
        if transitions.shouldKick(player):
            return player.goNow('waitBeforeKick')
        elif transitions.shouldPositionForKickFromApproachLoc(player):
            return player.goLater('positionForKick')
        elif player.ballInMyGoalBox():
            return player.goLater('ballInMyBox')
        elif transitions.shouldChaseAroundBox(player):
            return player.goLater('chaseAroundBox')
        elif transitions.shouldAvoidObstacleDuringApproachBall(player):
            return player.goLater('avoidObstacle')
        elif my.locScoreFramesBad > constants.APPROACH_NO_LOC_THRESH:
            return player.goLater('approachBall')
        elif not player.brain.tracker.activeLocOn and \
                transitions.shouldScanFindBall(player):
            return player.goLater('scanFindBall')
        elif player.brain.tracker.activeLocOn and \
                transitions.shouldScanFindBallActiveLoc(player):
            return player.goLater('scanFindBall')

    if player.brain.ball.locDist > constants.APPROACH_ACTIVE_LOC_DIST:
        player.brain.tracker.activeLoc()
    else :
        player.brain.tracker.trackBall()

    dest = player.getApproachPosition()
    useOmni = MyMath.dist(my.x, my.y, dest[0], dest[1]) <= \
        constants.APPROACH_OMNI_DIST
    changedOmni = False

    if useOmni != nav.movingOmni:
        player.changeOmniGoToCounter += 1
    else :
        player.changeOmniGoToCounter = 0
    if player.changeOmniGoToCounter > PositionConstants.CHANGE_OMNI_THRESH:
        changedOmni = True

    if player.firstFrame() or \
            nav.destX != dest[0] or \
            nav.destY != dest[1] or \
            nav.destH != dest[2] or \
            changedOmni:
        if not useOmni:
            nav.goTo(dest)
        else:
            nav.omniGoTo(dest)

    return player.stay()


def approachBall(player):
    """
    Once we are alligned with the ball, approach it
    """
    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.brain.CoA.setRobotGait(player.brain.motion)

    if player.penaltyKicking and \
            player.ballInOppGoalBox():
        player.stopWalking()
        return player.stay()

    # Switch to other states if we should
    if player.currentRole == pbc.GOALIE:
        if transitions.shouldKick(player):
            return player.goNow('waitBeforeKick')
        elif transitions.shouldPositionForKick(player):
            return player.goNow('positionForKick')
        elif transitions.shouldTurnToBall_ApproachBall(player):
            return player.goLater('turnToBall')
        elif transitions.shouldScanFindBall(player):
            return player.goLater('goalieScanFindBall')
        elif transitions.shouldAvoidObstacle(player):
            return player.goLater('avoidObstacle')
    else:
        if transitions.shouldDribble(player):
            return player.goNow('dribble')
        elif transitions.shouldKick(player):
            return player.goNow('waitBeforeKick')
        elif transitions.shouldPositionForKick(player):
            return player.goNow('positionForKick')

    return approachBallWalk(player)

def approachBallWalk(player):
    """
    Method that is used by both approach ball and dribble
    We use things as to when we should leave and how we should walk
    """
    if player.currentRole != pbc.GOALIE:
        if player.ballInMyGoalBox():
            return player.goLater('ballInMyBox')
        elif transitions.shouldChaseAroundBox(player):
            return player.goLater('chaseAroundBox')
        elif transitions.shouldApproachBallWithLoc(player):
            return player.goNow('approachBallWithLoc')
        elif transitions.shouldTurnToBall_ApproachBall(player):
            return player.goLater('turnToBall')
        elif transitions.shouldScanFindBall(player):
            return player.goLater('scanFindBall')
        elif transitions.shouldAvoidObstacle(player):
            return player.goLater('avoidObstacle')

    # Determine our speed for approaching the ball
    ball = player.brain.ball
    if player.currentRole == pbc.GOALIE and goalTran.dangerousBall(player):
        return player.goNow('approachDangerousBall')

    sX = MyMath.clip(ball.dist*constants.APPROACH_X_GAIN,
                     constants.MIN_APPROACH_X_SPEED,
                     constants.MAX_APPROACH_X_SPEED)

    # Determine the speed to turn to the ball
    sTheta = MyMath.clip(ball.bearing*constants.APPROACH_SPIN_GAIN,
                         -constants.APPROACH_SPIN_SPEED,
                         constants.APPROACH_SPIN_SPEED)
    # Avoid spinning so slowly that we step in place
    if fabs(sTheta) < constants.MIN_APPROACH_SPIN_MAGNITUDE:
        sTheta = 0.0

    # Set our walk towards the ball
    if ball.on:
        player.setSpeed(sX,0,sTheta)

    return player.stay()

def positionForKick(player):
    """
    State to align on the ball once we are near it
    """
    if player.firstFrame():
        player.brain.CoA.setRobotGait(player.brain.motion)

    ball = player.brain.ball
    player.inKickingState = True
    # Leave this state if necessary
    if transitions.shouldKick(player):
        return player.goNow('waitBeforeKick')
    elif transitions.shouldScanFindBall(player):
        player.inKickingState = False
        return player.goLater('scanFindBall')
    elif transitions.shouldTurnToBallFromPositionForKick(player):
        player.inKickingState = False
        return player.goLater('turnToBall')
    elif transitions.shouldApproachFromPositionForKick(player):
        player.inKickingState = False
        return player.goLater('approachBall')

    # Determine approach speed
    targetY = ball.relY

    sY = MyMath.clip(targetY * constants.PFK_Y_GAIN,
                     constants.PFK_MIN_Y_SPEED,
                     constants.PFK_MAX_Y_SPEED)
    if fabs(sY) < constants.PFK_MIN_Y_MAGNITUDE:
        sY = 0.0

    if transitions.shouldApproachForKick(player):
        targetX = (ball.relX -
                   (constants.BALL_KICK_LEFT_X_CLOSE +
                    constants.BALL_KICK_LEFT_X_FAR) / 2.0)
        sX = MyMath.clip(ball.relX * constants.PFK_X_GAIN,
                         constants.PFK_MIN_X_SPEED,
                         constants.PFK_MAX_X_SPEED)
    else:
        sX = 0.0

    if ball.on:
        player.setSpeed(sX,sY,0)
    return player.stay()

def dribble(player):
    """
    Keep running at the ball, but dribble
    """
    if player.firstFrame():
        player.brain.motion.setGait(RobotGaits.DUCK_GAIT)

    # if we should stop dribbling, see what else we should do
    if transitions.shouldStopDribbling(player):

        if transitions.shouldKick(player):
            return player.goNow('waitBeforeKick')
        elif transitions.shouldPositionForKick(player):
            return player.goNow('positionForKick')
        elif transitions.shouldApproachBall(player):
            return player.goNow('approachBall')

    return approachBallWalk(player)

def waitBeforeKick(player):
    """
    Stop before we kick to make sure we want to kick
    """
    player.inKickingState = True
    if player.firstFrame():
        player.brain.CoA.setRobotGait(player.brain.motion)
        player.stopWalking()

    if not player.brain.nav.isStopped():
        return player.stay()
    elif transitions.shouldApproachForKick(player):
        player.brain.tracker.trackBall()
        player.inKickingState = False
        return player.goLater('approachBall')
    elif transitions.shouldScanFindBall(player):
        player.inKickingState = False
        player.brain.tracker.trackBall()
        return player.goLater('scanFindBall')
    elif transitions.shouldRepositionForKick(player):
        player.brain.tracker.trackBall()
        return player.goLater('positionForKick')
    else:
        return player.goLater('getKickInfo')

def avoidObstacle(player):
    """
    If we detect something in front of us, dodge it
    """
    if player.firstFrame():
        player.doneAvoidingCounter = 0
        player.printf(player.brain.sonar)

        player.brain.CoA.setRobotGait(player.brain.motion)

        if (transitions.shouldAvoidObstacleLeft(player) and
            transitions.shouldAvoidObstacleRight(player)):
            # Backup
            player.printf("Avoid by backup");
            player.setSpeed(constants.DODGE_BACK_SPEED, 0, 0)

        elif transitions.shouldAvoidObstacleLeft(player):
            # Dodge right
            player.printf("Avoid by right dodge");
            player.setSpeed(0, constants.DODGE_RIGHT_SPEED, 0)

        elif transitions.shouldAvoidObstacleRight(player):
            # Dodge left
            player.printf("Avoid by left dodge");
            player.setSpeed(0, constants.DODGE_LEFT_SPEED, 0)

    if not transitions.shouldAvoidObstacle(player):
        player.doneAvoidingCounter += 1
    else :
        player.doneAvoidingCounter -= 1
        player.doneAvoidingCounter = max(0, player.doneAvoidingCounter)

    if player.doneAvoidingCounter > constants.DONE_AVOIDING_FRAMES_THRESH:
        player.shouldAvoidObstacleRight = 0
        player.shouldAvoidObstacleLeft = 0
        player.stopWalking()
        return player.goLater(player.lastDiffState)

    return player.stay()

def chaseAroundBox(player):
    if player.firstFrame():
        player.shouldNotChaseAroundBox = 0

        player.brain.CoA.setRobotGait(player.brain.motion)

    if transitions.shouldKick(player):
        return player.goNow('waitBeforeKick')
    elif transitions.shouldScanFindBall(player):
        return player.goLater('scanFindBall')
    elif transitions.shouldAvoidObstacle(player):
        return player.goLater('avoidObstacle')

    if not transitions.shouldChaseAroundBox(player):
        player.shouldChaseAroundBox += 1
    else :
        player.shouldChaseAroundBox = 0
    if player.shouldChaseAroundBox > constants.STOP_CHASING_AROUND_BOX:
        return player.goLater('chase')

    ball = player.brain.ball
    my = player.brain.my
    if my.x > NogginConstants.MY_GOALBOX_RIGHT_X:
        # go to corner nearest ball
        if ball.y > NogginConstants.MY_GOALBOX_TOP_Y:
            player.brain.nav.goTo( (NogginConstants.MY_GOALBOX_RIGHT_X +
                                    constants.GOALBOX_OFFSET,
                                    NogginConstants.MY_GOALBOX_TOP_Y +
                                    constants.GOALBOX_OFFSET,
                                    NogginConstants.MY_GOAL_HEADING ))

        if ball.y < NogginConstants.MY_GOALBOX_BOTTOM_Y:
            player.brain.nav.goTo(( NogginConstants.MY_GOALBOX_RIGHT_X +
                                    constants.GOALBOX_OFFSET,
                                    NogginConstants.MY_GOALBOX_BOTTOM_Y -
                                    constants.GOALBOX_OFFSET,
                                    NogginConstants.MY_GOAL_HEADING ))

    if my.x < NogginConstants.MY_GOALBOX_RIGHT_X:
        # go to corner nearest ball
        if my.y > NogginConstants.MY_GOALBOX_TOP_Y:
            player.brain.nav.goTo(( NogginConstants.MY_GOALBOX_RIGHT_X +
                                    constants.GOALBOX_OFFSET,
                                    NogginConstants.MY_GOALBOX_TOP_Y +
                                    constants.GOALBOX_OFFSET,
                                    NogginConstants.MY_GOAL_HEADING ))

        if my.y < NogginConstants.MY_GOALBOX_BOTTOM_Y:
            player.brain.nav.goTo(( NogginConstants.MY_GOALBOX_RIGHT_X +
                                    constants.GOALBOX_OFFSET,
                                    NogginConstants.MY_GOALBOX_BOTTOM_Y -
                                    constants.GOALBOX_OFFSET,
                                    NogginConstants.MY_GOAL_HEADING ))
    return player.stay()


def orbitBall(player):
    """
    Method to spin around the ball before kicking
    """
    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.brain.nav.orbitAngle(player.orbitAngle)

        player.brain.CoA.setRobotGait(player.brain.motion)


    if player.brain.nav.isStopped():
        return player.goLater('getKickInfo')

    return player.stay()

def steps(player):
    if player.brain.nav.isStopped():
        player.setSteps(3,3,0,5)
    elif player.brain.nav.currentState != "stepping":
        player.stopWalking()
    return player.stay()

def ballInMyBox(player):
    if player.firstFrame():
        player.brain.tracker.activeLoc()
        player.stopWalking()

        player.brain.CoA.setRobotGait(player.brain.motion)

    if not player.ballInMyGoalBox():
        return player.goLater('chase')
    return player.stay()

def approachDangerousBall(player):
    if player.firstFrame():
        player.stopWalking()
    print "approach dangerous ball"
    my = player.brain.my
    if -170 < my.h < -90: #ball to the right
        player.setSteps(2,3,1,1)
    elif 90 < my.h < 170:
        player.setSteps(2,-3,-1,1)
    else:
        player.setSteps(1, -1, 1, 1)

    if not goalTran.dangerousBall(player):
        return player.goLater('approachBall')
    #single steps towards ball and goal with spin
    #still check for kicking

    if transitions.shouldScanFindBall(player):
        return player.goLater('goalieScanFindBall')
    elif transitions.shouldKick(player):
        return player.goLater('waitBeforeKick')
    elif transitions.shouldTurnToBall_ApproachBall(player):
        return player.goLater('turnToBall')
    elif transitions.shouldSpinFindBall(player):
        return player.goLater('spinFindBall')

    return player.stay()
