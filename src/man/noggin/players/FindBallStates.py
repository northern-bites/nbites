import ChaseBallConstants as constants
import ChaseBallTransitions as transitions
from ..playbook.PBConstants import GOALIE
from math import fabs

def scanFindBall(player):
    """
    State to move the head to find the ball. If we find the ball, we
    move to align on it. If we don't find it, we spin to keep looking
    """
    if player.firstFrame():
        player.stopWalking()
        player.brain.tracker.trackBall()

    if transitions.shouldApproachBall(player):
        return player.goNow('approachBall')

    # a time based check. may be a problem for goalie. if it's not good for him to
    # spin, he should prbly not be chaser anymore, so this wouldn't get reached
    if transitions.shouldSpinFindBall(player):
        return player.goNow('spinFindBall')

    ball = player.brain.ball
    if fabs(ball.bearing) < constants.SCAN_FIND_BEARING_THRESH:
        return player.stay()

    else:
        return player.goLater('spinFindBall')

    return player.stay()

def spinFindBall(player):
    """
    State to spin to find the ball. If we find the ball, we
    move to align on it. If we don't find it, we go to a garbage state
    """

    if transitions.shouldApproachBall(player):
        return player.goNow('approachBall')

    if player.firstFrame():
        #if ball.on
        #player.brain.tracker.trackBallSpin()
        #else:
        #tracker.findBall()

        my = player.brain.my
        ball = player.brain.ball
        spinDir = my.spinDirToPoint(ball)

        player.setWalk(0, 0, spinDir*constants.FIND_BALL_SPIN_SPEED)

        player.brain.tracker.trackBallSpin()

    if not player.brain.play.isRole(GOALIE):
        if transitions.shouldWalkToBallLocPos(player):
            return player.goLater('approachBall')

    return player.stay()
