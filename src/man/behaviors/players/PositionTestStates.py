from .. import SweetMoves

def gameInitial(player):
    return player.stay()

def gameReady(player):
    if player.firstFrame():
        player.stand()

    return player.stay()

def gameSet(player):
    if player.firstFrame:
        pass

    return player.stay()

def gamePlaying(player):
    if player.firstFrame:
        pass

    return player.stay()

def gamePenalized(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.SIT_POS)
        player.brain.tracker.stopHeadMoves()

    return player.stay()

def doneState(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.SIT_POS)
        player.brain.tracker.stopHeadMoves()

    return player.stay()
