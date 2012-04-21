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
    # Pan for ball or stare if ball is on vision
    if player.firstFrame():
        player.brain.tracker.trackBall()
        if player.lastDiffState == 'gamePenalized':
            player.brain.nav.stand()

    # if ball comes close enough, kick it away.
    if player.brain.ball.vis.framesOn > 5 and \
            player.brain.ball.vis.dist < 18:
        return player.goLater('kickBall')

    return player.stay()

def kickBall(player):
    """
    Kick the ball
    """
    if player.firstFrame():
        player.brain.tracker.trackBall()
        if player.brain.ball.loc.relY < 0:
            kick = SweetMoves.RIGHT_BIG_KICK
        else:
            kick = SweetMoves.LEFT_BIG_KICK

        player.executeMove(kick)

    if player.counter > 10 and player.brain.nav.isStopped():
        return player.goLater('gamePlaying')

    return player.stay()

