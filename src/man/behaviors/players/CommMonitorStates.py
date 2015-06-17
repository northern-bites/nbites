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
    if player.commMode == -1:
        pass
    elif player.commMode != 0 and player.brain.game.have_remote_gc:
        print "Switched to good comm mode because we are on the GC BABAY!"
        player.role = player.brain.playerNumber
        if player.commMode == 2: 
            RoleConstants.roleConfiguration = player.prevRoleConfig
        RoleConstants.oddDefenderBox = RoleConstants.defenderBox
        RoleConstants.evenDefenderBox = RoleConstants.defenderBox
        # US Open Hack
        if player.brain.game:
            oppTeam = player.brain.game.team(1).team_number
        else:
            oppTeam = -1
        RoleConstants.setRoleConstants(player, player.role, oppTeam)
        player.roleSwitching = True
        player.commMode = 0
    elif not player.brain.game.have_remote_gc and player.commMode != 2 and transitions.awfulComm(player):
        print "Switched to awful comm mode!"
        player.role = player.brain.playerNumber
        player.prevRoleConfig = RoleConstants.roleConfiguration
        RoleConstants.roleConfiguration = RoleConstants.cautious
        RoleConstants.oddDefenderBox = RoleConstants.oddDefenderBoxCautious
        RoleConstants.evenDefenderBox = RoleConstants.evenDefenderBoxCautious
        # US Open Hack
        if player.brain.game:
            oppTeam = player.brain.game.team(1).team_number
        else:
            oppTeam = -1
        RoleConstants.setRoleConstants(player, player.role, oppTeam)
        player.roleSwitching = False
        player.commMode = 2
    elif not player.brain.game.have_remote_gc and player.commMode != 1 and transitions.mediocreComm(player):
        print "Switched to mediocre comm mode!"
        player.role = player.brain.playerNumber
        if player.commMode == 2: 
            RoleConstants.roleConfiguration = player.prevRoleConfig
        RoleConstants.oddDefenderBox = RoleConstants.defenderBox
        RoleConstants.evenDefenderBox = RoleConstants.defenderBox
        # US Open Hack
        if player.brain.game:
            oppTeam = player.brain.game.team(1).team_number
        else:
            oppTeam = -1
        RoleConstants.setRoleConstants(player, player.role, oppTeam)
        player.roleSwitching = False
        player.commMode = 1
    elif player.commMode != 0 and transitions.goodComm(player):
        print "Switched to good comm mode!"
        player.role = player.brain.playerNumber
        if player.commMode == 2: 
            RoleConstants.roleConfiguration = player.prevRoleConfig
        RoleConstants.oddDefenderBox = RoleConstants.defenderBox
        RoleConstants.evenDefenderBox = RoleConstants.defenderBox
        # US Open Hack
        if player.brain.game:
            oppTeam = player.brain.game.team(1).team_number
        else:
            oppTeam = -1
        RoleConstants.setRoleConstants(player, player.role, oppTeam)
        player.roleSwitching = True
        player.commMode = 0