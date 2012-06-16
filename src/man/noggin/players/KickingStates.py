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
        player.brain.speech.say("Kick it")
        player.brain.tracker.trackBallFixedPitch()

        print "Performing " + str(player.kick)
        kick = player.kick.sweetMove

        player.executeMove(kick)
        return player.stay()

    if player.counter > 10 and player.brain.nav.isStopped():
        return player.goNow('afterKick')

    return player.stay()

def afterKick(player):
    """
    State to follow up after a kick.
    Currently exits after one frame.
    """
    if player.firstFrame():
        player.stand()        # stand up right, ready to walk
        kick = player.kick
        player.brain.tracker.afterKickScan(kick.name)

        if kick.isBackKick():
            player.inKickingState = False
            return player.goNow('spinAfterBackKick')

        return player.stay()

    if transitions.shouldKickAgain(player):
        player.kick = kicks.chooseAlignedKickFromKick(player, player.kick)
        return player.goNow('positionForKick')

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
        player.brain.tracker.trackBallFixedPitch()
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

        player.brain.tracker.spinPanFixedPitch()

    return player.stay()
