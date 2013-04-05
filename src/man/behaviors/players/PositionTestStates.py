from .. import SweetMoves
from ..playbook import PBConstants
from objects import RelRobotLocation

def gameInitial(player):
    return player.stay()

def gameReady(player):
    if player.firstFrame():
        player.stand()

    return player.stay()

def gameSet(player):
    if player.counter == 1 or player.counter == 10:
        # Send two commands to check overwrite vs. enqueue
        #player.brain.nav.goTo(RelRobotLocation(50.0,0.0,0.0))
        player.brain.nav.performSweetMove(SweetMoves.LEFT_QUICK_STRAIGHT_KICK)

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



