"""
Here we house all of the state methods used for chasing the ball
"""
import ChaseBallTransitions as transitions
import ChaseBallConstants as constants
import GoalieTransitions as goalTran
from ..playbook.PBConstants import GOALIE

def chase(player):
    """
    Super State to determine what to do from various situations
    """
    player.isChasing = True
    player.hasAlignedOnce = False

    if player.brain.play.isRole(GOALIE):
        return player.goNow('goalieChase')

    # Check in order of importance
    if transitions.shouldFindBall(player):
        return player.goNow('findBall')
    elif transitions.shouldKickOff(player):
        return player.goNow('kickOff')
    elif transitions.shouldStopBeforeKick(player):
        return player.goNow('stopBeforeKick')
    elif transitions.shouldPositionForKick(player):
        return player.goNow('decideKick')
    elif transitions.shouldApproachBall(player):
        return player.goNow('approachBall')
    else:
        return player.goNow('findBall')

def goalieChase(player):
    """
    TODO: make goalie more aggressive (different transitions?)
    """
    # Check in order of importance

    #tells the goalie what state its in
    if player.firstFrame():
        player.isChasing = True
        player.isPositioning = False
        player.isSaving = False

    if transitions.shouldFindBall(player):
        return player.goNow('findBall')
    elif transitions.shouldSpinToBallClose(player):
        return player.goNow('spinToBallClose')
    elif transitions.shouldStopBeforeKick(player):
        return player.goNow('stopBeforeKick')
    elif transitions.shouldPositionForKick(player):
        return player.goNow('decideKick')
    elif transitions.shouldApproachBall(player):
        return player.goNow('approachBall')
    else:
        return player.goNow('findBall')

def approachBall(player):
    """
    Once we are aligned with the ball, approach it
    """
    # Switch to other states if we should
    if player.penaltyKicking and \
           player.brain.ball.inOppGoalBox():
        return player.goNow('penaltyBallInOppGoalbox')
    elif player.brain.tracker.activeLocOn:
        if transitions.shouldFindBallActiveLoc(player):
            return player.goLater('findBall')
    elif transitions.shouldFindBall(player):
        return player.goLater('findBall')
    elif player.brain.play.isRole(GOALIE) and goalTran.dangerousBall(player):
        return player.goNow('approachDangerousBall')
    elif transitions.shouldDribble(player):
        return player.goNow('dribble')
    elif transitions.shouldSpinToBallClose(player):
        return player.goNow('spinToBallClose')
    elif transitions.shouldStopBeforeKick(player):
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
    ball = player.brain.ball

    if (ball.relY > constants.SHOULD_STOP_Y or
        ball.relY < -1*constants.SHOULD_STOP_Y):
        spinDir = player.brain.my.spinDirToPoint(ball)
        player.setWalk(0, 0, spinDir*constants.BALL_SPIN_SPEED)
    else:
        if ball.dist > constants.SHOULD_START_DIST:
            player.brain.nav.chaseBall()
        else:
            player.stopWalking()
        return player.goNow('decideKick')
    if player.brain.tracker.activeLocOn:
        if transitions.shouldFindBallActiveLoc(player):
            return player.goLater('findBall')
    elif transitions.shouldFindBall(player):
        return player.goLater('findBall')

    return player.stay()

def spinToKick(player):
    """
    If the ball is behind the tip of our foot, but outside it, spin
    """
    player.brain.tracker.trackBall()
    ball = player.brain.ball

    if (ball.relY > constants.SHOULD_SPIN_TO_KICK_Y or
        ball.relY < -1*constants.SHOULD_SPIN_TO_KICK_Y):
        spinDir = player.brain.my.spinDirToPoint(ball)
        player.setWalk(0,0,spinDir*constants.BALL_SPIN_SPEED)
    else:
        if ball.dist > constants.SHOULD_SPIN_TO_KICK_DIST:
            player.brain.nav.chaseBall()
        else:
            player.stopWalking()
        return player.goNow('decideKick')
    if player.brain.tracker.activeLocOn:
        if transitions.shouldFindBallActiveLoc(player):
            return player.goLater('findBall')
    elif transitions.shouldFindBall(player):
        return player.goLater('findBall')

    return player.stay()

def decideKick(player):
    """
    Do a scan to determine where the goal is.
    Decide which kick to do accordingly.
    """
    if player.firstFrame():
        # Re-initialize to clear data from decideKick
        player.brain.kickDecider.resetInfo()

        player.brain.tracker.kickDecideScan()

    elif player.counter > 43: #time required for scan
        return player.goNow('positionForKick')

    player.brain.kickDecider.collectInfo()

    return player.stay()

def positionForKick(player):
    """
    State to align on the ball once we are near it
    """
    if player.firstFrame():
        kick = player.brain.kickDecider.getKick()

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
        if transitions.shouldFindBallActiveLoc(player):
            player.inKickingState = False
            return player.goLater('findBall')
    elif transitions.shouldFindBallKick(player):
        player.inKickingState = False
        return player.goLater('findBall')
    elif transitions.shouldSpinToKick(player):
        return player.goLater('spinToKick')
    if transitions.shouldChaseFromPositionForKick(player):
        player.inKickingState = False
        return player.goLater('chase')
    if not player.brain.play.isRole(GOALIE):
        if transitions.shouldDribble(player):
            return player.goLater('dribble')

    if player.brain.nav.isStopped():
        kick = player.brain.kickDecider.getKick()
        player.brain.nav.kickPosition(kick)

    return player.stay()

def kickOff(player):
    """
    Perform special behavior when we are kicking off
    """
    # Hard coded constant (3) since when we switch to 4 field players,
    # this won't be the case anymore.
    smallTeam = player.brain.playbook.pb.numActiveFieldPlayers < 3
    player.brain.kickDecider.setKickOff(smallTeam)

    return player.goNow('positionForKick')

def dribble(player):
    """
    Keep running at the ball, but dribble
    """
    if player.firstFrame():
        player.brain.nav.dribble()

    if transitions.shouldFindBallKick(player):
        return player.goLater('findBall')
    # if we should stop dribbling, see what else we should do
    if transitions.shouldStopDribbling(player):
        # may not be appropriate due to turned out feet...
        if transitions.shouldStopBeforeKick(player):
            return player.goLater('stopBeforeKick')
        if transitions.shouldPositionForKick(player):
            return player.goLater('decideKick')
        elif transitions.shouldChaseBall(player):
            return player.goLater('chase')

    return player.stay()

#adjusts position to be farther away from the ball
#if the goalie is too close to the ball while in
#the goal box
def approachDangerousBall(player):
    ball = player.brain.ball
    my = player.brain.my
    if player.firstFrame():
        player.stopWalking()

    #move away from the ball so it is no longer dangerous
    if player.brain.nav.isStopped():
        if ball.relY > 0:
            player.brain.nav.walk(0, -15, 0)
        else:
            player.brain.nav.walk(0, 15, 0)

    if not goalTran.dangerousBall(player):
        return player.goLater('chase')
    if transitions.shouldFindBall(player):
        return player.goLater('findBall')

    return player.stay()

def orbitBall(player):
    """
    State to orbit the ball
    """
    if player.firstFrame():
        player.brain.nav.orbitAngle(player.angleToOrbit)
        player.brain.tracker.trackBall()

    if transitions.shouldFindBall(player):
        return player.goLater('findBall')
    if transitions.shouldChaseFromPositionForKick(player):
        return player.goLater('chase')
    elif player.brain.nav.isStopped():
        return player.goLater('chase')
    return player.stay()
