from ..headTracker import HeadMoves
from .. import SweetMoves
from ..util import *
import PMotion_proto


@superState('gameControllerResponder')
def gameInitial(player):
    if player.firstFrame():
        player.gainsOn()
        player.returningFromPenalty = False
    return player.stay()

@superState('gameControllerResponder')
def gameReady(player):
    return player.goNow("wave")

@superState('gameControllerResponder')
def gameSet(player):
    return player.goNow("highFive")

@superState('gameControllerResponder')
def gamePlaying(player):
    return player.goNow("celebrate")

@superState('gameControllerResponder')
def gamePenalized(player):
    return player.stay()

@superState('gameControllerResponder')
def fallen(player):
    player.inKickingState = False
    return player.stay()

@superState('gameControllerResponder')
def wave(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.WAVE_RIGHT)
    return player.stay()

@superState('gameControllerResponder')
def highFive(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.HIGH_FIVE_RIGHT)
    return player.stay()

@superState('gameControllerResponder')
def celebrate(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.CELEBRATE)
    return player.stay()