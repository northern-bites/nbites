import time
from objects import RelRobotLocation
import noggin_constants as nogginConstants

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
        player.brain.tracker.repeatWidePanFixedPitch()
        if player.lastDiffState == 'gameInitial':
            player.initialDelayCounter = 0

    #HACK! TODO: this delay is to make sure the sensors get calibrated before
    #we start walking; find a way to query motion to see whether the sensors are
    #calibrated or not before starting
    if player.initialDelayCounter < 230:
        player.initialDelayCounter += 1
        return player.stay()

    # If the goalie were to move during ready, it should happen here.
    # If the goalie walks forward at start of game, need to check if a goal
    #  was just scored. If so, walk forward enough to require manual positioning.
    #  Otherwise, every goal accumulates forward walk, and goalie ends up out
    #  of position.

    return player.stay()

def gameSet(player):
    if player.firstFrame():
        player.brain.logger.startLogging()
        player.brain.nav.stand()
        player.gainsOn()
        player.brain.loc.resetBall()
        player.brain.tracker.trackBallFixedPitch()

    # For the goalie, reset loc every frame.
    # This way, garaunteed to have correctly set loc and be standing in that
    #  location for a frame before gamePlaying begins.
    player.brain.loc.resetLocTo(nogginConstants.FIELD_WHITE_LEFT_SIDELINE_X,
                                    nogginConstants.MIDFIELD_Y,
                                    nogginConstants.HEADING_RIGHT)

    return player.stay()

def gamePlaying(player):
    if player.firstFrame():
        player.gainsOn()
        player.brain.nav.stand()
        player.brain.tracker.trackBallFixedPitch()

    #if player.lastDiffState == 'gamePenalized':
        # Need to at least *try* to get back into goal.

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
    # step forward - NOPE, hacked out US open 2012
    if player.firstFrame():
        """player.brain.nav.walkTo(RelRobotLocation(15,0,0),
                                #player.brain.nav.CLOSE_ENOUGH,
                                (3,3,10),
                                #player.brain.nav.SLOW_SPEED)
                                0.2)

    # Just in case walkTo fails, eventually stop anyway
    if player.brain.nav.isStopped() or player.counter > 300:
        player.brain.nav.stop()
        return player.goNow('watch')"""

    return player.goLater('watch')

def watch(player):
    if player.firstFrame():
        player.brain.tracker.trackBallFixedPitch()
        if player.lastDiffState == 'kickBall':
            player.brain.nav.stand()

    #if player.brain.ball.dist < 100:
    #    player.executeMove(SweetMoves.GOALIE_SQUAT)
    #    return player.goLater('saveIt')

    return player.stay()

def kickBall(player):
    """
    Kick the ball
    """
    if player.firstFrame():
        player.brain.tracker.trackBallFixedPitch()
        if player.brain.ball.loc.relY < 0:
            kick = SweetMoves.RIGHT_BIG_KICK
        else:
            kick = SweetMoves.LEFT_BIG_KICK

        player.executeMove(kick)

    if player.counter > 10 and player.brain.nav.isStopped():
        return player.goLater('watch')

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
        # This is to stand up before a penalty is called.
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

        player.brain.tracker.trackBallFixedPitch()
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
