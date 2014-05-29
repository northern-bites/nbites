"""
An FSA for monitoring teammates and switching between roles on the field
implemented as a hierarchical FSA.

The second to top level in player FSA.
"""

import RoleSwitchingTransitions as transitions
import BoxPositionConstants as BPConstants
from ..util import *

@defaultState('gameControllerResponder')
@ifSwitchNow(transitions.chaserIsOut, 'switchRoles')
@superState('fallController')
def roleSwitcher(player):
    """
    Superstate for checking if we need to switch roles.
    """
    pass

def switchRoles(player):
    """
    State to decide who on the team should become the new chaser and switch accordingly.
    """
    BPConstants.setRoleConstants(player, player.openChaser)

    return player.goLater(player.gameState)

    # # Should I become the chaser?
    # for mate in player.brain.teamMembers:
    #     if (mate.active and mate.role != 1 and
    #        mate.role < player.role):
    #         # No, another player will do it, continue playing...
    #         print "We're not gonna switch!!!!"
    #         return player.goLater(player.gameState)

    # # Yes, become the chaser...
    # BPConstants.setRoleConstants(player, player.openChaser)

    # print "We are the chaser!"
    # # And continue playing...
    # return player.goLater(player.gameState)
