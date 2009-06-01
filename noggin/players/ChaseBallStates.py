"""
Here we house all of the state methods used for chasing the ball
"""

import man.noggin.util.MyMath as MyMath
import ChaseBallConstants as constants
import ChaseBallTransitions as transitions
from math import fabs

def chase(player):
    """
    Method to determine which chase state should be used.
    We dump the robot into this state when we our switching from something else.
    """
    if transitions.shouldScanFindBall(player):
        return player.goNow('scanFindBall')
    elif transitions.shouldApproachBall(player):
        return player.goNow('approachBall')
    elif transitions.shouldTurnToBall_ApproachBall(player):
        return player.goNow('turnToBall')
    elif transitions.shouldSpinFindBall(player):
        return player.goNow('spinFindBall')
    else:
        return player.goNow('scanFindBall')

def turnToBall(player):
    """
    Rotate to align with the ball. When we get close, we will approach it
    """
    ball = player.brain.ball

    if player.firstFrame():
        player.brain.tracker.trackBall()
        # Stop walking if we need to switch gaits
        if player.currentGait != constants.FAST_GAIT:
            player.stopWalking()
            player.stoppedWalk = False
        else:
            player.stoppedWalk = True

    # Determine the speed to turn to the ball
    turnRate = MyMath.clip(ball.bearing*constants.BALL_SPIN_GAIN,
                           -constants.BALL_SPIN_SPEED,
                           constants.BALL_SPIN_SPEED)

    # Avoid spinning so slowly that we step in place
    if fabs(turnRate) < constants.MIN_BALL_SPIN_SPEED:
        turnRate = MyMath.sign(turnRate)*constants.MIN_BALL_SPIN_SPEED

    # Determine that we have stopped walking
    if player.brain.nav.isStopped():
        player.stoppedWalk = True

    if transitions.shouldPositionForKick(player):
        return player.goLater('positionForKick')
    elif transitions.shouldApproachBall(player):
        return player.goLater('approachBall')
    elif transitions.shouldScanFindBall(player):
        return player.goLater('scanFindBall')
    elif ball.on and player.stoppedWalk:
        # Switch gaits if necessary
        if (player.brain.nav.isStopped() and
            player.currentGait != constants.FAST_GAIT):
            player.brain.CoA.setRobotGait(player.brain.motion)
            player.currentGait = constants.FAST_GAIT

        player.setSpeed(x=0,y=0,theta=turnRate)

    return player.stay()

def approachBall(player):
    """
    Once we are alligned with the ball, approach it
    """
    if player.firstFrame():
        if player.currentGait != constants.FAST_GAIT:
            player.stopWalking()
            player.stoppedWalk = False
        else:
            player.stoppedWalk = True

    # Determine if we have stopped walking
    if player.brain.nav.isStopped():
        player.stoppedWalk = True

    # Switch to other states if we should
    if transitions.shouldPositionForKick(player):
        return player.goLater('positionForKick')
    elif transitions.shouldTurnToBall_ApproachBall(player):
        return player.goLater('turnToBall')
    elif transitions.shouldScanFindBall(player):
        return player.goLater('scanFindBall')
    elif transitions.shouldAvoidObstacle(player):
        return player.goLater('avoidObstacle')

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

    # Don't turn right now
    sTheta = 0.0

    # Set our walk towards the ball
    if ball.on and player.stoppedWalk:
        if (player.brain.nav.isStopped() and
            player.currentGait != constants.FAST_GAIT):
            player.brain.CoA.setRobotGait(player.brain.motion)
            player.currentGait = constants.FAST_GAIT
        player.setSpeed(sX,0,sTheta)

    return player.stay()

def positionForKick(player):
    """
    State to align on the ball once we are near it
    Currently aligns the ball on the left foot
    """
    ball = player.brain.ball

    if player.firstFrame():
        # Stop if we need to switch gaits
        if player.currentGait != constants.NORMAL_GAIT:
            player.stoppedWalk = False
            player.stopWalking()
        else:
            player.stoppedWalk = True

    # Leave this state if necessary
    if transitions.shouldKick(player):
        return player.goLater('waitBeforeKick')
    elif transitions.shouldScanFindBall(player):
        return player.goLater('scanFindBall')
    elif transitions.shouldTurnToBall_ApproachBall(player):
        return player.goLater('turnToBall')
    elif transitions.shouldApproachFromPositionOnKick(player):
        return player.goLater('approachBall')

    # Determine approach speed
    targetY = (ball.locRelY -
               (constants.BALL_KICK_LEFT_Y_L + constants.BALL_KICK_LEFT_Y_R) / 2.0 )
    sY = MyMath.clip(targetY,
                     constants.PFK_MIN_Y_SPEED,
                     constants.PFK_MAX_Y_SPEED)
    if fabs(sY) < constants.PFK_MIN_Y_MAGNITUDE:
        sY = 0.0

    if transitions.shouldApproachForKick(player):
        targetX = (ball.locRelX -
                   (constants.BALL_KICK_LEFT_X_CLOSE +
                    constants.BALL_KICK_LEFT_X_FAR) / 2.0)
        sX = MyMath.clip(ball.locRelX,
                         constants.PFK_MIN_X_SPEED,
                         constants.PFK_MAX_X_SPEED)
    else:
        sX = 0.0

#     player.printf("\tPosition for kick target_y is " +
#                   str(targetY), "cyan")
#     player.printf("\tPosition for kick sY is " + str(sY), "cyan")
#     player.printf("\tPosition for kick sX is " + str(sX), "cyan")
#     player.printf("\tBall dist is: " + str(ball.dist), "cyan")
#     player.printf("\tBall current y is: " +
#                   str(ball.locRelY) + " want between " +
#                   str(constants.BALL_KICK_LEFT_Y_L) +
#                   " and " +
#                   str(constants.BALL_KICK_LEFT_Y_R), "cyan")
#     player.printf("\tBall current x is: " +
#                   str(ball.locRelX) + " want between " +
#                   str(constants.BALL_KICK_LEFT_X_CLOSE) +
#                   " and " + str(constants.BALL_KICK_LEFT_X_FAR), "cyan")

    # Determine if we have stopped
    if player.brain.nav.isStopped():
        player.stoppedWalk = True

    # Walk if we have stopped or have the correct gait already
    if ball.on and player.stoppedWalk:
        # Set the correct gait, to make us walk better
        if (player.brain.nav.isStopped() and
            player.currentGait != constants.NORMAL_GAIT) :
            player.brain.CoA.setRobotTurnGait(player.brain.motion)
            player.currentGait = constants.NORMAL_GAIT
        player.setSpeed(0,sY,sX)

    return player.stay()

def waitBeforeKick(player):
    """
    Stop before we kick to make sure we want to kick
    """
    if player.firstFrame():
        player.stopWalking()

    if not player.brain.nav.isStopped():
        return player.stay()
    elif transitions.shouldApproachForKick(player):
        player.brain.tracker.trackBall()
        return player.goLater('approachBall')
    elif transitions.shouldScanFindBall(player):
        player.brain.tracker.trackBall()
        return player.goLater('scanFindBall')
    elif transitions.shouldRepositionForKick(player):
        player.brain.tracker.trackBall()
        return player.goLater('positionForKick')
    else:
        return player.goLater('decideKick')

def ignoreOwnGoal(player):
    """
    Method to intelligently ignore kicking into our own goal
    """
    if transitions.shouldSpinFindBall(player):
        return player.goNow('spinFindBall')

    return player.stay()

def avoidObstacle(player):
    """
    If we detect something in front of us, dodge it
    """
    if player.firstFrame():
        player.stopWalking()
        player.stoppedWalk = False
        player.printf(player.brain.sonar)

    # Check if we've stopped
    if player.brain.nav.isStopped():
        player.stoppedWalk = True
        if (player.brain.nav.isStopped() and
            player.currentGait != constants.FAST_GAIT):
            player.brain.CoA.setRobotGait(player.brain.motion)
            player.currentGait = constants.FAST_GAIT

    if (transitions.shouldAvoidObstacleLeft(player) and
        transitions.shouldAvoidObstacleRight(player)):
        # Backup
        if player.stoppedWalk:
            player.setSpeed(constants.DODGE_BACK_SPEED, 0, 0)
    elif transitions.shouldAvoidObstacleLeft(player):
        # Dodge right
        if player.stoppedWalk:
            player.setSpeed(0, constants.DODGE_RIGHT_SPEED, 0)
    elif transitions.shouldAvoidObstacleRight(player):
        # Dodge left
        if player.stoppedWalk:
            player.setSpeed(0, constants.DODGE_LEFT_SPEED, 0)
    else:
        return player.goLater("chase")
    return player.stay()

def steps(player):
    if player.brain.nav.isStopped():
        player.setSteps(3,3,0,5)
    elif player.brain.nav.currentState != "stepping":
        player.stopWalking()
    return player.stay()
