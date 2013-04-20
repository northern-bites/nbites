#
# This file defines the states necessary for kick testing. Each method
# defines a state.
#

from ..headTracker import HeadMoves
from .. import SweetMoves

def gameInitial(player):
    if player.firstFrame():
        player.gainsOn()
        player.brain.fallController.enabled = False
    return player.stay()

def gameReady(player):
    if player.firstFrame():
        player.brain.fallController.enabled = False
    return player.goLater('standup')

def gameSet(player):
    if player.firstFrame():
        player.brain.fallController.enabled = False
    return player.goLater('standup')

def gamePlaying(player):
    if player.firstFrame():
        player.brain.fallController.enabled = False
    return player.goLater('standup')

def gamePenalized(player):
    if player.firstFrame():
        player.brain.fallController.enabled = False

    return player.goLater('standup')

def standup(player):
    if player.firstFrame():
        player.brain.nav.stand()

    if player.counter == 100:
        return player.goLater('kickStraight')
    return player.stay()

def kickStraight(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.LEFT_STRAIGHT_KICK)

    # if player.counter == 150:
    #     return player.goLater('restore')

    return player.stay()

def restore(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.GOALIE_SQUAT_STAND_UP)

    return player.stay()
