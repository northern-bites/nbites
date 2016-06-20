"""
An FSA for detecting communication problems and changing to the appropriate strategy.

The third to top level in player FSA.
"""

import CommMonitorTransitions as transitions
import RoleConstants
from ..util import *

# TODO stop defaulting to player number everywhere
# This is where we can switch our strategies based on comm quality
# Should add time stamps for how we switch strategies but also how we determine comm quality

@defaultState('gameControllerResponder')
@superState('roleSwitcher')
def commMonitor(player):
    
    if player.commMode == -1:
        pass

    elif player.commMode != 0 and player.brain.game.have_remote_gc:
        
        print "Switched to good comm mode because we are on the GC BABAY!"
        print "Role switching WOULD be set to true"
        
        player.role = player.brain.playerNumber
    
        if player.commMode == 2: 
            RoleConstants.roleConfiguration = player.prevRoleConfig
        
        RoleConstants.oddDefenderBox = RoleConstants.defenderBox
        RoleConstants.evenDefenderBox = RoleConstants.defenderBox
        RoleConstants.setRoleConstants(player, player.role)
        player.roleSwitching = False
        player.commMode = 0

    elif not player.brain.game.have_remote_gc and player.commMode != 2 and transitions.awfulComm(player):
        
        print "Switched to awful comm mode!"
        
        player.role = player.brain.playerNumber
        player.prevRoleConfig = RoleConstants.roleConfiguration
        RoleConstants.roleConfiguration = RoleConstants.spread
        RoleConstants.oddDefenderBox = RoleConstants.defenderBox
        RoleConstants.evenDefenderBox = RoleConstants.defenderBox
        RoleConstants.setRoleConstants(player, player.role)
        player.roleSwitching = False
        player.commMode = 2
    
    elif not player.brain.game.have_remote_gc and player.commMode != 1 and transitions.mediocreComm(player):

        print "Switched to mediocre comm mode!"
    
        player.role = player.brain.playerNumber
    
        if player.commMode == 2: 
            RoleConstants.roleConfiguration = player.prevRoleConfig
    
        RoleConstants.oddDefenderBox = RoleConstants.defenderBox
        RoleConstants.evenDefenderBox = RoleConstants.defenderBox
        RoleConstants.setRoleConstants(player, player.role)
        player.roleSwitching = False
        player.commMode = 1

    elif player.commMode != 0 and transitions.goodComm(player):
        
        print "Switched to good comm mode! ROLE SWITCHING SHOULD BE TURNED ON."
        
        player.role = player.brain.playerNumber
        
        if player.commMode == 2: 
            RoleConstants.roleConfiguration = player.prevRoleConfig
        
        RoleConstants.oddDefenderBox = RoleConstants.defenderBox
        RoleConstants.evenDefenderBox = RoleConstants.defenderBox
        RoleConstants.setRoleConstants(player, player.role)
        player.roleSwitching = False
        player.commMode = 0