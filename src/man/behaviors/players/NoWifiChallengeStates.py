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
    if player.firstFrame():
        player.brain.nav.stand()
    return player.stay()

@superState('gameControllerResponder')
def gameSet(player):
    return player.stay()

@superState('gameControllerResponder')
def gamePlaying(player):
    if player.firstFrame():
        player.goNow('waitingForNum')
    return player.stay()

@superState('gameControllerResponder')
def gamePenalized(player):
    return player.stay()

@superState('gameControllerResponder')
def fallen(player):
    player.inKickingState = False
    return player.stay()

@superState('gameControllerResponder')
def waitingForNum(player):
    if player.firstFrame():
        print "turnnnnnnnninnngngngngngnngng"
        player.brain.nav.walkTo(RelRobotLocation(0,0,90), speed = speeds.SPEED_EIGHT)
    return player.stay()

#     if player.firstFrame():
#          PHILNUMBER = 0.5 #the number phil gives me
#          degAngle = math.degrees(PHILNUMBER)
#     # #if player.brain.#THE NUMBER CHANGED AHHHH
#     player.goNow('turnBody')


# @superState('gameControllerResponder')
# def turnBody(player):
#     if player.firstFrame():
#         #player.brain.walkTo(0,0,degAngle)
#         player.brain.nav.walkTo(0,0,30)
#     return player.goNow(waitingForNum)
