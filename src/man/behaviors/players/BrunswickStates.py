"""
Game controller states for pBrunswick, our soccer player.
"""

import noggin_constants as nogginConstants
from ..util import *

### NORMAL PLAY ###
@superState('gameController')
def gameInitial(player):
    """
    Ensure we are sitting down and head is snapped forward.
    In the future, we may wish to make the head move a bit slower here
    Also, in the future, gameInitial may be responsible for turning off the gains
    """
    if player.firstFrame():
        player.inKickingState = False
        player.gameState = player.currentState
        player.runfallController = False
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

@superState('gameController')
def gameReady(player):
    """
    Stand up, and pan for localization
    """
    if player.firstFrame():
        player.inKickingState = False
        player.runFallController = True
        player.gameState = player.currentState
        player.brain.nav.stand()
        player.brain.tracker.repeatWidePan()
        player.timeReadyBegan = player.brain.time

    # Reset localization to proper starting position by player number.
    # Locations are defined in the wiki.
        if player.lastDiffState == 'gameInitial':
            player.brain.resetInitialLocalization()

        if player.lastDiffState == 'gamePenalized':
            return player.goNow('afterPenalty')

    # Wait until the sensors are calibrated before moving.
    if not player.brain.motion.calibrated:
        return player.stay()

    return player.goLater('playbookPosition')

@superState('gameController')
def gameSet(player):
    """
    Fixate on the ball, or scan to look for it
    """
    if player.firstFrame():
        player.inKickingState = False
        player.runFallController = False
        player.gameState = player.currentState
        player.brain.nav.stand()
        player.brain.tracker.performBasicPan()

        # TODO this is broken, replace?
        if player.lastDiffState == 'gamePenalized':
            # We KNOW that we've been manually positioned.
            pass
            #player.brain.resetSetLocalization()

    elif player.brain.tracker.isStopped():
        player.brain.tracker.trackBall()

    # Wait until the sensors are calibrated before moving.
    if not player.brain.motion.calibrated:
        return player.stay()

    return player.stay()

@superState('gameController')
def gamePlaying(player):
    if player.firstFrame():
        player.inKickingState = False
        player.runFallController = True
        player.gameState = player.currentState
        player.brain.nav.stand()
        player.brain.tracker.trackBall()
        # TODO clean up
        if player.lastDiffState == 'gamePenalized':
            print 'Player coming out of penalized state after ' + str(player.lastStateTime) + ' seconds in last state'
            if player.lastStateTime > 5:
                return player.goNow('afterPenalty')
        # TODO move this check into postPenaltyChaser and have that come back here for non-chasers.
        if (player.lastDiffState == 'afterPenalty' and
            player.brain.play.isChaser()):
            # special behavior case
            return player.goNow('postPenaltyChaser')

    # Wait until the sensors are calibrated before moving.
    if not player.brain.motion.calibrated:
        return player.stay()

    if player.usingBoxPositions:
        return player.goNow('positionAtHome')

    roleState = player.getRoleState()
    return player.goNow(roleState)

@superState('gameController')
def gameFinished(player):
    """
    Ensure we are sitting down and head is snapped forward.
    In the future, we may wish to make the head move a bit slower here
    """
    if player.firstFrame():
        player.inKickingState = False
        player.runFallController = False
        player.gameState = player.currentState
        player.stopWalking()
        player.zeroHeads()
        player.executeMove(SweetMoves.SIT_POS)

    if player.brain.nav.isStopped():
        player.gainsOff()

    return player.stay()

@superState('gameController')
def gamePenalized(player):
    if player.firstFrame():
        player.inKickingState = False
        player.runFallController = False
        player.gameState = player.currentState
        player.stand()
        player.penalizeHeads()

    return player.stay()

### PENALTY KICK PLAY ###
@superState('gameController')
def penaltyShotsGameSet(player):
    if player.firstFrame():
        player.stand()
        player.gameState = player.currentState
        player.inKickingState = False
        player.runFallController = False
        player.brain.tracker.trackBall()

    # Wait until the sensors are calibrated before moving.
    if not player.brain.motion.calibrated:
        return player.stay()

    # Reset loc properly every frame
    player.brain.resetPenaltyKickLocalization()

    return player.stay()

@superState('gameController')
def penaltyShotsGamePlaying(player):
    if player.firstFrame():
        player.stand()
        player.gameState = player.currentState
        player.runFallController = True
        player.inKickingState = False
        player.shouldKickOff = False
        player.penaltyKicking = True
        player.brain.resetPenaltyKickLocalization()

    # Wait until the sensors are calibrated before moving.
    if (not player.brain.motion.calibrated):
        return player.stay()

    return player.goNow('chase')
