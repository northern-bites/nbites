#
# This file defines the states necessary for kick testing. Each method
# defines a state.
#

from ..headTracker import HeadMoves
from .. import SweetMoves

def gameInitial(player):
    if player.firstFrame():
        player.gainsOn()
        player.brain.nav.stand()
        player.brain.fallController.enabled = False
    return player.stay()

def gameReady(player):
    if player.firstFrame():
        player.brain.nav.stand()
        player.brain.fallController.enabled = False
    return player.stay()

def gameSet(player):
    if player.firstFrame():
        player.brain.nav.stand()
        player.brain.fallController.enabled = False
    return player.stay()

def gamePlaying(player):
    if player.firstFrame():
        player.brain.fallController.enabled = False
    return player.goLater('kick')

def gamePenalized(player):
    if player.firstFrame():
        player.brain.fallController.enabled = False

    return player.stay()

def kick(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.LEFT_STRAIGHT_KICK)

    return player.stay()
