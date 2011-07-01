import PositionConstants as constants

####### POSITIONING STUFF ##############
def shouldKickAtPosition(player):
    """
    Is the ball close enough that we should kick
    """
    return (player.brain.ball.vis.on and
            player.brain.ball.dist < constants.AT_POSITION_KICK_DIST)
