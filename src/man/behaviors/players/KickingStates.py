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

        kickBallExecute.sweetMove = player.kick.sweetMove

        kickBallExecute.preKickDelay = 30
        return player.stay()

    # wait a second for stability.
    kickBallExecute.preKickDelay -= 1

    if kickBallExecute.preKickDelay == 0:
        player.executeMove(kickBallExecute.sweetMove)
        return player.stay()

    if player.counter > 40 and player.brain.nav.isStopped():
        return player.goNow('afterKick')

    return player.stay()

kickBallExecute.preKickDelay = 0
kickBallExecute.sweetMove = None

def afterKick(player):
    """
    State to follow up after a kick.
    Currently exits after one frame.
    """
    if player.firstFrame():
        player.stand()        # stand up right, ready to walk
        player.brain.tracker.afterKickScan(player.kick.name)
        return player.stay()

    if transitions.shouldKickAgain(player):
        player.kick = kicks.chooseAlignedKickFromKick(player, player.kick)
        return player.goNow('positionForKick')

    if player.kick.isBackKick() and player.counter > 10:
        player.inKickingState = False
        return player.goNow('spinAfterBackKick')


    if (transitions.shouldChaseBall(player) or
        transitions.shouldFindBallKick(player)):
        player.inKickingState = False
        return player.goLater('chase')

    return player.stay()

def spinAfterBackKick(player):
    """
    State to spin to the ball after we kick it behind us.
    """
    # TODO This is essentially spinFindBall... maybe we go straight to that.

    if transitions.shouldChaseBall(player):
        player.stopWalking()
        player.brain.tracker.trackBall()
        return player.goLater('chase')

    if player.firstFrame():
        player.brain.tracker.stopHeadMoves()
        player.stopWalking()

    if player.brain.nav.isStopped() and player.brain.tracker.isStopped():
        kick = player.brain.kickDecider.getKick()
        if (kick is kicks.LEFT_LONG_BACK_KICK or
            kick is kicks.LEFT_SHORT_BACK_KICK):
            player.setWalk(0, 0, constants.FIND_BALL_SPIN_SPEED)
        else:
            player.setWalk(0, 0, -1*constants.FIND_BALL_SPIN_SPEED)

        player.brain.tracker.spinPan()

    return player.stay()
