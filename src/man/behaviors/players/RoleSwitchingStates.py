"""
An FSA for monitoring teammates and switching between roles on the field
implemented as a hierarchical FSA.

The second to top level in player FSA.
"""

import RoleSwitchingTransitions as transitions
import RoleConstants as roleConstants
from ..util import *

@defaultState('commMonitor')
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
    roleConstants.setRoleConstants(player, player.openChaser)

    return player.goLater(player.gameState)
