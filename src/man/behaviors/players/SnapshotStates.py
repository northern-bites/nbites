from .. import SweetMoves
from ..headTracker import HeadMoves
from .. import StiffnessModes
from ..navigator import BrunswickSpeeds as speeds
from objects import RelRobotLocation
from ..navigator import Navigator

####Change these for picture taking####
FRAME_SAVE_RATE = 1
NUM_FRAMES_TO_SAVE = 150

def gameReady(player):
    if player.firstFrame():
        player.gainsOn()
        #player.executeMove(SweetMoves.INITIAL_POS)
    return player.stay()

def gameSet(player):
    if player.firstFrame():
        player.brain.nav.stand()

    return player.stay()

def gamePlaying(player):
    if player.firstFrame():
        player.brain.tracker.repeatBasicPan()

def gamePenalized(player):
#    if player.firstFrame():
#        player.executeMove(SweetMoves.SIT_POS)
#        player.brain.tracker.stopHeadMoves()
    return player.stay()

def doneState(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.SIT_POS)
        player.brain.tracker.stopHeadMoves()

#     if player.stateTime > 8.0:
#         shutoff = motion.StiffnessCommand(0.0)
#         player.brain.motion.sendStiffness(shutoff)

    return player.stay()

def gameInitial(player):
    return player.stay()
