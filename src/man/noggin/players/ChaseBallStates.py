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
    if transitions.shouldFindBall(player):
        return player.goNow('findBall')

    if player.brain.play.isRole(GOALIE) and goalTran.dangerousBall(player):
        return player.goNow('approachDangerousBall')

    elif player.penaltyKicking and player.brain.ball.inOppGoalBox():
        return player.goNow('penaltyBallInOppGoalbox')

    else:
        return player.goNow('positionForKick')

def positionForKick(player):
    """
    Get to the ball.
    Uses chaseBall to walk to the ball when its far away, and positionForKick
    once we get close. This allows Player to monitor Navigator's progress as it
    positions.
    """
    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.inKickingState = True

    if player.counter % 10 is 0:
        player.brain.kickDecider.decideKick()
        player.brain.nav.kickPosition(player.brain.kickDecider.getKick())

    # most of the time going to chase will kick back to here, lets us reset
    if transitions.shouldFindBallKick(player):
        player.inKickingState = False
        return player.goLater('chase')

    #if transitions.shouldKick(player):
    if transitions.ballInPosition(player):
        if transitions.shouldOrbit(player):
            print "Don't have a kick, orbitting"
            return player.goNow('orbitBall')
        else:
            return player.goNow('kickBallExecute')

    return player.stay()

def orbitBall(player):
    """
    State to orbit the ball
    """
    if player.firstFrame():
        player.brain.nav.orbitAngle(45) # TODO HACK HACK
        player.brain.tracker.bounceHead()

    if transitions.shouldFindBall(player) or player.brain.nav.isStopped():
        player.inKickingState = False
        player.shouldOrbit = False
        player.brain.tracker.trackBall()
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

    if not goalTran.dangerousBall(player) or transitions.shouldFindBall(player):
        return player.goLater('chase')

    return player.stay()

