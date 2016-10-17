from ..headTracker import HeadMoves
from ..navigator import Navigator
from objects import RobotLocation, RelRobotLocation
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
    return player.goNow('walkToLine')

@superState('gameControllerResponder')
def gamePenalized(player):
    return player.stay()

@superState('gameControllerResponder')
def walkToLine(player):
        
    lines = player.brain.visionLines
    line_dist = lines(0).inner.r

    if line_dist > 20:
        player.brain.nav.walk(0.3, 0, 0)
    else:
        player.brain.nav.stop()
        player.goNow('stop')

    return player.stay()

@superState('gameControllerResponder')
def stop(player):
    return player.stay()

@superState('gameControllerResponder')
def fallen(player):
    return player.stay()
