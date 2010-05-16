import ChaseBallConstants as constants
import ChaseBallTransitions as transitions
from ..playbook.PBConstants import GOALIE
from man.noggin.typeDefs.Location import RobotLocation

def scanFindBall(player):
    """
    State to move the head to find the ball. If we find the ball, we
    move to align on it. If we don't find it, we spin to keep looking
    """
    if player.firstFrame():
        player.stopWalking()
        player.brain.tracker.trackBall()

    if player.brain.play.isRole(GOALIE):
        if transitions.shouldSpinFindBall(player):
            return player.goLater('spinFindBall')
    else:
        if transitions.shouldApproachBallWithLoc(player):
            return player.goLater('approachBall')
        elif transitions.shouldSpinFindBall(player):
            return player.goLater('spinFindBall')

    if abs(player.brain.ball.bearing) < constants.SCAN_FIND_BEARING_THRESH \
            or player.brain.ball.dist < constants.SCAN_FIND_DIST_THRESH \
            and not player.brain.play.isRole(GOALIE):
        return player.stay()
    elif player.brain.play.isRole(GOALIE) and \
            abs(player.brain.ball.bearing) <\
            constants.SCAN_FIND_BEARING_THRESH:
        return player.stay()
    elif player.firstFrame():
        return player.goLater('spinFindBall')
    return player.stay()

def spinFindBall(player):
    """
    State to spin to find the ball. If we find the ball, we
    move to align on it. If we don't find it, we go to a garbage state
    """

    if player.brain.play.isRole(GOALIE):
        if transitions.shouldApproachBall(player):
            return player.goLater('approachBall')
    else:
        if transitions.shouldApproachBallWithLoc(player):
            return player.goLater('approachBall')
        elif transitions.shouldApproachBall(player):
            return player.goLater('approachBall')
        elif transitions.shouldWalkToBallLocPos(player):
            return player.goLater('approachBall')

    if player.firstFrame():
        player.brain.tracker.trackBall()

        if player.justKicked:
            spinDir = player.getSpinDirAfterKick()
        else:
            my = player.brain.my
            ball = player.brain.ball
            spinDir = my.spinDirToHeading(my.h + ball.bearing)

        player.setWalk(0, 0, spinDir*constants.FIND_BALL_SPIN_SPEED)

    return player.stay()
