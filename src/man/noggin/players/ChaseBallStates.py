"""
Here we house all of the state methods used for chasing the ball
"""
import ChaseBallTransitions as transitions
import ChaseBallConstants as constants
import GoalieTransitions as goalTran
from ..playbook.PBConstants import GOALIE
from .. import NogginConstants as nogginConstants
from man.noggin.typeDefs.Location import RobotLocation
from man.noggin.kickDecider import KickInformation

def chase(player):
    """
    Super State to determine what to do from various situations
    """
    player.isChasing = True
    player.hasAlignedOnce = False

    if player.brain.play.isRole(GOALIE):
        return player.goNow('goalieChase')

    # Check in order of importance
    if transitions.shouldScanFindBall(player):
        return player.goNow('scanFindBall')
    elif transitions.shouldStopAndKick(player):
        return player.goNow('stopBeforeKick')
    elif transitions.shouldPositionForKick(player):
        return player.goNow('decideKick')
    elif transitions.shouldApproachBall(player):
        return player.goNow('approachBall')
    else:
        return player.goNow('scanFindBall')

def goalieChase(player):
    """
    TODO: make goalie more aggressive (different transitions?)
    """
    # Check in order of importance
    if transitions.shouldScanFindBall(player):
        return player.goNow('scanFindBall')
    elif transitions.shouldSpinToBallClose(player):
        return player.goNow('spinToBallClose')
    elif transitions.shouldStopAndKick(player):
        return player.goNow('stopBeforeKick')
    elif transitions.shouldPositionForKick(player):
        return player.goNow('decideKick')
    elif transitions.shouldApproachBall(player):
        return player.goNow('approachBall')
    else:
        return player.goNow('scanFindBall')

def approachBall(player):
    """
    Once we are aligned with the ball, approach it
    """
    # Switch to other states if we should
    if player.penaltyKicking and \
           player.brain.ball.inOppGoalBox():
        return player.goNow('penaltyBallInOppGoalbox')

    elif player.brain.tracker.activeLocOn:
        if transitions.shouldScanFindBallActiveLoc(player):
            return player.goLater('scanFindBall')

    elif transitions.shouldScanFindBall(player):
        return player.goLater('scanFindBall')

    elif player.brain.play.isRole(GOALIE) and goalTran.dangerousBall(player):
        return player.goNow('approachDangerousBall')

    elif transitions.shouldDribble(player):
        return player.goNow('dribble')

    elif transitions.shouldSpinToBallClose(player):
        return player.goNow('spinToBallClose')

    elif transitions.shouldStopAndKick(player):
        return player.goNow('stopBeforeKick')

    elif transitions.shouldPositionForKick(player):
        return player.goNow('decideKick')

    if player.firstFrame():
        player.brain.nav.chaseBall()
        player.hasAlignedOnce = False

    player.brain.tracker.trackBall()

    return player.stay()

def stopBeforeKick(player):
    """
    If we haven't decided which kick to do, but we should stop
    so we don't kick the ball away, stop
    """
    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.stopWalking()

    if player.brain.nav.isStopped():
        return player.goNow('decideKick')

    return player.stay()

def spinToBallClose(player):
    """
    If the ball is really close to us, but we aren't facing it yet,
    stop and spin toward it, then decide your kick.
    """
    player.brain.tracker.trackBall()

    if player.brain.ball.relY > constants.SHOULD_STOP_Y or \
            player.brain.ball.relY < -1*constants.SHOULD_STOP_Y:
        spinDir = player.brain.my.spinDirToPoint(player.brain.ball)
        print spinDir
        print player.brain.ball.relY
        print player.brain.my.h
        print player.brain.my.getRelativeBearing(player.brain.ball)
        player.setWalk(0, 0, spinDir*constants.BALL_SPIN_SPEED)
    else:
        player.stopWalking()
        return player.goNow('decideKick')

    return player.stay()

def decideKick(player):
    """
    Do a scan to determine where the goal is.
    Decide which kick to do accordingly.
    """
    if player.firstFrame():
        # Re-initialize to clear data from decideKick
        player.brain.kickDecider.kickInfo = \
            KickInformation.KickInformation(player)

        player.brain.tracker.kickDecideScan()

    elif player.counter > 43:
        return player.goNow('positionForKick')

    player.brain.kickDecider.kickInfo.collectData(player.brain)

    return player.stay()

def positionForKick(player):
    """
    State to align on the ball once we are near it
    """
    if player.firstFrame():
        kick = player.brain.kickDecider.kickInfo.getKick()
        player.brain.kickDecider.currentKick = kick

        if kick is None:
            player.angleToOrbit = player.brain.kickDecider.kickInfo.orbitAngle
            return player.goLater('orbitBall')

        player.inKickingState = True

        if transitions.shouldKickNow(player):
            return player.goLater('kickBallExecute')

        player.brain.nav.kickPosition(kick)

    player.brain.tracker.trackBall()

    # Leave this state if necessary
    if transitions.shouldStopAndKick(player):
        return player.goLater('preKickStop')

    if player.brain.tracker.activeLocOn:
        if transitions.shouldScanFindBallActiveLoc(player):
            player.inKickingState = False
            return player.goLater('scanFindBall')

    elif transitions.shouldScanFindBall(player):
        player.inKickingState = False
        return player.goLater('scanFindBall')

    if transitions.shouldChaseFromPositionForKick(player):
        player.inKickingState = False
        return player.goLater('chase')

    if not player.brain.play.isRole(GOALIE):
        if transitions.shouldDribble(player):
            return player.goLater('dribble')

    if player.brain.nav.isStopped():
        kick = player.brain.kickDecider.kickInfo.getKick()
        player.brain.nav.kickPosition(kick)

    return player.stay()

def dribble(player):
    """
    Keep running at the ball, but dribble
    """
    if player.firstFrame():
        player.brain.nav.dribble()

    if transitions.shouldScanFindBall(player):
        return player.goLater('scanFindBall')
    # if we should stop dribbling, see what else we should do
    if transitions.shouldStopDribbling(player):
        # may not be appropriate due to turned out feet...
        if transitions.shouldStopAndKick(player):
            return player.goLater('stopBeforeKick')
        if transitions.shouldPositionForKick(player):
            return player.goLater('decideKick')
        elif transitions.shouldChaseBall(player):
            return player.goLater('chase')

    return player.stay()

# TODO
def approachDangerousBall(player):
    if player.firstFrame():
        player.stopWalking()
    #print "approach dangerous ball"
    #single steps towards ball and goal with spin
    #player.setSteps(0, 0, 0, 0)
    ball = player.brain.ball
    my = player.brain.my
    if player.brain.nav.isStopped():
        if ball.dist >= 10:
            if ball.y > my.y + 7:
                player.brain.nav.walk(0, 10, 0)
            elif ball.y < my.y - 7:
                player.brain.nav.walk(0, -10, 0)

    if not goalTran.dangerousBall(player):
        return player.goLater('chase')
    if transitions.shouldScanFindBall(player):
        return player.goLater('scanFindBall')

    return player.stay()

def guardCorner(player):

    ball = player.brain.ball
    if player.brain.nav.isStopped():
        if ball.y > nogginConstants.LANDMARK_LEFT_POST_Y:
            player.brain.nav.goTo(RobotLocation(LANDMARK_LEFT_POST_X + 6, LANDMARK_LEFT_POST_Y, 0))
        elif ball.y < nogginConstants.LANDMARK_RIGHT_POST_Y:
            player.brain.nav.goTo(RobotLocation(LANDMARK_RIGHT_POST_X + 6, LANDMARK_RIGHT_POST_Y, 0))

    return player.stay()

def orbitBall(player):
    """
    State to orbit the ball
    """
    if player.firstFrame():
        player.brain.nav.orbitAngle(player.angleToOrbit)
        player.brain.tracker.trackBall()

    if transitions.shouldScanFindBall(player):
        return player.goLater('scanFindBall')

    if transitions.shouldChaseFromPositionForKick(player):
        return player.goLater('chase')

    elif player.brain.nav.isStopped():
        return player.goLater('chase')
    return player.stay()
