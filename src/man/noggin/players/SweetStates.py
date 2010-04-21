#
# Defines states for SweetMove testing
#

import man.motion.SweetMoves as SweetMoves

def gameInitial(player):
    player.gainsOn()
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
        player.executeMove(SweetMoves.ZERO_POS)

    if player.counter == 1:
        return player.goLater('done')
    return player.stay()

def done(player):
    if player.firstFrame():
        return player.stay()
    return player.stay()
