from .. import SweetMoves
from ..headTracker import HeadMoves
from .. import StiffnessModes
from ..navigator import BrunswickSpeeds as speeds
from objects import RelRobotLocation
from ..navigator import Navigator
from ..util import *

### Change these for picture taking ###
FRAME_SAVE_RATE = 1
NUM_FRAMES_TO_SAVE = 150

@superState('gameControllerResponder')
def gameInitial(player):
    if player.firstFrame():
        player.returningFromPenalty = False
        player.stand()
        player.brain.tracker.lookToAngle(0)
    return player.stay()

@superState('gameControllerResponder')
def gameReady(player):
    if player.firstFrame():
        player.stand()
        player.brain.tracker.lookToAngle(0)
    return player.stay()

@superState('gameControllerResponder')
def gameSet(player):
    if player.firstFrame():
        player.stand()
        player.brain.tracker.lookToAngle(0)

    # player.brain.resetInitialLocalization()
    return player.stay()

@superState('gameControllerResponder')
def gamePlaying(player):
    if player.firstFrame():
        # @SNAPPAN-CHANGE
        player.brain.tracker.repeatWideSnapPan()

    # player.brain.resetInitialLocalization()
    return player.stay()

@superState('gameControllerResponder')
def gamePenalized(player):
    return player.stay()

@superState('gameControllerResponder')
def fallen(player):
    player.inKickingState = False
    return player.stay()
