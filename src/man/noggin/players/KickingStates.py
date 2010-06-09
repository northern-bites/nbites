"""
Here we house all of the state methods used for kicking the ball
"""

import man.motion.SweetMoves as SweetMoves
import KickingConstants as constants

def kickBallExecute(player):
    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.executeMove(player.brain.kickDecider.getSweetMove())

        if not player.penaltyMadeFirstKick:
            player.penaltyMadeFirstKick = True
        elif not player.penaltyMadeSecondKick:
            player.penaltyMadeSecondKick = True

    if player.counter > 1 and player.brain.nav.isStopped():
        return player.goLater('afterKick')

    return player.stay()

def afterKick(player):
    """
    State to follow up after a kick.
    Currently exits after one frame.
    """
    tracker = player.brain.tracker

    # trick the robot into standing up instead of leaning to the side
    if player.firstFrame():
        player.hasAlignedOnce = False
        player.standup()

        if player.penaltyKicking:
            return player.goLater('penaltyKickRelocalize')

        if player.brain.ball.on:
            tracker.trackBall()
        return player.stay()

    if player.chosenKick == SweetMoves.LEFT_SIDE_KICK or \
            player.chosenKick == SweetMoves.RIGHT_SIDE_KICK:
        player.inKickingState = False
        return player.goLater('spinFindBall')

    if not player.brain.motion.isHeadActive():
        player.inKickingState = False
        return player.goLater('scanFindBall')
    return player.stay()
