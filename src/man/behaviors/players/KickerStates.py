#
# This file defines the states necessary for kick testing. Each method
# defines a state.
#

from ..headTracker import HeadMoves
from .. import SweetMoves
from ..util import *

@superState('gameController')
def gameInitial(player):
    if player.firstFrame():
        player.gainsOn()
        player.brain.nav.stand()
        player.runfallController = False
    return player.stay()

@superState('gameController')
def gameReady(player):
    if player.firstFrame():
        player.brain.nav.stand()
    return player.stay()

@superState('gameController')
def gameSet(player):
    return player.stay()

@superState('gameController')
def gamePlaying(player):
    return player.goNow('kick')

@superState('gameController')
def gamePenalized(player):
    return player.stay()

def kick(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.LEFT_STRAIGHT_KICK)

    return player.stay()
