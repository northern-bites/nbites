"""
An FSA for responding to changes in the gameState according to gameController, 
implemented as a hierarchical FSA. 

The second to top level in player FSA.
"""

import noggin_constants as nogginConstants
from ..GameController import convertStateFormat, convertStateFormatPenaltyShots
from ..util import *

@defaultState('gameInitial')
@superState('fallController')
def gameControllerResponder(player):
    """
    Superstate for responding to gameState switches and penalties.
    See GameController.py for more information.
    """
    pass
    # TODO commented out until new positioning system is pulled
    # GC = player.brain.gameController
    # if GC.stateChanged:
    #     if GC.penalized:
    #         return player.goLater('gamePenalized')
    #     else:
    #         if GC.penaltyShots:
    #             return player.goLater(convertStateFormatPenaltyShots[GC.currentState])
    #         else:
    #             return player.goLater(convertStateFormat[GC.currentState])
