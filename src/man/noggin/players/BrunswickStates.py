from ..playbook.PBConstants import (GOALIE, CHASER, GOALIE_KICKOFF)
import man.motion.SweetMoves as SweetMoves

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
        player.brain.nav.stop()
        player.gainsOn()
        player.zeroHeads()
        player.GAME_INITIAL_satDown = False
        # Reset localization to proper starting position by player number.
        # Locations are defined in the wiki.
        if player.brain.my.playerNumber == 1:
            player.brain.loc.resetLocTo(player.brain.Constants.BLUE_GOALBOX_RIGHT_X,
                                        player.brain.Constants.FIELD_WHITE_BOTTOM_SIDLELINE_Y,
                                        player.brain.Constants.HEADING_UP)
        elif player.brain.my.playerNumber == 2:
            player.brain.loc.resetLocTo(player.brain.Constants.LANDMARK_BLUE_GOAL_CROSS_X,
                                        player.brain.Constants.FIELD_WHITE_BOTTOM_SIDELINE_Y,
                                        player.brain.Constants.HEADING_UP)
        elif player.brain.my.playerNumber == 3:
            player.brain.loc.resetLocTo(player.brain.LANDMARKBLUE_GOAL_CROSS_X,
                                        player.brain.Constants.FIELD_WHITE_TOP_SIDELINE_Y,
                                        player.brain.Constants.HEADING_DOWN)
        elif player.brain.my.playerNumber == 4:
            player.brain.loc.resetLocTo(player.brain.Constants.BLUE_GOALBOX_RIGHT_X,
                                        player.brain.Constants.FIELD_WHITE_TOP_SIDELINE_Y,
                                        player.brain.Constants.HEADING_DOWN)

    elif (player.brain.nav.isStopped() and not player.GAME_INITIAL_satDown
          and not player.motion.isBodyActive()):
        player.GAME_INITIAL_satDown = True
        player.executeMove(SweetMoves.SIT_POS)

    return player.stay()

def gameReady(player):
    """
    Stand up, and pan for localization
    """
    if player.firstFrame():
        player.inKickingState = False
        player.gainsOn()
        player.brain.nav.stand()
        player.brain.tracker.repeatWidePanFixedPitch()
        player.brain.sensors.startSavingFrames()
        if player.lastDiffState == 'gameInitial':
            player.initialDelayCounter = 0

    #HACK! TODO: this delay is to make sure the sensors get calibrated before
    #we start walking; find a way to query motion to see whether the sensors are
    #calibrated or not before starting
    player.initialDelayCounter += 1
    if player.initialDelayCounter < 230:
        return player.stay()

    # Works with rules (2011) to get goalie manually positioned
    if (player.lastDiffState == 'gameInitial'
        and not player.brain.play.isRole(GOALIE)):
        return player.goLater('relocalize')

    elif player.lastDiffState == 'gamePenalized':
        player.brain.loc.resetLocTo(player.brain.Constants.LANDMARK_BLUE_GOAL_CROSS_X,
                                    player.brain.Constants.FIELD_WHITE_BOTTOM_SIDELINE_Y,
                                    90,
                                    player.brain.Constants.LANDMARK_BLUE_GOAL_CROSS_X,
                                    player.brain.Constants.FIELD_WHITE_TOP_SIDELINE_Y,
                                    -90)
        # Do we still want to do this? Seems to be just a hack for loc. Summer 2012
        #return player.goLater('afterPenalty')

    #See above about rules(2011) - we should still reposition after goals
    if (player.lastDiffState == 'gameInitial'
        and player.brain.play.isRole(GOALIE)):
        return player.stay()
    else:
        return player.goLater('playbookPosition')

def gameSet(player):
    """
    Fixate on the ball, or scan to look for it
    """
    if player.firstFrame():
        player.brain.logger.startLogging()
        player.inKickingState = False
        player.brain.nav.stand()
        player.gainsOn()
        player.brain.loc.resetBall()
        player.brain.tracker.trackBallFixedPitch()

        if player.brain.play.isRole(GOALIE):
            player.brain.resetGoalieLocalization()

        if (player.brain.my.playerNumber == 2 and
            player.brain.gameController.ownKickOff):
            print "Setting Kickoff to True"
            player.shouldKickOff = True

        if player.lastDiffState == 'gamePenalized':
            player.brain.resetLocalization()

    # For the goalie, reset loc every frame.
    # This way, garaunteed to have correctly set loc and be standing in that
    #  location for a frame before gamePlaying begins.
    if player.brain.play.isRole(GOALIE):
        player.brain.loc.resetLocTo(player.brain.Constants.FIELD_WHITE_LEFT_SIDELINE_X,
                                    player.brain.Constants.MIDFIELD_Y,
                                    0)

    return player.stay()

def gamePlaying(player):
    if player.firstFrame():
        player.gainsOn()
        player.brain.nav.stand()
        player.brain.tracker.trackBallFixedPitch()
        if player.lastDiffState == 'gamePenalized':
            player.brain.sensors.startSavingFrames()
            if player.lastStateTime > 25:
                # 25 is arbitrary. This check is meant to catch human error and
                # possible 0 sec. penalties for the goalie
                player.brain.loc.resetLocTo(player.brain.Constants.LANDMARK_BLUE_GOAL_CROSS_X,
                                            player.brain.Constants.FIELD_WHITE_BOTTOM_SIDELINE_Y,
                                            90,
                                            player.brain.Constants.LANDMARK_BLUE_GOAL_CROSS_X,
                                            player.brain.Constants.FIELD_WHITE_TOP_SIDELINE_Y,
                                            -90)
                # Do we still want to do this? Seems to be just a hack for loc.
                #   Summer 2012
                #return player.goLater('afterPenalty')

                # 2011 rules have no 0 second penalties for any robot,
                # but check should be here if there is.
            #else human error
    roleState = player.getRoleState()
    return player.goNow(roleState)


def gamePenalized(player):
    if player.firstFrame():
        player.brain.logger.stopLogging()
        player.inKickingState = False
        player.stopWalking()
        player.penalizeHeads()
        player.brain.sensors.stopSavingFrames()

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
    Also, in the future, gameInitial may be responsible for turning off the gains
    """
    if player.firstFrame():
        player.inKickingState = False
        player.stopWalking()
        player.zeroHeads()
        player.GAME_FINISHED_satDown = False
        player.brain.sensors.stopSavingFrames()
        return player.stay()

    # Sit down once we've finished walking
    if (player.brain.nav.isStopped() and not player.GAME_FINISHED_satDown
        and not player.motion.isBodyActive()):
        player.GAME_FINISHED_satDown = True
        player.executeMove(SweetMoves.SIT_POS)
        return player.stay()

    if not player.motion.isBodyActive() and  player.GAME_FINISHED_satDown:
        player.gainsOff()
    return player.stay()

########### PENALTY SHOTS STATES ############

def penaltyShotsGameSet(player):
    if player.firstFrame():
        player.gainsOn()
        player.stopWalking()
        player.stand()
        player.brain.loc.resetBall()
        player.inKickingState = False

        if player.lastDiffState == 'gamePenalized':
            player.brain.resetLocalization()
        if player.brain.play.isRole(GOALIE):
            player.brain.tracker.trackBallFixedPitch()
        else:
            #player.brain.tracker.activeLoc()
            player.brain.tracker.trackBallFixedPitch()
    if player.brain.play.isRole(GOALIE):
        player.brain.resetGoalieLocalization()

    return player.stay()

def penaltyShotsGamePlaying(player):
    if (player.lastDiffState == 'gamePenalized' and
            player.firstFrame()):
        player.brain.resetLocalization()

    if player.firstFrame():
        player.gainsOn()
        player.stand()
        player.penaltyKicking = True

    if player.brain.play.isRole(GOALIE):
        player.brain.play.setSubRole(GOALIE_KICKOFF)
        roleState = player.getRoleState()
        return player.goNow(roleState)
    return player.goNow('chase')


