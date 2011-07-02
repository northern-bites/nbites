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
    if player.brain.play.isRole(GOALIE):
        return player.goNow('goalieChase')

    # Check in order of importance
    if transitions.shouldFindBall(player):
        return player.goNow('findBall')

    elif transitions.shouldKickOff(player):
        return player.goNow('kickOff')

    elif transitions.shouldSpinToBall(player):
        return player.goNow('spinToBall')

    else:
        return player.goNow('approachBall')

def goalieChase(player):
    """
    TODO: make goalie more aggressive (different transitions?)
    TODO: dangerousBall??
    """
    # Check in order of importance

    if transitions.shouldFindBall(player):
        return player.goNow('findBall')
    elif player.brain.play.isRole(GOALIE) and goalTran.dangerousBall(player):
        return player.goNow('approachDangerousBall')
    elif transitions.shouldSpinToBall(player):
        return player.goNow('spinToBall')

    elif transitions.shouldPositionForKick(player):
        return player.goNow('positionForKick')
    else:
        return player.goNow('approachBall')

def approachBall(player):
    """
    Once we are aligned with the ball, approach it
    """
    # Switch to other states if we should
    if player.penaltyKicking and player.brain.ball.inOppGoalBox():
        return player.goNow('penaltyBallInOppGoalbox')
    elif transitions.shouldFindBall(player):
        return player.goLater('findBall')
    elif player.brain.play.isRole(GOALIE) and goalTran.dangerousBall(player):
        return player.goNow('approachDangerousBall')
    elif transitions.shouldSpinToBall(player):
        return player.goNow('spinToBall')

    elif transitions.shouldPositionForKick(player):
        return player.goNow('positionForKick')

    if player.firstFrame():
        player.brain.nav.chaseBall()
        player.brain.tracker.trackBall()

    return player.stay()

def spinToBall(player):
    """
    If the ball is close to us, but we aren't facing it yet,
    stop and spin toward it, then decide your kick.
    """
    # in case we lose the ball in our shoulder, always trackBall.
    player.brain.tracker.trackBall()
    ball = player.brain.ball
    spinDir = player.brain.my.spinDirToPoint(ball)
    player.setWalk(0, 0, spinDir*constants.BALL_SPIN_SPEED)

    if transitions.shouldFindBall(player):
        return player.goLater('findBall')
    elif transitions.shouldChaseFromSpinToBall(player):
        player.brain.nav.chaseBall()
        return player.goNow('chase')

    return player.stay()

def positionForKick(player):
    """
    State to align on the ball once we are near it.
    """
    if player.firstFrame():
        player.brain.kickDecider.decideKick()
        kick = player.brain.kickDecider.getKick()

        print "Chose: {0}".format(kick)
        player.inKickingState = True

        player.brain.tracker.trackBall()

        if kick.nullKick:
            print "Don't have a kick, orbitting"
            return player.goNow('orbitBall')
        else:
            player.saveBallPosition()
            player.brain.nav.kickPosition(kick)

    # if we're getting close, decide whether to set another destination
    # also, set a new destination if the ball has moved from its absolute loc
    if player.brain.nav.nearDestination and player.brain.nav.brain.ball.dist > 30:
        print 'Ball far away ({0}), setting new destination' \
              .format(player.brain.nav.brain.ball.dist)
        kick = player.brain.kickDecider.getKick()
        player.brain.nav.kickPosition(kick)

    if transitions.shouldKick(player):
        return player.goNow('kickBallExecute')

    elif player.ballMoved():
        return player.goLater('chase')

    elif transitions.shouldFindBallKick(player):
        player.inKickingState = False
        return player.goLater('findBall')

    return player.stay()

def kickOff(player):
    """
    Perform special behavior when we are kicking off
    """
    if player.firstFrame():
        player.brain.kickDecider.setKickOff()

    if transitions.shouldPositionForKick(player):
        return player.goNow('positionForKick')

    if player.firstFrame():
        player.brain.nav.chaseBall()
        player.brain.tracker.trackBall()

    return player.stay()

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
        return player.goLater('chase')
    return player.stay()

def approachDangerousBall(player):
    """adjusts position to be farther away from the ball
    if the goalie is too close to the ball while in
    the goal box"""
    if player.firstFrame():
        player.stopWalking()

    #move away from the ball so it is no longer dangerous
    if player.brain.nav.isStopped():
        if player.brain.ball.relY > 0:
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
        player.brain.nav.orbitAngle(45) # TODO HACK HACK
        player.brain.tracker.kickDecideScan()

    if transitions.shouldFindBall(player):
        return player.goLater('findBall')

    elif player.brain.nav.isStopped():
        return player.goLater('chase')

    return player.stay()
