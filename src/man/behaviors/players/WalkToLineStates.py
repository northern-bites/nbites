from ..headTracker import HeadMoves
from ..navigator import Navigator
from objects import RobotLocation, RelRobotLocation
from ..util import *
import PMotion_proto
import Vision_proto

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
    
    # get the array of field lines
    lines = player.brain.visionLines

    # get the first field line in the field lines array
    r = lines(0).inner.r

    # walk forward until the robot is 5cm from the line
    while r > 10:
        player.brain.nav.walkTo(RelRobotLocation(5, 0, 0))
        r = lines(0).inner.r

    # stop
    return player.stay()

@superState('gameControllerResponder')
def fallen(player):
    return player.stay()
