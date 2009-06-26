import ChaseBallConstants as constants
import ChaseBallTransitions as transitions
import GoalieTransitions as goalTrans
import GoalieConstants as goalCon
from ..playbook import PBConstants as pbc
import man.motion.HeadMoves as HeadMoves
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

def goalieScanFindBall(player):
    ball = player.brain.ball
    head = player.brain.tracker

    if player.firstFrame():
        time = goalTrans.getTimeUntilSave(player)
        #timeUntilSave gives us an idea of if the ball was coming at us
        if time != -1 and time < 10:
            turnDir = goalTrans.strafeDirForSave(player)
            if turnDir == 1:
                head.lookToDir('leftDown')
            if turnDir == -1:
                head.lookToDir('rightDown')
            return player.goLater('goalieSpinFindBall')
        else:
            head.trackBall()
    if transitions.shouldTurnToBall_FoundBall(player):
        return player.goLater('turnToBall')
    elif transitions.shouldSpinFindBall(player):
        return player.goLater('spinFindBall')

    return player.stay()

def goalieSpinFindBall(player):
    ball = player.brain.ball
    if player.firstFrame():
        spinDir = goalTrans.strafeDirForSave(player)
        #spinDir = MyMath.getSpinDir(ball.locBearing)
        player.setSpeed(0, 0, spinDir*constants.FIND_BALL_SPIN_SPEED)
    if not player.brain.motion.isHeadActive():
        player.brain.tracker.trackBall()
    if transitions.shouldTurnToBall_FoundBall(player):
        return player.goLater('turnToBall')
    return player.stay()
