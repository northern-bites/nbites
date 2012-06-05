import ChaseBallConstants as constants
import ChaseBallTransitions as transitions
from ..playbook.PBConstants import GOALIE


def findBall(player):
    """
    State to stop all activity and begin finding the ball
    """
    if transitions.shouldChaseBall(player):
        if not player.brain.play.isChaser():
            return player.goLater('playbookPosition')
        else:
            return player.goLater('chase')

    if player.firstFrame():
        player.stopWalking()
        player.brain.tracker.stopHeadMoves()

    if player.brain.nav.isStopped():
        return player.goLater('scanFindBall')

    return player.stay()


def scanFindBall(player):
    """
    State to move the head to find the ball. If we find the ball, we
    mppove to align on it. If we don't find it, we spin to keep looking
    """
    player.brain.tracker.trackBallFixedPitch()

    if transitions.shouldChaseBall(player):
        return player.goNow('findBall')

    # a time based check. may be a problem for goalie. if it's not
    # good for him to spin, he should prbly not be chaser anymore, so
    # this wouldn't get reached
    if transitions.shouldSpinFindBall(player):
        return player.goLater('spinFindBall')

    return player.stay()

def spinFindBall(player):
    """
    State to spin to find the ball. If we find the ball, we
    move to align on it. If we don't find it, we walk to look for it
    """
    if transitions.shouldChaseBall(player):
        player.stopWalking()
        player.brain.tracker.trackBallFixedPitch()
        return player.goNow('findBall')

    if player.firstFrame():
        player.brain.tracker.stopHeadMoves()

    if player.brain.nav.isStopped() and player.brain.tracker.isStopped():
        my = player.brain.my
        ball = player.brain.ball
        spinDir = my.spinDirToPoint(ball.loc)
        player.setWalk(0, 0, spinDir*constants.FIND_BALL_SPIN_SPEED)

        # TODO: fix track ball spin for fixed pitch @summer 2012
        player.brain.tracker.trackBallSpin()

    if not player.brain.play.isRole(GOALIE):
        if transitions.shouldWalkFindBall(player):
            return player.goLater('walkFindBall')

    return player.stay()

def walkFindBall(player):
    """
    State to walk to find the ball. If we find the ball we chase it.
    """
    if player.firstFrame():
        player.stopWalking()
        # Do a slow pan
        player.brain.tracker.repeatHeadMove(player.brain.tracker.FIXED_PITCH_PAN_WIDE)

    if transitions.shouldChaseBall(player):
        player.stopWalking()
        player.brain.tracker.trackBallFixedPitch()
        return player.goNow('findBall')

    if player.brain.nav.isStopped():
        player.brain.nav.chaseBall()

    if transitions.shouldSpinFindBallAgain(player):
        return player.goLater('spinFindBall')

    return player.stay()
