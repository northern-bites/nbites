from ..headTracker import HeadMoves
from ..navigator import Navigator
from ..navigator import BrunswickSpeeds as speeds
from objects import RobotLocation, RelRobotLocation
from ..util import *

@superState('gameControllerResponder')
def gameInitial(player):
    if player.firstFrame():
        player.gainsOn()
        player.brain.nav.stand()
    return player.stay()

@superState('gameControllerResponder')
def gameReady(player):
    return player.stay()

@superState('gameControllerResponder')
def gameSet(player):
    return player.stay()


@superState('gameControllerResponder')
def gamePlaying(player):
    if player.firstFrame():
        print "in firstFrame()"
        #first line seen
    line = player.brain.visionLines(0)

    if line.inner.r > 20:
        player.brain.nav.walk(.5,0,0)
    elif line.inner.r < 20 and line.inner.r > 15:
        print "line distance = ", line.inner.r
        player.brain.nav.stand()
        return player.goNow('doneWalking')

    return player.stay()

@superState('gameControllerResponder')
def doneWalking(player):
    return player.stay()

@superState('gameControllerResponder')
def gamePenalized(player):
    return player.stay()

@superState('gameControllerResponder')
def fallen(player):
    return player.stay()
