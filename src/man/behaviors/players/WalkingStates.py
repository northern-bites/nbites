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
        player.brain.interface.motionRequest.reset_odometry = True
        player.brain.interface.motionRequest.timestamp = int(player.brain.time * 1000)
    elif player.counter == 1:
        player.brain.nav.walkTo(RelRobotLocation(100,0,0),
                                speeds.SPEED_SEVEN)
    elif player.counter > 30 and player.brain.interface.motionStatus.standing:
        player.brain.nav.stand()

    return player.stay()

@superState('gameControllerResponder')
def gamePenalized(player):
    if player.firstFrame():
        player.brain.interface.motionRequest.reset_odometry = True
        player.brain.interface.motionRequest.timestamp = int(player.brain.time * 1000)
    elif player.counter == 1:
        player.brain.nav.walkTo(RelRobotLocation(0,0,90),
                                speeds.SPEED_SEVEN)
    elif player.counter > 30 and player.brain.interface.motionStatus.standing:
        player.brain.nav.stand()

    return player.stay()

@superState('gameControllerResponder')
def fallen(player):
    return player.stay()
