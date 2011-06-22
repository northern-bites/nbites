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
        player.executeMove(player.brain.kickDecider.getSweetMove())

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

        kick = player.brain.kickDecider.getKick()

        # We need to find it!
        if not player.brain.ball.on:
            if kick is kicks.LEFT_SIDE_KICK:
                player.brain.tracker.lookToDir("right")
            elif kick is kicks.RIGHT_SIDE_KICK:
                player.brain.tracker.lookToDir("left")
            elif kick is kicks.RIGHT_DYNAMIC_STRAIGHT_KICK or \
                    kick is kicks.LEFT_DYNAMIC_STRAIGHT_KICK:
                player.brain.tracker.kickDecideScan() # should scan upper reaches.
            elif kick is kicks.LEFT_LONG_BACK_KICK or kicks.LEFT_SHORT_BACK_KICK:
                player.setWalk(0, 0, constants.FIND_BALL_SPIN_SPEED)
                player.brain.tracker.trackBallSpin()
            else:
                player.setWalk(0, 0, -1*constants.FIND_BALL_SPIN_SPEED)
                player.brain.tracker.trackBallSpin()

        if player.penaltyKicking:
            return player.goLater('penaltyKickRelocalize')

        return player.stay()

    if transitions.shouldKickAgain(player):
        player.brain.nav.justKicked = False
        return player.goNow('positionForKick')
    if transitions.shouldFindBallKick(player):
        player.inKickingState = False
        player.hasKickedOff = True
        player.brain.nav.justKicked = False
        return player.goLater('findBall')
    if player.brain.nav.isStopped() or transitions.shoulChaseBall(player):
        player.inKickingState = False
        player.hasKickedOff = True
        player.brain.nav.justKicked = False
        return player.goNow('chase')

    return player.stay()
