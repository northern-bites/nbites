#
# Defines states for SweetMove testing
#

from .. import SweetMoves

def gameInitial(player):
    if player.firstFrame():
        player.gainsOn()
        player.stand()
    return player.stay()

def gameReady(player):
    return player.goLater('doMove')
def gameSet(player):
    return player.goLater('doMove')
def gamePlaying(player):
    return player.goLater('doMove')
def gamePenalized(player):
    return player.goLater('doMove')

def doMove(player):
    if player.firstFrame():
        player.gainsOn()
        player.executeMove(SweetMoves.LEFT_BIG_KICK)
        player.executeMove(SweetMoves.RIGHT_BIG_KICK)

    if player.counter == 1:
        return player.goLater('done')
    return player.stay()

def done(player):
    if player.firstFrame():
        return player.stay()
    return player.stay()
