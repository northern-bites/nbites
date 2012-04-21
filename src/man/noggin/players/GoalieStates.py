import time
from objects import RelRobotLocation

import man.motion.SweetMoves as SweetMoves

def gameInitial(player):
    if player.firstFrame():
        player.brain.nav.stop()
        player.gainsOn()
        player.zeroHeads()
        player.GAME_INITIAL_satDown = False

    elif (player.brain.nav.isStopped() and not player.GAME_INITIAL_satDown
          and not player.motion.isBodyActive()):
        player.GAME_INITIAL_satDown = True
        player.executeMove(SweetMoves.SIT_POS)

    return player.stay()

def gameReady(player):
    if player.firstFrame():
        player.gainsOn()
        player.brain.nav.stand()
        player.brain.tracker.locPans()
        if player.lastDiffState == 'gameInitial':
            player.initialDelayCounter = 0

    if player.initialDelayCounter < 230:
        player.initialDelayCounter += 1
        return player.stay()

    return player.stay()

def gameSet(player):
    if player.firstFrame():
        player.brain.logger.startLogging()
        player.brain.nav.stand()
        player.gainsOn()
        player.brain.loc.resetBall()
        player.brain.tracker.trackBall()

        player.brain.resetGoalieLocalization()

    return player.stay()

def gamePlaying(player):
    if player.firstFrame():
        player.gainsOn()
        player.brain.nav.stand()
        player.brain.tracker.trackBall()
    return player.goLater('position')

def gamePenalized(player):
    if player.firstFrame():
        player.brain.logger.stopLogging()
        player.inKickingState = False
        player.stopWalking
        player.executeMove(SweetMoves.SIT_POS)
        player.penalizeHeads()

    return player.stay()

def fallen(player):
    return player.stay()

def gameFinished(player):
    if player.firstFrame():
        player.stopWalking()
        player.zeroHeads()
        player.GAME_FINISHED_satDown = False
        return player.stay()

    if (player.brain.nav.isStopped() and not player.GAME_FINISHED_satDown
        and not player.motion.isBodyActive()):
        player.GAME_FINISHED_satDown = True
        player.executeMove(SweetMoves.SIT_POS)
        return player.stay()

    if not player.motion.isBodyActive() and player.GAME_FINISHED_satDown:
        player.gainsOff()
    return player.stay()

def position(player):
    # step forward
    if player.firstFrame():
        player.brain.nav.walkTo(RelRobotLocation(15,0,0),
                                #player.brain.nav.CLOSE_ENOUGH,
                                (3,3,10),
                                #player.brain.nav.SLOW_SPEED)
                                0.2)

    # Just in case walkTo fails, eventually stop anyway
    if player.brain.nav.isStopped() or player.counter > 300:
        player.brain.nav.stop()
        return player.goNow('watch')

    return player.stay()

def watch(player):
    if player.firstFrame():
        player.brain.tracker.trackBall()

    if player.brain.ball.dist < 100:
        player.executeMove(SweetMoves.GOALIE_SQUAT)
        return player.goLater('saveIt')
    return player.stay()

def saveIt(player):
    if player.firstFrame():
        player.isSaving = False
    if (not player.motion.isBodyActive() and not player.isSaving):
        player.squatTime = time.time()
        player.isSaving = True
        return player.stay()
    if player.isSaving:
        stopTime = time.time()
        if (stopTime - player.squatTime > 4):
            player.executeMove(SweetMoves.GOALIE_SQUAT_STAND_UP)
            return player.goLater('upUpUP')
    return player.stay()

def upUpUP(player):
    if player.firstFrame():
        player.upDelay = 0

    if player.motion.isBodyActive():
        return player.stay()
    elif player.upDelay < 60:
        player.upDelay += 1
        return player.stay()
    else:
        return player.goLater('watch')
    return player.stay()

############# PENALTY SHOOTOUT #############


def penaltyShotsGameSet(player):
    if player.firstFrame():
        player.gainsOn()
        player.stopWalking()
        player.stand()
        player.brain.loc.resetBall()

        player.brain.tracker.trackBall()
        player.initialDelayCounter = 0

    if player.initialDelayCounter < 230:
        player.initialDelayCounter += 1
        return player.stay()

    return player.stay()

def penaltyShotsGamePlaying(player):
    if player.firstFrame():
        player.gainsOn()
        player.stand()

    return player.goLater('position')
    #return player.goLater('watch')
