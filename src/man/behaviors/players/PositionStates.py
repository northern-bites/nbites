import ChaseBallTransitions as transitions
import ChaseBallConstants as constants
from ..navigator import Navigator
from ..navigator import BrunswickSpeeds as speeds
from ..util import *
import noggin_constants as NogginConstants
from objects import Location, RelRobotLocation
from . import RoleConstants as roleConstants
from . import SharedTransitions

@superState('gameControllerResponder')
def positionReady(player):
    """
    Game Ready positioning
    """
    if player.firstFrame():
        if player.brain.gameController.ownKickOff and player.isKickingOff:
            print "Player role in PositionStates is", player.role
            player.kickoffPosition = roleConstants.ourKickoff
            print "kicking off, position is", player.kickoffPosition
        elif player.isKickingOff:
            player.kickoffPosition = roleConstants.theirKickoff

        if roleConstants.isChaser(player.brain.playerNumber):
          precision = Navigator.CLOSE_ENOUGH
        else:
          precision = Navigator.GRAINY

        player.brain.nav.goTo(player.kickoffPosition,
                              precision = precision,
                              speed = speeds.SPEED_SEVEN,
                              avoidObstacles = True,
                              fast = True, pb = False)

        player.brain.tracker.repeatWideSnapPan()

    return player.stay()