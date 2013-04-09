from ..playbook.PBConstants import (GOALIE, CHASER, GOALIE_KICKOFF)
from .. import SweetMoves
import noggin_constants as nogginConstants
#import _localization

###
# Reimplementation of Game Controller States for pBrunswick
###

def gameInitial(player):
    """
    Ensure we are sitting down and head is snapped forward.
    In the future, we may wish to make the head move a bit slower here
    Also, in the future, gameInitial may be responsible for turning off the gains
    """
    if player.firstFrame():
        player.inKickingState = False
        player.stand()
        # player.zeroHeads()
        # Reset localization to proper starting position by player number.
        # Locations are defined in the wiki.
# HACK HACK until localization is implemented in messages we cant do this.
        # if player.brain.my.playerNumber == 1:
        #     player.brain.loc.resetLocTo(nogginConstants.BLUE_GOALBOX_RIGHT_X,
        #                                 nogginConstants.FIELD_WHITE_BOTTOM_SIDELINE_Y,
        #                                 nogginConstants.HEADING_UP,
        #                                 _localization.LocNormalParams(15.0, 15.0, 1.0))
        # elif player.brain.my.playerNumber == 2:
        #     player.brain.loc.resetLocTo(nogginConstants.LANDMARK_BLUE_GOAL_CROSS_X,
        #                                 nogginConstants.FIELD_WHITE_BOTTOM_SIDELINE_Y,
        #                                 nogginConstants.HEADING_UP,
        #                                 _localization.LocNormalParams(15.0, 15.0, 1.0))
        # elif player.brain.my.playerNumber == 3:
        #     player.brain.loc.resetLocTo(nogginConstants.LANDMARK_BLUE_GOAL_CROSS_X,
        #                                 nogginConstants.FIELD_WHITE_TOP_SIDELINE_Y,
        #                                 nogginConstants.HEADING_DOWN,
        #                                 _localization.LocNormalParams(15.0, 15.0, 1.0))
        # elif player.brain.my.playerNumber == 4:
        #     player.brain.loc.resetLocTo(nogginConstants.BLUE_GOALBOX_RIGHT_X,
        #                                 nogginConstants.FIELD_WHITE_TOP_SIDELINE_Y,
        #                                 nogginConstants.HEADING_DOWN,
        #                                 _localization.LocNormalParams(15.0, 15.0, 1.0))

    return player.stay()

def gameReady(player):
    """
    Stand up, and pan for localization
    """
    if player.firstFrame():
        player.inKickingState = False
        player.brain.nav.stand()
        player.brain.tracker.repeatWidePan()

    # Reset localization to proper starting position by player number.
    # Locations are defined in the wiki.
        if player.lastDiffState == 'gameInitial':
            player.brain.resetInitialLocalization()

        if player.lastDiffState == 'gamePenalized':
            player.brain.resetLocalizationFromPenalty()

    # Wait until the sensors are calibrated before moving.
    if not player.brain.motion.calibrated:
        return player.stay()

    return player.goLater('playbookPosition')

def gameSet(player):
    """
    Fixate on the ball, or scan to look for it
    """
    if player.firstFrame():
        player.inKickingState = False
        player.brain.nav.stand()
        player.brain.tracker.trackBall()

        if (player.brain.playerNumber == 4 and
            player.brain.gameController.ownKickOff):
            print "Setting Kickoff to True"
            player.shouldKickOff = True
        else:
            player.shouldKickOff = False

        if player.lastDiffState == 'gamePenalized':
            player.brain.resetSetLocalization()

    # Wait until the sensors are calibrated before moving.
    if not player.brain.motion.calibrated:
        return player.stay()

    return player.stay()

def gamePlaying(player):
    if player.firstFrame():
        player.brain.nav.stand()
        player.brain.tracker.trackBall()
        player.inKickingState = False
        if player.lastDiffState == 'gamePenalized':
            print 'Player coming out of penalized state after ' + str(player.lastStateTime) + ' seconds in last state'
            if player.lastStateTime > 5:
                player.brain.resetLocalizationFromPenalty()
        if player.lastDiffState == 'gameSet':
            player.brain.resetSetLocalization()

    # Wait until the sensors are calibrated before moving.
    if not player.brain.motion.calibrated:
        return player.stay()

    if player.lastDiffState == 'gamePenalized' and  player.brain.play.isChaser():
        return player.goNow('afterPenalty')

    roleState = player.getRoleState()
    return player.goNow(roleState)


def gamePenalized(player):
    if player.lastDiffState == '':
        # Just started up! Need to calibrate sensors
        player.brain.nav.stand()

    if player.firstFrame():
        player.inKickingState = False
        player.stopWalking()
        player.penalizeHeads()

    return player.stay()

def fallen(player):
    """
    Stops the player when the robot has fallen
    """
    player.inKickingState = False
    return player.stay()

def gameFinished(player):
    """
    Ensure we are sitting down and head is snapped forward.
    In the future, we may wish to make the head move a bit slower here
    """
    if player.firstFrame():
        player.inKickingState = False
        player.stopWalking()
        player.zeroHeads()
        player.executeMove(SweetMoves.SIT_POS)
        return player.stay()

    if player.brain.nav.isStopped():
        player.gainsOff()

    return player.stay()

########### PENALTY SHOTS STATES ############

def penaltyShotsGameSet(player):
    if player.firstFrame():
        player.stand()
        player.inKickingState = False

    # Wait until the sensors are calibrated before moving.
    if not player.brain.motion.calibrated:
        return player.stay()

        if player.lastDiffState == 'gamePenalized':
            player.brain.resetPenaltyKickLocalization()
        else:
            player.brain.tracker.trackBall()

    return player.stay()

def penaltyShotsGamePlaying(player):
    if (player.lastDiffState == 'gamePenalized' and
            player.firstFrame()):
        player.brain.resetPenaltyKickLocalization()

    if player.firstFrame():
        player.stand()
        player.inKickingState = False
        player.shouldKickOff = False
        player.penaltyKicking = True

    # Wait until the sensors are calibrated before moving.
    if (not player.brain.motion.calibrated):
        return player.stay()

    return player.goNow('chase')
