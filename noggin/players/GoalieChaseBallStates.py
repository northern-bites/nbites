import GoalieTransitions as helper
import ChaseBallTransitions as transitions
import ChaseBallConstants as constants
import PositionConstants
from ..util import MyMath
from math import fabs

def goalieChase(player):
    if player.firstFrame():
        player.shouldChaseCounter = 0
        player.brain.tracker.trackBall()
        player.isChasing = True

    if transitions.shouldScanFindBall(player):
        return player.goNow('goalScanFindBall')
    elif transitions.shouldApproachBallWithLoc(player):
        return player.goNow('goalApproachBallWithLoc')
    elif transitions.shouldApproachBall(player):
        return player.goNow('goalApproachBall')
    elif transitions.shouldTurnToBall_ApproachBall(player):
        return player.goNow('goalTurnToBall')
    elif transitions.shouldSpinFindBall(player):
        return player.goNow('goalSpinFindBall')
    else:
        return player.goNow('goalScanFindBall')

def goalScanFindBall(player):

    if player.firstFrame():
        player.stopWalking()
        player.brain.tracker.trackBall()
    if transitions.shouldApproachBallWithLoc(player):
        player.brain.tracker.trackBall()
        return player.goLater('goalApproachBallWithLoc')
    elif transitions.shouldTurnToBall_FoundBall(player):
        return player.goLater('goalTurnToBall')
    elif transitions.shouldSpinFindBall(player):
        return player.goLater('goalSpinFindBall')
    return player.stay()

def goalSpinFindBall(player):

    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.setSpeed(0,0,10)

    # Determine if we should leave this state
    if transitions.shouldApproachBallWithLoc(player):
        player.brain.tracker.trackBall()
        return player.goLater('goalApproachBallWithLoc')
    elif transitions.shouldTurnToBall_FoundBall(player):
        return player.goLater('goalTurnToBall')

    return player.stay()

def goalApproachBallWithLoc(player):
    nav = player.brain.nav
    my = player.brain.my

    if my.locScoreFramesBad > constants.APPROACH_NO_LOC_THRESH:
        return player.goLater('goalApproachBall')
    elif not player.brain.tracker.activeLocOn and \
            transitions.shouldScanFindBall(player):
        return player.goLater('goalScanFindBall')

    if player.brain.ball.locDist > constants.APPROACH_ACTIVE_LOC_DIST:
        player.brain.tracker.activeLoc()
    else :
        player.brain.tracker.trackBall()

    dest = player.getApproachPosition()
    if player.firstFrame() or \
            nav.destX != dest[0] or \
            nav.destY != dest[1] or \
            nav.destH != dest[2]:
        nav.omniGoTo(dest)

    return player.stay()

def goalTurnToBall(player):
    """
    Rotate to align with the ball. When we get close, we will approach it
    """
    ball = player.brain.ball

    if player.firstFrame():
        player.brain.tracker.trackBall()

    # Determine the speed to turn to the ball
    turnRate = MyMath.clip(ball.bearing*constants.BALL_SPIN_GAIN,
                           -constants.BALL_SPIN_SPEED,
                           constants.BALL_SPIN_SPEED)

    # Avoid spinning so slowly that we step in place
    if fabs(turnRate) < constants.MIN_BALL_SPIN_SPEED:
        turnRate = MyMath.sign(turnRate)*constants.MIN_BALL_SPIN_SPEED

    if transitions.shouldApproachBall(player):
        return player.goLater('goalApproachBall')
    elif transitions.shouldScanFindBall(player):
        return player.goLater('goalScanFindBall')

    elif ball.on:
        player.setSpeed(x=0,y=0,theta=turnRate)

    return player.stay()

def goalApproachBall(player):
    """
    Once we are aligned with the ball, approach it
    """
    if player.firstFrame():
        player.brain.tracker.trackBall()

    if player.penaltyKicking and \
            not player.ballInMyGoalBox():
        player.stopWalking()
        return player.stay()

    elif transitions.shouldApproachBallWithLoc(player):
        return player.goNow('goalApproachBallWithLoc')
    elif transitions.shouldTurnToBall_ApproachBall(player):
        return player.goLater('goalTurnToBall')
    elif transitions.shouldScanFindBall(player):
        return player.goLater('goalScanFindBall')

    # Determine our speed for approaching the ball
    ball = player.brain.ball
    sX = MyMath.clip(ball.dist*constants.APPROACH_X_GAIN,
                     constants.MIN_APPROACH_X_SPEED,
                     constants.MAX_APPROACH_X_SPEED)

    # Determine the speed to turn to the ball
    sTheta = MyMath.clip(ball.bearing*constants.APPROACH_SPIN_GAIN,
                         -constants.APPROACH_SPIN_SPEED,
                         constants.APPROACH_SPIN_SPEED)
    # Avoid spinning so slowly that we step in place
    if fabs(sTheta) < constants.MIN_APPROACH_SPIN_SPEED:
        sTheta = 0.0

    # Set our walk towards the ball
    if ball.on:
        player.setSpeed(constants.MAX_APPROACH_X_SPEED,0,sTheta)

    return player.stay()
