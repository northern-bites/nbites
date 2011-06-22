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
    elif transitions.shouldClaimBall(player):
        return player.goNow('claimBall')
    elif transitions.shouldPositionForKick(player):
        return player.goNow('decideKick')
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
    elif transitions.shouldClaimBall(player):
        return player.goNow('claimBall')
    elif transitions.shouldPositionForKick(player):
        return player.goNow('decideKick')
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
    elif transitions.shouldClaimBall(player):
        return player.goNow('claimBall')
    elif transitions.shouldPositionForKick(player):
        return player.goNow('decideKick')

    if player.firstFrame():
        player.brain.nav.chaseBall()
        player.brain.tracker.trackBall()

    return player.stay()

def claimBall(player):
    """
    If we haven't decided which kick to do, but we would kick
    the ball away if we decided our kick on the move, go claim
    it and then decide.
    """
    if player.firstFrame():
        kick = player.brain.kickDecider.getCenterKickPosition()
        player.brain.tracker.trackBall()
        player.brain.nav.kickPosition(kick)

    if transitions.shouldKick(player):
        return player.goNow('decideKick')
    elif transitions.shouldFindBall(player):
        return player.goLater('findBall')
    elif transitions.shouldChaseFromClaimBall(player):
        return player.goNow('chase')

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
        return player.goNow('chase')

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

    #TODO change this to be better.
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

        player.brain.tracker.trackBall()
        player.brain.nav.kickPosition(kick)

    if transitions.shouldKick(player):
        return player.goNow('kickBallExecute')
    elif transitions.shouldFindBallKick(player):
        player.inKickingState = False
        return player.goLater('findBall')
    elif transitions.shouldChaseFromPositionForKick(player):
        player.inKickingState = False
        return player.goLater('chase')

    return player.stay()

def kickOff(player):
    """
    Perform special behavior when we are kicking off
    """
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
        player.brain.nav.orbitAngle(player.angleToOrbit)
        player.brain.tracker.trackBall()

    if transitions.shouldFindBall(player):
        return player.goLater('findBall')
    if transitions.shouldChaseFromPositionForKick(player):
        return player.goLater('chase')
    elif player.brain.nav.isStopped():
        return player.goLater('chase')
    return player.stay()
