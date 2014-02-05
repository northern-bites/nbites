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

def atRest(player):
    """
    We have fallen and rested on the ground for more than a second.
    """
    return sameStateForNSeconds(constants.WAIT_BEFORE_STANDUP)(player)
