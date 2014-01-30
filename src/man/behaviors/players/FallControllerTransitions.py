import noggin_constants as nogginConstants
import FallControllerConstants as constants
from SharedTransitions import sameStateForNSeconds

def currentlyFalling(player):
    """
    We are currently in the process of falling or have fallen.
    """
    return (player.runFallController and
            (player.brain.interface.fallStatus.falling or
             fallen(player)))

def fallen(player):
    """
    We have fallen and are therefore currently on the ground.
    """
    return player.brain.interface.fallStatus.fallen

def fallenAndWaited(player):
    """
    We have fallen and rested on the ground for more than a second.
    """
    return (fallen(player) and 
            sameStateForNSeconds(constants.WAIT_BEFORE_STANDUP)(player))
