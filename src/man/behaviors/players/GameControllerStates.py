"""
The game controller, implemented as a hierarchical FSA.
The second to top level in player FSA.
"""

import noggin_constants as nogginConstants
import GameControllerTransitions as transitions
from GameControllertransitions import convertStateFormat
from GameControllertransitions import convertStateFormatPenaltyShots
from ..util import *

@default('gameInitial')
@ifSwitch(transitions.penalized, 'gamePenalized', False)
@ifSwitch(transitions.gcChangedStatePKs, 
          convertStateFormatPenaltyShots[player.brain.interface.gameState], 
          False)
@ifSwitch(transitions.gcChangedStateNoPKs, 
          convertStateFormat[player.brain.interface.gameState], 
          False)
@superState('fallController')
def gameController(player):
    """
    Superstate for checking gameState switches and penalties.
    (1) gcChangedStateNoPKs changes between GI, GR, GS, GP automatically.
    (2) gcChangedStatePKs changes between penalty kick states (set, playing) 
    automatically.
    (3) penalized puts the player in 'gamePenalized' state.
    """
    pass
