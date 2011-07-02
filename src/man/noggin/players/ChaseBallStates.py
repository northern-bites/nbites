"""
Here we house all of the state methods used for chasing the ball
"""
import ChaseBallTransitions as transitions
import GoalieTransitions as goalTran
from ..playbook.PBConstants import GOALIE

def chase(player):
    """
    Super State to determine what to do from various situations
    """
    if transitions.shouldFindBall(player):
        return player.goNow('findBall')

    if player.brain.play.isRole(GOALIE) and goalTran.dangerousBall(player):
        return player.goNow('goalieChase')

    elif player.penaltyKicking and player.brain.ball.inOppGoalBox():
        return player.goNow('penaltyBallInOppGoalbox')

    else:
        return player.goNow('positionForKick')

def positionForKick(player):
    """
    Get to the ball
    """

    """
    if player.brain.ball.dist > 80:
        player.brain.nav.chaseBall()
        if player.brain.ball.framesOn > 20:
            player.brain.tracker.kickDecideScan()
    else:
        # should be in a firstFrame
        player.brain.kickDecider.decideKick()
        kick = player.brain.kickDecider.getKick()
    """

    if player.firstFrame():
        player.brain.kickDecider.decideKick()
        kick = player.brain.kickDecider.getKick()

        player.inKickingState = True
        player.brain.tracker.trackBall()
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
        if transitions.shouldOrbit(player):
            print "Don't have a kick, orbitting"
            return player.goNow('orbitBall')
        else:
            return player.goNow('kickBallExecute')

    if player.ballMoved():
        player.inKickingState = False
        return player.goLater('chase')

    if transitions.shouldFindBallKick(player):
        player.inKickingState = False
        return player.goLater('findBall')

    return player.stay()

def orbitBall(player):
    """
    State to orbit the ball
    """
    if player.firstFrame():
        player.brain.nav.orbitAngle(45) # TODO HACK HACK
        player.brain.tracker.bounceHead()

    if transitions.shouldFindBall(player):
        player.inKickingState = False
        player.shouldOrbit = False
        return player.goLater('findBall')

    elif player.brain.nav.isStopped():
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

    if not goalTran.dangerousBall(player):
        return player.goLater('chase')
    if transitions.shouldFindBall(player):
        return player.goLater('findBall')

    return player.stay()

