# TODO: get rid of this file ??

def penaltyBallInOppGoalbox(player):
    """
    We can't do anything if the ball is in the opponent's goalbox
    """
    if player.firstFrame():
        player.stopWalking()
        player.brain.tracker.trackBallFixedPitch()
    if not player.brain.ball.loc.inOppGoalBox():
        return player.goLater('chase')
    return player.stay()

