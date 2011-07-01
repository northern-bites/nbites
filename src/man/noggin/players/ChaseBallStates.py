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
        player.brain.kickDecider.decideKick()
        return player.goNow('positionForKick')

def positionForKick(player):
    """
    Get to the ball
    """
    if player.brain.ball.dist > 60:
        player.brain.nav.chaseBall()
        player.kickDecideScan()

    else:
        kick = player.brain.kickDecider.getKick()

        #if player.firstFrame():
        player.inKickingState = True
        player.brain.nav.kickPosition(kick)

        # TODO make this a better way to tell if the ball has moved. also we should go back to chase.
        # if we're getting close, decide whether to set another destination
        if player.brain.nav.nearDestination and player.brain.nav.brain.ball.dist > 20:
            print 'Ball far away ({0}), setting new destination' \
                .format(player.brain.nav.brain.ball.dist)
            player.brain.nav.kickPosition(kick)

        if transitions.ballInPosition(player) and transitions.shouldKick(player):
            if kick.nullKick:
                print "Don't have a kick, orbitting"
                return player.goNow('orbitBall')
            else:
                return player.goNow('kickBallExecute')

    if transitions.shouldFindBallKick(player):
        player.inKickingState = False
        return player.goLater('findBall')

    return player.stay()

def orbitBall(player):
    """
    State to orbit the ball
    """
    if player.firstFrame():
        player.brain.nav.orbitAngle(80) # TODO HACK HACK
        player.brain.tracker.bounceHead()

    if transitions.shouldFindBall(player):
        return player.goLater('findBall')

    elif player.brain.nav.isStopped():
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
