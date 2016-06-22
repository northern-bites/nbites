import math

from .. import SweetMoves
from ..headTracker import HeadMoves
from .. import StiffnessModes
from ..navigator import BrunswickSpeeds as speeds
from objects import RelRobotLocation
from ..navigator import Navigator
from ..util import *


@superState('gameControllerResponder')
def gameInitial(player):
    if player.firstFrame():
        player.returningFromPenalty = False
        player.brain.tracker.lookToAngle(0)
    return player.stay()

@superState('gameControllerResponder')
def gameReady(player):
    if player.firstFrame():
        player.brain.tracker.lookToAngle(0)
    return player.stay()

@superState('gameControllerResponder')
def gameSet(player):
    if player.firstFrame():
        player.brain.tracker.lookToAngle(0)
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
    #if player.firstFrame():
    #     PHILNUMBER = 0.5 #the number phil gives me
    #     degAngle = math.degrees(PHILNUMBER)
    # #if player.brain.#THE NUMBER CHANGED AHHHH
    player.goNow('turnBody')


@superState('gameControllerResponder')
def turnBody(player):
    if player.firstFrame():
        #player.brain.walkTo(0,0,degAngle)
        player.brain.nav.walkTo(0,0,30)
