from ..headTracker import HeadMoves
from .. import SweetMoves
from ..util import *
import PMotion_proto #how we communicate with C++ code
from ..navigator import Navigator
from ..navigator import BrunswickSpeeds as speeds
from objects import RobotLocation, RelRobotLocation

@superState('gameControllerResponder')
def gameInitial(player):
    if player.firstFrame():
        player.gainsOn()
        player.brain.nav.stand()
        # player.runfallController = False
    return player.stay()

@superState('gameControllerResponder')
def gameReady(player):
    if player.firstFrame():
        player.brain.nav.stand()
        #if we didn't include this, it would execute multiple times 
    return player.stay()

@superState('gameControllerResponder')
def gameSet(player):
    return player.stay()

@superState('gameControllerResponder')
def gamePlaying(player):
    return player.goNow('check')

@superState('gameControllerResponder')
def walk(player):
    if player.firstFrame():
        # player.brain.nav.callKickEngine(PMotion_proto.messages.Kick.M_Left_Chip_Shot)
        # player.executeMove(SweetMoves.CUTE_KICK_LEFT)
        #write walk step here
        dest = RelRobotLocation(0,0,0)
        player.brain.nav.walkTo(dest,
                                speeds.SPEED_FIVE)

    # return player.goLater('check')
    return player.stay(
)
@superState('gameControllerResponder')
def check(player):
    if player.firstFrame():
        # player.
        #check how far line is
        lines = player.brain.visionLine
        

    #if check < some distance, stop = player.stand() or something jk it's player.brain.nav.stand()
   
    return player.stay()


@superState('gameControllerResponder')
def fallen(player):
    return player.stay()
