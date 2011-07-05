# Test class for landmark tracking localization system

import man.motion.SweetMoves as SweetMoves
from .. import NogginConstants
from man.motion import MotionConstants

def gameInitial(player):
    # Turn on gains and stand up.
    player.gainsOn()
    player.executeMove(SweetMoves.INITIAL_POS)

    return player.stay()

def gameReady(player):
    if player.firstFrame():
        player.gainsOn()
    return player.stay()

def gameSet(player):
    return player.stay()

def gamePlaying(player):
    if player.firstFrame():
        player.brain.tracker.orbitPan()
    return player.stay()

def gamePenalized(player):
    if player.firstFrame():
        player.brain.tracker.stopHeadMoves()
    return player.stay()
