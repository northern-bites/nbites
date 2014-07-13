"""
An FSA for detecting communication problems and changing strategies if necessary.

The third to top level in player FSA.
"""

import CommMonitorTransitions as transitions
import RoleConstants

@defaultState('gameControllerResponder')
@superState('roleSwitcher')
def commMonitor(player):
    if not player.badComm and transitions.checkForBadComm(player):
        player.badComm = True
        player.role = player.playerNumber
        player.prevRoleConfig = RoleConstants.roleConfiguration
        RoleConstants.roleConfiguration = RoleConstants.cautious
        RoleConstants.oddDefenderBox = oddDefenderBoxCautious
        RoleConstants.evenDefenderBox = evenDefenderBoxCautious
        RoleConstants.setRoleConstants(self, self.role)
        player.roleSwitching = False
    elif player.badComm and transitions.checkForGoodComm(player):
        player.badComm = False
        RoleConstants.roleConfiguration = player.prevRoleConfig
        RoleConstants.oddDefenderBox = defenderBox
        RoleConstants.evenDefenderBox = defenderBox
        RoleConstants.setRoleConstants(self, self.role)
        player.roleSwitching = True
