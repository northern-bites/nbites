"""
Here we house all of the state methods used for chasing the ball
"""
from man.noggin.util import MyMath
from man.motion import SweetMoves
import ChaseBallConstants as constants
import ChaseBallTransitions as transitions
import KickingHelpers
import GoalieTransitions as goalTran
from ..playbook.PBConstants import GOALIE

def chase(player):
    """
    Method to determine which chase state should be used.
    We dump the robot into this state when we our switching from something else.
    """
    player.isChasing = True
    player.hasAlignedOnce = False

    if player.brain.play.isRole(GOALIE):
        if transitions.shouldScanFindBall(player):
            return player.goNow('scanFindBall')
        elif transitions.shouldApproachBall(player):
            return player.goNow('approachBall')
        elif transitions.shouldKick(player):
            return player.goNow('waitBeforeKick')
        else:
            return player.goNow('scanFindBall')

    if transitions.shouldScanFindBall(player):
        return player.goNow('scanFindBall')
    elif transitions.shouldApproachBall(player):
        return player.goNow('approachBall')
    elif transitions.shouldKick(player):
        return player.goNow('waitBeforeKick')
    else:
        return player.goNow('scanFindBall')

def chaseAfterKick(player):
    player.brain.tracker.trackBall()
    return player.goLater('chase')

def approachBall(player):
    """
    Once we are alligned with the ball, approach it
    """
    if player.firstFrame():
        player.brain.nav.chaseBall()
        player.hasAlignedOnce = False

    if transitions.shouldActiveLoc(player):
        player.brain.tracker.activeLoc()
    else:
        player.brain.tracker.trackBall()

    # Switch to other states if we should
    if player.penaltyKicking and \
           player.brain.ball.inOppGoalBox():
        return player.goNow('penaltyBallInOppGoalbox')

    elif transitions.shouldKick(player):
        return player.goNow('waitBeforeKick')

    elif player.brain.play.isRole(GOALIE):
        if goalTran.dangerousBall(player):
            return player.goNow('approachDangerousBall')

    else:
        if transitions.shouldDribble(player):
            return player.goNow('dribble')

    if transitions.shouldPositionForKick(player):
        return player.goNow('positionForKick')

    if player.brain.tracker.activeLocOn:
        if transitions.shouldScanFindBallActiveLoc(player):
            return player.goLater('scanFindBall')
    else:
        if transitions.shouldScanFindBall(player):
            return player.goLater('scanFindBall')

    return player.stay()

def positionForKick(player):
    """
    State to align on the ball once we are near it
    """

    if player.firstFrame():
        player.brain.kickDecider.decideKick()
        player.brain.nav.kickPosition()
        player.inKickingState = True

    player.brain.tracker.trackBall()

    # Leave this state if necessary
    if transitions.shouldKick(player):
        return player.goNow('kickBallExecute')

    elif transitions.shouldScanFindBallActiveLoc(player):
        player.inKickingState = False
        return player.goLater('scanFindBall')

    elif transitions.shouldApproachFromPositionForKick(player):
        player.inKickingState = False
        return player.goLater('approachBall')

    if not player.brain.play.isRole(GOALIE):
        if transitions.shouldDribble(player):
            return player.goNow('dribble')

    return player.stay()

def dribble(player):
    """
    Keep running at the ball, but dribble
    """
    if player.firstFrame():
        player.brain.nav.dribble()

    # if we should stop dribbling, see what else we should do
    if transitions.shouldStopDribbling(player):

        # may not be appropriate due to turned out feet...
        if transitions.shouldPositionForKick(player):
            return player.goNow('positionForKick')
        elif transitions.shouldApproachBall(player):
            return player.goNow('approachBall')

    return player.stay()

def steps(player):
    if player.brain.nav.isStopped():
        player.setSteps(3,3,0,5)
    elif player.brain.nav.currentState != "stepping":
        player.stopWalking()
    return player.stay()

# TODO
def approachDangerousBall(player):
    if player.firstFrame():
        player.stopWalking()
    #print "approach dangerous ball"
    #single steps towards ball and goal with spin
    player.setSteps(0, 0, 0, 0)

    if not goalTran.dangerousBall(player):
        return player.goLater('approachBall')
    if transitions.shouldScanFindBall(player):
        return player.goLater('scanFindBall')
    elif transitions.shouldSpinFindBall(player):
        return player.goLater('spinFindBall')

    return player.stay()
