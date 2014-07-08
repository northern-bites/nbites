from ..headTracker import HeadMoves
from ..navigator import Navigator
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
        player.brain.nav.walkTo(RelRobotLocation(100,0,0),
                                Navigator.QUICK_SPEED)

    return player.stay()

@superState('gameControllerResponder')
def gamePenalized(player):
    if player.firstFrame():
        player.brain.interface.motionRequest.reset_odometry = True
        player.brain.interface.motionRequest.timestamp = int(player.brain.time * 1000)
        player.brain.nav.stand()

    return player.stay()

@superState('gameControllerResponder')
def fallen(player):
    return player.stay()
