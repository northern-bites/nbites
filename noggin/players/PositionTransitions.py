import PositionConstants as constants

####### POSITIONING STUFF ##############

def shouldTurnToBall_fromAtBallPosition(player):
    """
    Should we spin towards the ball, if already at our position
    """
    ball = player.brain.ball
    return (ball.on and
        abs(ball.bearing > (constants.BALL_SPIN_POSITION_THRESH +
                            constants.BALL_SPIN_BUFFER)))

def atSpinBallDir(player):
    """
    Should we stop spinning because we are facing the ball
    """
    ball = player.brain.ball
    return (ball.on and
            abs(ball.bearing < constants.BALL_SPIN_POSITION_THRESH))

def shouldSpinFindBallPosition(player):
    """
    Should we spin to find the ball
    """
    if player.stateTime >= constants.BALL_OFF_SPIN_TIME:
        return True
    return False

def shouldKickAtPosition(player):
    """
    Is the ball close enough that we should kick
    """
    return (player.brain.ball.on and
            player.brain.ball.dist < constants.AT_POSITION_KICK_DIST)


def shouldRelocalize(player):
    my = player.brain.my
    return my.uncertX > constants.RELOC_UNCERT_XY_THRESH or \
        my.uncertY > constants.RELOC_UNCERT_XY_THRESH or \
        my.uncertH > constants.RELOC_UNCERT_THETA_THRESH

def isWellLocalized(player):
    my = player.brain.my
    return my.uncertX < constants.WELL_LOCED_UNCERT_XY and \
        my.uncertY > constants.WELL_LOCED_UNCERT_XY and \
        my.uncertH > constants.WELL_LOCED_UNCERT_THRESH
