"""
Game controller states for pBrunswick, our soccer player.
"""

from ..Say import *
import BrunswickHelper as helper

import noggin_constants as nogginConstants
from math import fabs
from ..util import *
from .. import SweetMoves
from . import RoleConstants as roleConstants
import KickOffConstants as kickOff
from math import fabs, degrees

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
        
        player.stand()
        player.lastStiffStatus = True
        player.zeroHeads()
        
        player.brain.resetInitialLocalization()
        
        player.role = player.brain.playerNumber
        roleConstants.setRoleConstants(player, player.role)

    # If stiffnesses were JUST turned on, then stand up.
    if player.lastStiffStatus == False and player.brain.interface.stiffStatus.on:
        player.stand()
        player.zeroHeads()
    
    # Remember last stiffness.
    player.lastStiffStatus = player.brain.interface.stiffStatus.on

    return player.stay()

@superState('gameControllerResponder')
@stay
@defaultState('localize')
def gameReady(player):
    """
    Stand up, and pan for localization, and walk to kicking off positions.
    """
    if player.firstFrame():
        player.inKickingState = False
        player.brain.fallController.enabled = True
        
        # player.stand()
        player.brain.tracker.switchTo('snap')

        # player.timeReadyBegan = player.brain.time
        # if player.lastDiffState == 'gameInitial':
        #     player.brain.resetInitialLocalization()

        # if player.wasPenalized:
        #     player.wasPenalized = False
        #     return player.goNow('manualPlacement')

    # Wait until the sensors are calibrated before moving.
    # if player.brain.motion.calibrated:
    #     player.helper.walkToReadyPosition(player.brain, player.role)


@superState('gameReady')
@stay
@ifSwitchLater(helper.shouldWalkToReadyPosition, 'walkToReadyPosition')
def localize(player):
    print "in localize"
    print helper.shouldWalkToReadyPosition(player)

@superState('gameReady')
@stay
@ifSwitchNow(helper.inReadyPosition, 'inKickoffPosition')
def walkToReadyPosition(player):
    if player.firstFrame():
        helper.walkToReadyPosition(player.brain, player.role)
    print player.brain.nav.destination

@superState('gameReady')
@stay
@ifSwitchNow(helper.noLongerInReadyPosition, 'walkToReadyPosition')
def inKickoffPosition(player):
    print "in kickoff position"






@superState('gameControllerResponder')
def gameSet(player):
    """
    Fixate on the ball, or scan to look for it
    """

    # if player.brain.interface.fallStatus.pickup == 1:
    #     player.brain.pickedUpInSet = True

    # if player.firstFrame():
    #     #The player's currentState = gameSet

    #     # print "GAME SET FIRST FRAME"

    #     player.inKickingState = False
    #     player.brain.fallController.enabled = True
    #     player.gainsOn()
    #     player.stand()
    #     player.brain.nav.stand()

    #     player.brain.whistleHeard = False
    #     player.brain.whistlePenalty = False

    #     player.brain.gameSetX = player.brain.loc.x
    #     player.brain.gameSetY = player.brain.loc.y
    #     player.brain.gameSetH = player.brain.loc.h

    #     # player.brain.tracker.helper.boundsSnapPan(-90, 90, False)
    #     player.brain.tracker.performGameSetInitialWideSnapPan()
    #     # player.brain.tracker.helper.startingPan(Head)
    #     # player.brain.tracker.helper.executeHeadMove(player.brain.tracker.helper.boundsSnapPan(-90, 90, False))

    #     if player.wasPenalized:
    #         player.wasPenalized = False
    #         return player.goNow('manualPlacement')

    # elif player.brain.tracker.isStopped():

    #     # print "TRUE"

    #     player.brain.tracker.trackBall(True)
    #     # print "Current Angle: " + str(degrees(self.tracker.brain.interface.joints.head_yaw))
    # # Wait until the sensors are calibrated before moving.
    # if not player.brain.motion.calibrated:
    #     return player.stay()
    
    return player.stay()

@superState('gameControllerResponder')
def gamePlaying(player):
    if player.firstFrame():
        player.inKickingState = False
        # player.inKickOffPlay = (kickOff.shouldRunKickOffPlay(player) and 
        #                        (roleConstants.isChaser(player.role) or 
        #                         roleConstants.isCherryPicker(player.role)))
        player.inKickOffPlay = False
        player.passBack = False
        player.brain.fallController.enabled = True
        player.brain.nav.stand()
        player.brain.tracker.trackBall()

        # If we were picked up in set, we were probably manually placed. Let's
        # reset to manual placement locations.
        if player.brain.pickedUpInSet == True:
            player.brain.pickedUpInSet = False
            player.brain.player.brain.resetLocTo(999, 999, 999)
            
    # TODO without pb, is this an issue?
    # if (player.lastDiffState == 'afterPenalty' and
    #     player.brain.play.isChaser()):
    #     # special behavior return
    #     case player.goNow('postPenaltyChaser')
    # Wait until the sensors are calibrated before moving.

    if player.wasPenalized:
        player.wasPenalized = False
        if player.lastDiffState != 'gameSet': 
            # Remove the next two lines once we're done testing manual placement
            # if DEBUG_MANUAL_PLACEMENT:
            #     return player.goNow('manualPlacement')
            return player.goNow('afterPenalty')

    if not player.brain.motion.calibrated:
        return player.stay()

    if player.brain.gameController.timeSincePlaying < 10:
        if player.brain.gameController.ownKickOff:
            if (roleConstants.isFirstChaser(player.role) and
                player.brain.ball.vis.on):
                player.shouldKickOff = True
                print "SHOULD KICK OFF"
                return player.goNow('approachBall')
            else:
                return player.goNow('playOffBall')
        else:
            return player.goNow('waitForKickoff')
    return player.goNow('playOffBall')


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
        player.wasPenalized = False

        player.brain.whistleHeard = False


        if nogginConstants.V5_ROBOT:
            player.executeMove(SweetMoves.SIT_POS_V5)
        else:
            player.executeMove(SweetMoves.SIT_POS)

    if player.brain.nav.isStopped():
        player.gainsOff()

    return player.stay()

@superState('gameControllerResponder')
def gamePenalized(player):
    if player.firstFrame():
        player.inKickingState = False
        player.brain.fallController.enabled = True
        player.gainsOn()
        player.stand()
        player.penalizeHeads()
        player.wasPenalized = True
        player.brain.penalizedEdgeClose = 0
        player.brain.penalizedCount = 0
        player.brain.penaltyCount = 0

        if player.brain.whistleHeard:
            print "BrunswickStates.py: whistle heard and now a penalty?!?! I'm hearing things!"
            player.brain.whistlePenalty = True

        # save current score so we can check in afterPenalty
        player.brain.scoreAtPenaltyUs = player.brain.ourScore
        player.brain.scoreAtPenaltyThem = player.brain.theirScore

        player.executeMove(SweetMoves.STAND_STRAIGHT_POS)
        # RESET LOC TO FIELD CROSS
        if player.brain.penalizedHack:
            player.brain.resetLocToCross()
            # print "BRUNSWICK PENALIZED"

    if player.brain.vision.horizon_dist < 200.0:
        player.brain.penalizedEdgeClose += 1

#    if player.brain.interface.gameState.whistle_override:
#        player.brain.whistleHeard = True
#    else:
#        player.brain.whistleHeard = False

    player.brain.penaltyCount += 1
    return player.stay()

@superState('gameControllerResponder')
def waitForKickoff(player):
    if player.firstFrame():
        waitForKickoff.ballRelX = player.brain.ball.rel_x
        waitForKickoff.ballRelY = player.brain.ball.rel_y

    if (player.brain.gameController.timeSincePlaying > 10 or
        fabs(player.brain.ball.rel_x - waitForKickoff.ballRelX) > 20 or
        fabs(player.brain.ball.rel_y - waitForKickoff.ballRelY) > 20):
        return player.goNow('playOffBall')

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
