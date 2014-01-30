import noggin_constants as nogginConstants

def gcChangedStateNoPKs(player):
    """
    The game controller changed the state.
    It is not time for PKs.
    """
    gameState = player.brain.interface.gameState
    return (player.gameState != gameState and
            gameState.secondary_state != STATE2_PENALTYSHOOT)

def gcChangedStatePKs(player):
    """
    The game controller changed the state.
    It is time for PKs.
    """
    gameState = player.brain.interface.gameState
    return (player.gameState != gameState and
            gameState.secondary_state == STATE2_PENALTYSHOOT)

def penalized(player):
    """
    We have been penalized.
    """
    gameState = player.brain.interface.gameState
    return (gameState.team(player.brain.teamNumber).
            player(player.brain.playerNumber-1).penalty)
