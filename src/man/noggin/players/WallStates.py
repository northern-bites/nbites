import man.motion.SweetMoves as SweetMoves
import man.motion.HeadMoves as HeadMoves
from objects import RelRobotLocation
from ..navigator import Navigator

def gameReady(player):
    if player.firstFrame():
        player.gainsOn()
        player.brain.nav.stand()
    return player.stay()

def gameSet(player):
    return player.stay()

def gamePenalized(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.SIT_POS)
        player.penalizeHeads()
    return player.stay()

def doneState(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.SIT_POS)
    return player.stay()

def gameInitial(player):
    return player.stay()

def gamePlaying(player):
    # This is where the magic happens!

    # If coming from penalized, stand back up.
    if player.lastDiffState == 'gamePenalized':
        player.brain.nav.stand()

    # Pan for ball or stare if ball is on vision
    player.brain.tracker.trackBall()

    return player.goLater('pace')
