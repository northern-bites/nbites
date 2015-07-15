"""
Role switching states between other teams' robots for the Drop In player.
"""

import RoleSwitchingTransitions as transitions
import RoleConstants as constants
from ..util import *

@ifSwitchNow(transitions.chaserIsOut, 'switchRoles')
@defaultState('commMonitor')
@superState('fallController')
def roleSwitcher(player):
    """
    Superstate for checking if we need to switch roles.
    """

    pass

@superState('roleSwitcher')
def switchRoles(player):
    """
    State to decide who on the team should become the new chaser and switch accordingly.
    """

    if transitions.offenseMissing(player):
        constants.setRoleConstants(player, player.role)
    return player.goNow(player.gameState)

    if transitions.roleOverlap(player):
        constants.setRoleConstants(player,player.role)
        constants.setRoleConstants(player, role)
