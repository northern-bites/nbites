import noggin_constants as nogginConstants
from SharedTransitions import sameStateForNSeconds

def currentlyFalling(player):
    """
    We are currently in the process of falling, not yet at rest on ground.
    """
    return (player.runFallController and
            player.brain.interface.fallStatus.falling)

def fallen(player):
    """
    We have fallen and are therefore currently on the ground.
    """
    return player.brain.interface.fallStatus.fallen

def fallenAndWaited(player):
    """
    We have fallen and rested on the ground for more than a second.
    """
    return (fallen(player) and sameStateForNSeconds(1)(player)) #TODO constant
