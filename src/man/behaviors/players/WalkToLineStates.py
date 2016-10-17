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
        
    # get the array of field lines
    lines = player.brain.visionLines

    # get the first line in the array (I'm told that this is sufficient for
    # the purposes of this assignment)
    line_dist = lines(0).inner.r

    # if the distance to the line is greater than 20cm, continue walking
    if line_dist > 20:
        player.brain.nav.walk(0.3, 0, 0)

    # if the distance to the line is less than or equal to 20cm, stop
    else:
        return player.goNow('stop')

    # stay in this state until within 20cm of line
    return player.stay()

@superState('gameControllerResponder')
# stops the robot
def stop(player):
    player.brain.nav.stop()
    return player.stay()

@superState('gameControllerResponder')
def fallen(player):
    return player.stay()
