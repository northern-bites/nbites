"""
The fall controller, implemented as a hierarchical FSA.
The top level in player FSA.
"""

from .. import SweetMoves
from SharedTransitions import sameStateForNSeconds
import noggin_constants as nogginConstants
import FallControllerTransitions as transitions
from ..util import *

@defaultState('roleSwitcher')
# @ifSwitchNow(transitions.falling, 'falling')
def fallController(player):
    """
    Superstate for checking if we are falling.
    """
    pass

# @stay
# @ifSwitchNow(transitions.atRest, 'prepareForStandUp')
# def falling(player):
#     """
#     Protects robot as he falls. Stops him and kills stiffness.
#     """
#     if player.firstFrame():
#         player.brain.player.gainsOff()
#         player.brain.tracker.stopHeadMoves()
#         player.inKickingState = False
# 
# @switchLater('standingUp')
# def prepareForStandUp(player):
#     """
#     Reset motion providers and turn on stiffness before attempting stand up.
#     """
#     if player.firstFrame():
#         player.brain.interface.motionRequest.reset_providers = True
#         player.brain.interface.motionRequest.timestamp = int(player.brain.time*1000)
#         player.brain.player.gainsOn()
#         player.brain.tracker.setNeutralHead()
# 
# @stay
# def standingUp(player):
#     """
#     Attempts a standup.
#     """
#     if player.firstFrame():
#         if (player.brain.interface.fallStatus.on_front):
#             standingUp.move = SweetMoves.STAND_UP_FRONT
#         else:
#             standingUp.move = SweetMoves.STAND_UP_BACK
# 
#         player.brain.player.executeMove(standingUp.move)
# 
#     # Does not use ifSwitch because we need access to 'move' and 
#     # 'player.currentState'
#     if sameStateForNSeconds(SweetMoves.getMoveTime(standingUp.move))(player): 
#         return player.goLater(player.gameState)
