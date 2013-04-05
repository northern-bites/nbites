from .. import SweetMoves
from ..playbook import PBConstants

def gameInitial(player):
    if player.firstFrame():
        player.stopWalking()
        player.gainsOn()
        player.zeroHeads()

    return player.stay()

def gameReady(player):
    if player.firstFrame():
        player.stand()
        player.brain.tracker.locPans()

    return player.goLater('playbookPosition')

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
