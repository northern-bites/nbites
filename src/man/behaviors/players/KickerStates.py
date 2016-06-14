from ..headTracker import HeadMoves
from .. import SweetMoves
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
    return player.goNow('kick')

@superState('gameControllerResponder')
def gamePenalized(player):
    return player.stay()

@superState('gameControllerResponder')
def kick(player):
    if player.firstFrame():
        player.brain.nav.callKickEngine(PMotion_proto.messages.Kick.kickForwardRight)
        player.executeMove(SweetMoves.GOALIE_SQUAT)

    return player.stay()

@superState('gameControllerResponder')
def fallen(player):
    return player.stay()
