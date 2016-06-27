import math

from .. import SweetMoves
from ..headTracker import HeadMoves
from .. import StiffnessModes
from ..navigator import BrunswickSpeeds as speeds
from objects import RelRobotLocation
from ..navigator import Navigator
from ..util import *
import PMotion_proto

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
        player.brain.interface.motionRequest.reset_odometry = True
        player.brain.interface.motionRequest.timestamp = int(player.brain.time * 1000)
    elif player.counter == 1:
        player.setWalk(0,0,player.currNum)

    elif player.counter == 2:
        player.brain.nav.stand()
        player.executeMove(SweetMoves.POINT)
        player.lastNum = currNum
        return player.goNow('waitForNum')
    return player.stay()
@superState('gameControllerResponder')
def waitForNum(player):
    currNum = 15
    if (player.lastNum != currNum):
        return player.goNow('pleaseTurn')
    return player.stay()

#DONT FORGET TO MAKE SURE INITIALIZED PLAYER.LAST NUM IS FINE