"""
Game controller states for pBrunswick, our soccer player.
"""

import noggin_constants as nogginConstants
from math import fabs
from ..util import *
from .. import SweetMoves

### NORMAL PLAY ###
@superState('gameControllerResponder')
def gameInitial(player):
    """
    Ensure we are sitting down and head is snapped forward.
    In the future, we may wish to make the head move a bit slower here.
    """
    if player.firstFrame():
        player.inKickingState = False
        player.brain.fallController.enabled = False
        player.gainsOn()
        player.stand()
        player.zeroHeads()
        #Reset localization to proper starting position by player number.
        #Locations are defined in the wiki.
        player.brain.resetInitialLocalization()
        player.lastStiffStatus = True

    # If stiffnesses were JUST turned on, then stand up.
    if player.lastStiffStatus == False and player.brain.interface.stiffStatus.on:
        player.stand()
    # Remember last stiffness.
    player.lastStiffStatus = player.brain.interface.stiffStatus.on

    return player.stay()

@superState('gameControllerResponder')
def gameReady(player):
    """
    Stand up, and pan for localization, and walk to kicking off positions.
    """
    if player.firstFrame():
        player.inKickingState = False
        player.brain.fallController.enabled = True
        player.brain.nav.stand()
        player.brain.tracker.repeatWidePan()
        player.timeReadyBegan = player.brain.time

    # Reset localization to proper starting position by player number.
    # Locations are defined in the wiki.
        if player.lastDiffState == 'gameInitial':
            player.brain.resetInitialLocalization()

        if player.wasPenalized:
            player.wasPenalized = False
            return player.goNow('afterPenalty')

    # Wait until the sensors are calibrated before moving.
    if not player.brain.motion.calibrated:
        return player.stay()

    return player.goNow('positionReady')

@superState('gameControllerResponder')
def gameSet(player):
    """
    Fixate on the ball, or scan to look for it
    """
    if player.firstFrame():
        player.inKickingState = False
        player.brain.fallController.enabled = False
        player.brain.nav.stand()
        player.brain.tracker.performBasicPan()

    elif player.brain.tracker.isStopped():
        player.brain.tracker.trackBall()

    # Wait until the sensors are calibrated before moving.
    if not player.brain.motion.calibrated:
        return player.stay()

    return player.stay()

@superState('gameControllerResponder')
def gamePlaying(player):
    if player.firstFrame():
        player.inKickingState = False
        player.brain.fallController.enabled = True
        player.brain.nav.stand()
        player.brain.tracker.trackBall()

    # TODO without pb, is this an issue?
    # if (player.lastDiffState == 'afterPenalty' and
    #     player.brain.play.isChaser()):
    #     # special behavior case
    #     return player.goNow('postPenaltyChaser')
    # Wait until the sensors are calibrated before moving.

    if player.wasPenalized:
        player.wasPenalized = False
        return player.goNow('afterPenalty')

    if not player.brain.motion.calibrated:
        return player.stay()

    if (player.isKickingOff and player.brain.gameController.ownKickOff and
        player.brain.gameController.timeSincePlaying < 10):
        player.shouldKickOff = True
        return player.goNow('approachBall')
    elif player.brain.gameController.timeSincePlaying < 10:
        return player.goNow('waitForKickoff')
    return player.goNow('positionAtHome')

@superState('gameControllerResponder')
def gameFinished(player):
    """
    Ensure we are sitting down and head is snapped forward.
    In the future, we may wish to make the head move a bit slower here
    """
    if player.firstFrame():
        player.inKickingState = False
        player.brain.fallController.enabled = False
        player.stopWalking()
        player.zeroHeads()
        player.executeMove(SweetMoves.SIT_POS)

    if player.brain.nav.isStopped():
        player.gainsOff()

    return player.stay()

@superState('gameControllerResponder')
def gamePenalized(player):
    if player.firstFrame():
        player.inKickingState = False
        player.brain.fallController.enabled = False
        player.stand()
        player.penalizeHeads()
        player.wasPenalized = True

    return player.stay()

@superState('gameControllerResponder')
def waitForKickoff(player):
    if player.firstFrame():
        waitForKickoff.ballRelX = player.brain.ball.rel_x
        waitForKickoff.ballRelY = player.brain.ball.rel_y

    if (player.brain.gameController.timeSincePlaying > 10 or
        fabs(player.brain.ball.rel_x - waitForKickoff.ballRelX) > 10 or
        fabs(player.brain.ball.rel_y - waitForKickoff.ballRelY) > 10):
        return player.goNow('watchForBall')

    return player.stay()

waitForKickoff.ballRelX = "the relX position of the ball when we started"
waitForKickoff.ballRelY = "the relY position of the ball when we started"

### PENALTY KICK PLAY ###
@superState('gameControllerResponder')
def penaltyShotsGameSet(player):
    if player.firstFrame():
        player.gainsOn()
        player.stand()
        player.inKickingState = False
        player.brain.fallController.enabled = False
        player.brain.tracker.trackBall()

    # Wait until the sensors are calibrated before moving.
    if not player.brain.motion.calibrated:
        return player.stay()

    # Reset loc properly every frame
    player.brain.resetPenaltyKickLocalization()

    return player.stay()

@superState('gameControllerResponder')
def penaltyShotsGamePlaying(player):
    if player.firstFrame():
        player.stand()
        player.brain.fallController.enabled = True
        player.inKickingState = False
        player.shouldKickOff = False
        player.penaltyKicking = True
        player.brain.resetPenaltyKickLocalization()

    # Wait until the sensors are calibrated before moving.
    if (not player.brain.motion.calibrated):
        return player.stay()

    return player.goNow('prepareForPenaltyKick')


@superState('gameControllerResponder')
def fallen(player):
    player.inKickingState = False
    return player.stay()
