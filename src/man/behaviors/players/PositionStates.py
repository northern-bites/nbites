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
            player.kickoffPosition = roleConstants.ourKickoff
        elif player.isKickingOff:
            player.kickoffPosition = roleConstants.theirKickoff

        player.brain.nav.goTo(player.kickoffPosition,
                              precision = Navigator.GRAINY ,
                              speed = speeds.SPEED_SEVEN,
                              avoidObstacles = True,
                              fast = True, pb = False)

        player.brain.tracker.repeatWideSnapPan()

    return player.stay()