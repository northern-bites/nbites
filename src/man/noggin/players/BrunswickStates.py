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
        player.stopWalking()
        player.gainsOn()
        player.zeroHeads()
        player.GAME_INITIAL_satDown = False

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
        player.stopWalking()
        player.walkPose()
        player.brain.tracker.locPans()
        player.brain.sensors.startSavingFrames()
        player.gainsOn()

        # Works with rules (2011) to get goalie manually positioned
        if (player.lastDiffState == 'gameInitial'
            and not player.brain.play.isRole(GOALIE)):
            return player.goLater('relocalize')

        elif player.lastDiffState == 'gamePenalized':
            player.brain.resetLocalization()
            return player.goLater('afterPenalty')

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
        player.inKickingState = False
        player.stopWalking()
        player.gainsOn()
        player.walkPose()
        player.brain.loc.resetBall()
        player.brain.tracker.trackBall()

        if player.brain.play.isRole(GOALIE):
            player.brain.resetGoalieLocalization()

        if (player.brain.play.isRole(CHASER) and
            player.brain.gameController.ownKickOff):
            print "Setting Kickoff to True"
            player.shouldKickOff = True

        if player.lastDiffState == 'gamePenalized':
            player.brain.resetLocalization()

    return player.stay()

def gamePlaying(player):
    if player.firstFrame():
        player.stopWalking()
        player.gainsOn()
        player.walkPose()
        player.brain.logger.startLogging()
        if player.lastDiffState == 'gamePenalized':
            player.brain.sensors.startSavingFrames()

            if player.lastStateTime > 25:
                # 25 is arbitrary. This check is meant to catch human error and
                # possible 0 sec. penalties for the goalie
                player.brain.resetLocalization()
                return player.goLater('afterPenalty')
                # 2011 rules have no 0 second penalties for any robot,
                # but check should be here if there is.
            #else human error

    roleState = player.getRoleState()
    return player.goNow(roleState)


def gamePenalized(player):
    if player.firstFrame():
        player.inKickingState = False
        player.stopWalking()
        player.penalizeHeads()
        player.brain.sensors.stopSavingFrames()
        player.brain.logger.stopLogging()

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
        player.walkPose()
        player.brain.loc.resetBall()
        player.inKickingState = False

        if player.lastDiffState == 'gamePenalized':
            player.brain.resetLocalization()
        if player.brain.play.isRole(GOALIE):
            player.brain.tracker.trackBall()
        else:
            player.brain.tracker.activeLoc()
    if player.brain.play.isRole(GOALIE):
        player.brain.resetGoalieLocalization()

    return player.stay()

def penaltyShotsGamePlaying(player):
    if (player.lastDiffState == 'gamePenalized' and
            player.firstFrame()):
        player.brain.resetLocalization()

    if player.firstFrame():
        player.gainsOn()
        player.walkPose()
        player.penaltyKicking = True

    if player.brain.play.isRole(GOALIE):
        player.brain.play.setSubRole(GOALIE_KICKOFF)
        roleState = player.getRoleState()
        return player.goNow(roleState)
    return player.goNow('chase')


