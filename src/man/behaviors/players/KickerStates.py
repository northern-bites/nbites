#
# This file defines the states necessary for kick testing. Each method
# defines a state.
#

from ..headTracker import HeadMoves
from .. import SweetMoves

def gameInitial(player):
    if player.firstFrame():
        player.brain.fallController.enabled = False
        #player.gainsOn()
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
        #print player.brain.ball.loc.relX, player.brain.ball.loc.relY
        print "TEST! TEST!"
        player.executeMove(SweetMoves.STAND_UP_BACK)
    if player.brain.nav.isStopped() and player.counter > 100:
        return player.goLater('done')
    return player.stay()

def done(player):
    if player.firstFrame():
        pass
#        player.executeMove(SweetMoves.SIT_POS)
    return player.stay()
