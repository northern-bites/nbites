"""
Here we house all of the state methods used for kicking the ball
"""

def kickBallExecute(player):
    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.executeMove(player.brain.kickDecider.getSweetMove())

        #if player.penaltyKicking:
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
    # trick the robot into standing up instead of leaning to the side
    if player.firstFrame():
        player.hasAlignedOnce = False
        player.standup()

        if player.penaltyKicking:
            return player.goLater('penaltyKickRelocalize')

        return player.stay()

    if player.brain.ball.on:
        player.brain.tracker.trackBall()
        return player.goLater('chase')

    if not player.brain.motion.isHeadActive():
        player.inKickingState = False
        return player.goLater('scanFindBall')
    return player.stay()
