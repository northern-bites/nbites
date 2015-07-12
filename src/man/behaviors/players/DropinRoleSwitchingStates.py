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
    print "\nI think my current role is", player.role, "! \n"
    if player.brain. activeTeamMates() < 5:
        transitions.determineOpenRoles(player)
    return player.goNow(player.gameState)