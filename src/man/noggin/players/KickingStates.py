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

        print "performing " + str(player.kick)
        kick = player.kick.sweetMove

#        if transitions.ballInPosition(player) and kick is not None:
        player.executeMove(kick)
        return player.stay()
#        else:
            #Either it's close and we can't kick it now or it's far
            #away and we should search.

    if player.counter > 10 and player.brain.nav.isStopped():
        return player.goLater('chase')
#        return player.goLater('afterKick')

    return player.stay()

def afterKick(player):
    """
    State to follow up after a kick.
    Currently exits after one frame.
    """
    if player.firstFrame():
        player.stand()        # stand up right, ready to walk
        player.brain.tracker.trackBallFixedPitch()

        kick = player.brain.kickDecider.getKick()

        # TODO: fix after kick scans @summer 2012
        #player.brain.tracker.afterKickScan(kick.name)

        if kick.isBackKick():
            player.inKickingState = False
            return player.goLater('spinAfterBackKick')

        return player.stay()

    if transitions.shouldKickAgain(player):
        return player.goNow('positionForKick')

    if ((player.counter > 1 and player.brain.nav.isStopped()) or
        transitions.shouldChaseBall(player) or
        transitions.shouldFindBallKick(player)):
        player.inKickingState = False
        return player.goNow('chase')

    return player.stay()

def spinAfterBackKick(player):
    """
    State to spin to the ball after we kick it behind us.
    """
    if transitions.shouldChaseBall(player):
        player.stopWalking()
        player.brain.tracker.trackBallFixedPitch()
        return player.goNow('chase')

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
