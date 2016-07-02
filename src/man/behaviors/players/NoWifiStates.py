import math
import os

from .. import SweetMoves
from ..headTracker import HeadMoves
from .. import StiffnessModes
from ..navigator import BrunswickSpeeds as speeds
from objects import RelRobotLocation
from ..navigator import Navigator
from ..util import *
import PMotion_proto

# Nao
#        _________   
#       /         \   
#      /           \   )
#         0    0        )
#     \      .      /  )
#      \___________/
#


@superState('gameControllerResponder')
def gameInitial(player):
    if player.firstFrame():
        player.gainsOn()
        player.brain.nav.stand()
        player.runfallController = False
    return player.stay()

@superState('gameControllerResponder')
def gameReady(player):
    return player.stay()

@superState('gameControllerResponder')
def gameSet(player):
    return player.stay()

@superState('gameControllerResponder')
def gamePlaying(player):
    return player.goNow('waitForNum')

@superState('gameControllerResponder')
def gamePenalized(player):
    return player.stay()

@superState('gameControllerResponder')
def fallen(player):
    player.inKickingState = False
    return player.stay()

@superState('gameControllerResponder')
def pleaseTurn(player):
    if player.firstFrame():
        dest = RelRobotLocation(0,0,-player.currNum)        
        player.brain.nav.destinationWalkTo(dest, speeds.SPEED_SEVEN)
    else:
        dest = RelRobotLocation(0,0,-player.currNum)        
        player.brain.nav.updateDestinationWalkDest(dest)
    if ( player.brain.loc.h + 2 > -player.currNum) and (player.brain.loc.h - 2 < -player.currNum):
        print -player.currNum
        player.brain.nav.stand()
        player.executeMove(SweetMoves.POINT)
        return player.goLater('resetRobot')

    return player.stay()

@superState('gameControllerResponder')
def waitForNum(player):
    if ( player.currNum != player.brain.nowifi_angle ):
        player.currNum = player.brain.nowifi_angle
        print "NO WIFI ANGLE CHANGED TO ", player.currNum
        return player.goNow('justPoint')

    return player.stay()
@superState('gameControllerResponder')
def justPoint(player):
    if player.firstFrame():
        if player.currNum != 0:
            player.executeMove(SweetMoves.POINT)
    return player.goLater("waitForNum")

#DONT FORGET TO MAKE SURE INITIALIZED PLAYER.LAST NUM IS FINE

@superState('gameControllerResponder')
def resetRobot(player):
    if player.firstFrame():
        dest = RelRobotLocation(0,0,player.currNum)        
        player.brain.nav.destinationWalkTo(dest, speeds.SPEED_SEVEN)
    else:
        print player.brain.loc.h
        dest = RelRobotLocation(0,0,player.currNum)        
        player.brain.nav.updateDestinationWalkDest(dest)
    if( player.brain.loc.h + 2 > player.currNum) and (player.brain.loc.h - 2 < player.currNum):
        player.brain.nav.stand()
        return player.goNow('waitForNum')

    return player.stay()