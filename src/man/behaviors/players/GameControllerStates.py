"""
An FSA for responding to changes in the gameState according to gameController, 
implemented as a hierarchical FSA. 

The fourth to top level in player FSA.
"""

import noggin_constants as nogginConstants
from ..GameController import convertStateFormat, convertStateFormatPenaltyShots
from ..util import *

@defaultState('gameInitial')
@superState('commMonitor')
def gameControllerResponder(player):
    """
    Superstate for responding to gameState switches and penalties.
    See GameController.py for more information.
    """
    GC = player.brain.gameController
    if GC.stateChanged:
        if GC.penalized:
            player.gameState = 'gamePenalized'
            return player.goLater('gamePenalized')
        else:
            if GC.penaltyShots:
                player.gameState = convertStateFormatPenaltyShots[GC.currentState]
                return player.goLater(convertStateFormatPenaltyShots[GC.currentState])
            else:
                player.gameState = convertStateFormat[GC.currentState]
                return player.goLater(convertStateFormat[GC.currentState])
