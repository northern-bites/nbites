import man.motion.SweetMoves as SweetMoves
import man.motion.HeadMoves as HeadMoves
import man.motion.StiffnessModes as StiffnessModes
from ..navigator import BrunswickSpeeds as speeds
from objects import RelRobotLocation
from ..navigator import Navigator

def gameReady(player):
    if player.firstFrame():
        player.gainsOn()
    return player.stay()

def gameSet(player):
    if player.firstFrame():
        player.gainsOn()
        player.brain.nav.stand()
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

def gameInitial(player):
    return player.stay()

def gamePlaying(player):
    # This is where the magic happens!

    # Pan for ball or stare if ball is on vision
    player.brain.tracker.trackBall()

    return player.stay()
