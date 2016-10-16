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

    # the minimum distance between the robot and one of
    # the field lines
    minDist = 1000

    # the angle of the field line relative to the robot
    minTheta = 0

    # cycle through the list of field lines and get the
    # one with the least distance (smallest r-value)
    for i in range(0, player.brain.vision.line_size()):
        r = lines(i).inner.r
        if r < minDist:
            minDist = r
            minTheta = lines(i).inner.t

    # walk toward the field line but stop 10cm short
    player.brain.nav.walk(r-10, 0, 0)

    return player.stay()

@superState('gameControllerResponder')
def fallen(player):
    return player.stay()
