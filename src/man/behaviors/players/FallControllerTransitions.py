import noggin_constants as nogginConstants
import FallControllerConstants as constants
from SharedTransitions import sameStateForNSeconds

def falling(player):
    """
    We are currently in the process of falling or have fallen.
    """
    return (player.runFallController and
            (player.brain.interface.fallStatus.falling or
             player.brain.interface.fallStatus.fallen))

def fallen(player):
    """
    We have fallen according to guardian.
    """
    return player.brain.interface.fallStatus.fallen

def atRest(player):
    """
    We have rested on the ground, so as not to attempt standing up too quickly.
    """
    return sameStateForNSeconds(constants.WAIT_BEFORE_STANDUP)(player)
