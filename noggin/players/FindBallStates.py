import ChaseBallConstants as constants
import ChaseBallTransitions as transitions
from ..playbook import PBConstants as pbc
from ..util import MyMath

def scanFindBall(player):
    """
    State to move the head to find the ball. If we find the ball, we
    move to align on it. If we don't find it, we spin to keep looking
    """
    if player.firstFrame():
        player.stopWalking()
        player.brain.tracker.trackBall()

    if player.currentRole == pbc.GOALIE:
        if transitions.shouldTurnToBall_FoundBall(player):
            return player.goLater('turnToBall')
        elif transitions.shouldSpinFindBall(player):
            return player.goLater('spinFindBall')
    else:
        if transitions.shouldApproachBallWithLoc(player):
            player.brain.tracker.trackBall()
            return player.goLater('approachBallWithLoc')
        elif transitions.shouldTurnToBall_FoundBall(player):
            return player.goLater('turnToBall')
        elif transitions.shouldSpinFindBall(player):
            return player.goLater('spinFindBall')

    return player.stay()

def spinFindBall(player):
    """
    State to spin to find the ball. If we find the ball, we
    move to align on it. If we don't find it, we go to a garbage state
    """

    if player.currentRole == pbc.GOALIE:
        if transitions.shouldTurnToBall_FoundBall(player):
            return player.goLater('turnToBall')
    else:
        if transitions.shouldApproachBallWithLoc(player):
            player.brain.tracker.trackBall()
            return player.goLater('approachBallWithLoc')
        elif transitions.shouldTurnToBall_FoundBall(player):
            return player.goLater('turnToBall')
        elif transitions.shouldWalkToBallLocPos(player):
            return player.goLater('walkToBallLocPos')
    if player.firstFrame():
        player.brain.tracker.trackBall()

        if player.justKicked:
            spinDir = player.getSpinDirAfterKick()
        else:
            my = player.brain.my
            ball = player.brain.ball
            bearingToBall = MyMath.getRelativeBearing(my.x, my.y, my.h,
                                                      ball.x,
                                                      ball.y )
            spinDir = MyMath.getSpinDir(my.h,
                                        my.h + bearingToBall)

        player.setSpeed(0, 0, spinDir*constants.FIND_BALL_SPIN_SPEED)

    return player.stay()

def walkToBallLocPos(player):
    if player.firstFrame():
        player.brain.tracker.trackBall()
    if transitions.shouldApproachBallWithLoc(player):
        player.brain.tracker.trackBall()
        return player.goLater('approachBallWithLoc')
    elif transitions.shouldTurnToBall_FoundBall(player):
        return player.goLater('turnToBall')

    ball = player.brain.ball
    destH = MyMath.getTargetHeading(player.brain.my, ball.x, ball.y)
    dest = (ball.x, ball.y, destH)

    nav = player.brain.nav
    if player.firstFrame() or \
            nav.destX != dest[0] or \
            nav.destY != dest[1] or \
            nav.destH != dest[2]:
        nav.goTo(dest)
    return player.stay()
