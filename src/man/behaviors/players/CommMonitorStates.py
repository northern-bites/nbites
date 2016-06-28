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

    # role switching off for drop in player
    if player.dropIn:
        return
    
    if player.commMode == -1:
        pass

    elif player.commMode != 0 and player.brain.game.have_remote_gc:
        
        if player.firstFrame():
            print "Switched to good comm mode because we are on the GC BABAY!"
            print "Player role switch is", str(player.roleSwitching)
        
        player.role = player.brain.playerNumber
    
        if player.commMode == 2: 
            RoleConstants.roleConfiguration = player.prevRoleConfig
        
        RoleConstants.oddDefenderBox = RoleConstants.defenderBox
        RoleConstants.evenDefenderBox = RoleConstants.defenderBox
        RoleConstants.setRoleConstants(player, player.role)
        player.roleSwitching = False # HACK switched this off for testing; TODO back on for Germany!
        player.commMode = 0

    elif not player.brain.game.have_remote_gc and player.commMode != 2 and transitions.awfulComm(player):
        
        if player.firstFrame():
            print "Switched to awful comm mode!"
            print "Player role switch is", str(player.roleSwitching)

        player.role = player.brain.playerNumber
        player.prevRoleConfig = RoleConstants.roleConfiguration
        RoleConstants.roleConfiguration = RoleConstants.spread
        RoleConstants.oddDefenderBox = RoleConstants.defenderBox
        RoleConstants.evenDefenderBox = RoleConstants.defenderBox
        RoleConstants.setRoleConstants(player, player.role)
        player.roleSwitching = False
        player.commMode = 2
    
    elif not player.brain.game.have_remote_gc and player.commMode != 1 and transitions.mediocreComm(player):

        if player.firstFrame():
            print "Switched to mediocre comm mode!"
            print "Player role switch is", str(player.roleSwitching)
        player.role = player.brain.playerNumber
    
        if player.commMode == 2: 
            RoleConstants.roleConfiguration = player.prevRoleConfig
    
        RoleConstants.oddDefenderBox = RoleConstants.defenderBox
        RoleConstants.evenDefenderBox = RoleConstants.defenderBox
        RoleConstants.setRoleConstants(player, player.role)
        player.roleSwitching = False
        player.commMode = 1

    elif player.commMode != 0 and transitions.goodComm(player):
        
        if player.firstFrame():
            print "Switched to good comm mode! ROLE SWITCHING SHOULD BE TURNED ON."
            print "Player role switch is", str(player.roleSwitching)   
        
        player.role = player.brain.playerNumber
        
        if player.commMode == 2: 
            RoleConstants.roleConfiguration = player.prevRoleConfig
        
        RoleConstants.oddDefenderBox = RoleConstants.defenderBox
        RoleConstants.evenDefenderBox = RoleConstants.defenderBox
        RoleConstants.setRoleConstants(player, player.role)
        player.roleSwitching = False # HACK TODO turn this back on for Germany
        player.commMode = 0
