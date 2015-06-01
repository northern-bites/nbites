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
    # US Open Hack
    if player.brain.game:
        oppTeam = player.brain.game.team(1).team_number
    else:
        oppTeam = -1
    roleConstants.setRoleConstants(player, player.openChaser, oppTeam)

    return player.goLater(player.gameState)
