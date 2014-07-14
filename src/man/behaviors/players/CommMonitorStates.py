"""
An FSA for detecting communication problems and changing to the appropriate strategy.

The third to top level in player FSA.
"""

import CommMonitorTransitions as transitions
import RoleConstants

@defaultState('gameControllerResponder')
@superState('roleSwitcher')
def commMonitor(player):
    if player.commMode != 2 and transitions.awfulComm(player):
        player.role = player.playerNumber
        player.prevRoleConfig = RoleConstants.roleConfiguration
        RoleConstants.roleConfiguration = RoleConstants.cautious
        RoleConstants.oddDefenderBox = oddDefenderBoxCautious
        RoleConstants.evenDefenderBox = evenDefenderBoxCautious
        RoleConstants.setRoleConstants(player, player.role)
        player.roleSwitching = False
        player.commMode = 2
    elif player.commMode != 1 and transitions.mediocreComm(player):
        player.role = player.playerNumber
        if player.commMode == 2: 
            RoleConstants.roleConfiguration = player.prevRoleConfig
        RoleConstants.oddDefenderBox = defenderBox
        RoleConstants.evenDefenderBox = defenderBox
        RoleConstants.setRoleConstants(player, player.role)
        player.roleSwitching = False
        player.commMode = 1
    elif player.commMode != 0:
        player.role = player.playerNumber
        if player.commMode == 2: RoleConstants.roleConfiguration = player.prevRoleConfig
        RoleConstants.oddDefenderBox = defenderBox
        RoleConstants.evenDefenderBox = defenderBox
        RoleConstants.setRoleConstants(player, player.role)
        player.roleSwitching = True
        player.commMode = 0
