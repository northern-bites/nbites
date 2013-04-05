#
# This file defines the states necessary for kick testing. Each method
# defines a state.
#

from ..headTracking import HeadMoves
from .. import SweetMoves

def gameInitial(player):
    if player.firstFrame():
        pass
        #player.gainsOn()
        #player.brain.fallController.enableFallProtection(False)
    return player.stay()

def gameReady(player):
    if player.firstFrame():
        pass
        #player.brain.fallController.enableFallProtection(False)
    return player.goLater('standup')

def gameSet(player):
    if player.firstFrame():
        pass
        #player.brain.fallController.enableFallProtection(False)
    return player.goLater('standup')

def gamePlaying(player):
    if player.firstFrame():
        pass
        #player.brain.fallController.enableFallProtection(False)
    return player.goLater('standup')

def gamePenalized(player):
    if player.firstFrame():
        pass
        #player.brain.fallController.enableFallProtection(False)
    return player.goLater('standup')

def standup(player):
    if player.firstFrame():
        #player.brain.tracker.setNeutralHead()
        #player.gainsOn()
        player.brain.nav.stand()

    if player.counter == 100:
        return player.goLater('kickStraight')
    return player.stay()

def kickStraight(player):
    if player.firstFrame():
        #print player.brain.ball.loc.relX, player.brain.ball.loc.relY
        player.executeMove(SweetMoves.LEFT_FAR_KICK)
    if player.brain.nav.isStopped() and player.counter > 100:
        return player.goLater('done')
    return player.stay()

def done(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.SIT_POS)
    return player.stay()
