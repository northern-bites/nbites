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
    return player.goLater('wait')

def gamePenalized(player):
    if player.firstFrame():
        player.brain.fallController.enabled = False

    return player.goLater('restore')

def wait(player):
    if player.firstFrame():
        player.brain.nav.stand()

    if player.counter == 100:
        return player.goLater('dive')
    return player.stay()

def dive(player):
    if player.firstFrame():
        player.brain.tracker.performHeadMove(HeadMoves.OFF_HEADS)
        player.executeMove(SweetMoves.GOALIE_DIVE_RIGHT)

    return player.stay()

def restore(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.GOALIE_ROLL_OUT_RIGHT)

    return player.stay()
