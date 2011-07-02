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
        # This doesn't seem to help any more, need to investigate
        player.saveBallPosition()

        if player.brain.ball.dist > constants.BALL_SET_DEST_CUTOFF:
            player.brain.speech.say("Speed walking")
            player.brain.nav.chaseBall()

            if player.brain.ball.vis.framesOn > 20:
                #player.brain.tracker.kickDecideScan()
                #else:
                player.brain.tracker.trackBall()

        else:
            player.brain.kickDecider.decideKick()
            kick = player.brain.kickDecider.getKick()

            player.inKickingState = True
            player.brain.tracker.trackBall()
            player.brain.nav.kickPosition(kick)

    if transitions.shouldKick(player):
        if transitions.shouldOrbit(player):
            print "Don't have a kick, orbitting"
            return player.goNow('orbitBall')
        else:
            return player.goNow('kickBallExecute')

    # most of the time going to chase will kick back to here, lets us reset
    if (transitions.ballTooFar(player) or
        transitions.shouldSwitchPFKModes(player) or
        transitions.shouldFindBallKick(player)):

        player.inKickingState = False
        return player.goLater('chase')

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

