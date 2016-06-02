"""
An FSA for detecting communication problems and changing to the appropriate strategy.

The third to top level in player FSA.
"""

import CommMonitorTransitions as transitions
import RoleConstants
from ..util import *


@defaultState('gameControllerResponder')
@superState('roleSwitcher')
def commMonitor(player):

    if player.dropIn:
        pass

    
    if player.commMode == -1:
        pass
    elif player.commMode != 2 and transitions.awfulComm(player):
        print "Switched to awful comm mode!"
        # player.role = player.brain.playerNumber
        player.prevRoleConfig = RoleConstants.roleConfiguration
        RoleConstants.roleConfiguration = RoleConstants.spread
        RoleConstants.oddDefenderBox = RoleConstants.defenderBox
        RoleConstants.evenDefenderBox = RoleConstants.defenderBox
        RoleConstants.setRoleConstants(player, player.role)
        if not player.dropIn:
            player.roleSwitching = False
        player.commMode = 2

