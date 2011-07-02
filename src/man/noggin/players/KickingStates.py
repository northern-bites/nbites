from . import ChaseBallTransitions as transitions
from . import ChaseBallConstants as constants
from ..kickDecider import kicks

"""
Here we house all of the state methods used for kicking the ball
"""

def kickBallExecute(player):
    """
    Kick the ball
    """
    if player.firstFrame():
        player.brain.tracker.trackBall()
        if transitions.ballInPosition(player):
            player.executeMove(player.brain.kickDecider.getSweetMove())
        else:
            #Either it's close and we can't kick it now or it's far
            #away and we should search.  Lets hope its close and let
            #positionForKick put us in findBall if needed
            player.goNow('positionForKick')

        #if player.penaltyKicking:
        if not player.penaltyMadeFirstKick:
            player.penaltyMadeFirstKick = True
        elif not player.penaltyMadeSecondKick:
            player.penaltyMadeSecondKick = True

    if player.counter > 1 and player.brain.nav.isStopped():
        player.brain.nav.justKicked = True
        return player.goLater('afterKick')

    return player.stay()

def afterKick(player):
    """
    State to follow up after a kick.
    Currently exits after one frame.
    """
    # trick the robot into standing up instead of leaning to the side
    if player.firstFrame():
        player.standup()
        player.brain.tracker.trackBall()

        kick = player.brain.kickDecider.getKick()

        player.brain.tracker.afterKickScan(kick.name)

        if kick.isBackKick():
            player.inKickingState = False
            return player.goLater('spinAfterBackKick')

        return player.stay()

    if transitions.shouldKickAgain(player):
        player.brain.nav.justKicked = False
        return player.goNow('positionForKick')

    if transitions.shouldFindBallKick(player):
        player.inKickingState = False
        player.hasKickedOff = True
        player.brain.nav.justKicked = False
        return player.goLater('findBall')
    if ((player.counter > 1 and player.brain.nav.isStopped()) or
        transitions.shouldChaseBall(player)):
        player.inKickingState = False
        player.hasKickedOff = True
        player.brain.nav.justKicked = False
        return player.goNow('chase')

    return player.stay()

def spinAfterBackKick(player):
    """
    State to spin to the ball after we kick it behind us.
    """
    if transitions.shouldChaseBall(player):
        player.stopWalking()
        player.brain.tracker.trackBall()
        return player.goNow('chase')

    if player.firstFrame():
        player.brain.tracker.stopHeadMoves()
        player.stopWalking()

    if player.brain.nav.isStopped() and player.brain.tracker.isStopped():
        kick = player.brain.kickDecider.getKick()
        if kick is kicks.LEFT_LONG_BACK_KICK or kick is kicks.LEFT_SHORT_BACK_KICK:
            player.setWalk(0, 0, constants.FIND_BALL_SPIN_SPEED)
        else:
            player.setWalk(0, 0, -1*constants.FIND_BALL_SPIN_SPEED)

        player.brain.tracker.trackBallSpin()

    return player.stay()
