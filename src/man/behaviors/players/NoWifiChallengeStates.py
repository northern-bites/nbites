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
        player.goNow('pleaseTurn')
    return player.stay()

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
        print "turnnnnnnnninnngngngngngnngng"
            # player.brain.nav.walkTo(RelRobotLocation(0.0, 0.0, 20.0), speed = nav.FAST_SPEED)
        player.setWalk(0,0,15.0)
    return player.stay()
