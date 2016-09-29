from ..headTracker import HeadMoves
from ..navigator import Navigator
from ..navigator import BrunswickSpeeds as speeds
from objects import RobotLocation, RelRobotLocation
from ..util import *
from ..Say import *
import math

@superState('gameControllerResponder')
def gameInitial(player):
    if player.firstFrame():
        say(Say.IN_DEBUG, "Put me in front of a line and switch me to game ready!")
        player.gainsOn()
        player.brain.nav.stand()
    return player.stay()

@superState('gameControllerResponder')
def gameReady(player):
    if player.firstFrame():
        print()
        player.brain.lineDistance = player.brain.visionLines(0).inner.r
        distance = player.brain.lineDistance
        print(distance)
        if distance != 0.0:
            say(Say.IN_DEBUG, "I see a line about " + str(math.floor(distance)) + " centimeters away.")
        else:
            say(Say.IN_DEBUG, "I don't see a line.")
        
    return player.stay()

@superState('gameControllerResponder')
def gameSet(player):
    if player.firstFrame():
        distance = player.brain.lineDistance
        print(distance)
        player.brain.interface.motionRequest.reset_odometry = True
        player.brain.nav.goTo(RelRobotLocation(player.brain.lineDistance, 0, 0))
    # print(player.brain.interface.motionStatus)
    return player.stay()

@superState('gameControllerResponder')
def gamePlaying(player):
    if player.firstFrame():
        player.brain.interface.motionRequest.reset_odometry = True
        player.brain.interface.motionRequest.timestamp = int(player.brain.time * 1000)
    elif player.counter == 1:
        player.brain.nav.goTo(RelRobotLocation(100,0,0),
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
