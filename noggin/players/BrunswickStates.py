from ..playbook.PBConstants import (GOALIE, CHASER)
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
        player.isChasing = False
        player.inKickingState = False
        player.justKicked = False
        player.gainsOn()
        player.zeroHeads()
        player.GAME_INITIAL_satDown = False

        if player.squatting:
            player.executeMove(SweetMoves.GOALIE_SQUAT_STAND_UP)
            player.squatting = False
        else:
            player.stopWalking()

    elif (player.brain.nav.isStopped() and not player.GAME_INITIAL_satDown
          and player.motion.isBodyActive()):
        player.GAME_INITIAL_satDown = True
        player.executeMove(SweetMoves.SIT_POS)

    return player.stay()

def gamePenalized(player):
    if player.firstFrame():
        player.isChasing = False
        player.inKickingState = False
        player.justKicked = False
        player.penalizeHeads()

        if player.squatting:
            player.executeMove(SweetMoves.GOALIE_SQUAT_STAND_UP)
            player.squatting = False
        else:
            player.stopWalking()

    if (player.brain.play.isRole(GOALIE) and
        player.stateTime >=
        SweetMoves.getMoveTime(SweetMoves.GOALIE_SQUAT_STAND_UP)):
        player.stopWalking()

    return player.stay()

def gameReady(player):
    """
    Stand up, and pan for localization
    """
    if player.firstFrame():
        player.isChasing = False
        player.inKickingState = False
        player.justKicked = False
        player.brain.CoA.setRobotGait(player.brain.motion)

        if player.squatting:
            player.executeMove(SweetMoves.GOALIE_SQUAT_STAND_UP)
            player.squatting = False
        else:
            player.standup()

    if player.brain.gameController.ownKickOff:
        player.hasKickedOffKick = False
    else:
        player.hasKickedOffKick = True

    player.brain.tracker.locPans()
    if player.lastDiffState == 'gameInitial':
        return player.goLater('relocalize')
    if player.firstFrame() and \
            player.lastDiffState == 'gamePenalized':
        player.brain.resetLocalization()

    return player.goLater('playbookPosition')

def gameSet(player):
    """
    Fixate on the ball, or scan to look for it
    """
    if player.firstFrame():
        player.isChasing = False
        player.inKickingState = False
        player.justKicked = False
        player.brain.CoA.setRobotGait(player.brain.motion)

    if player.firstFrame() and player.lastDiffState == 'gamePenalized':
        player.brain.resetLocalization()

    if player.firstFrame():
        player.stopWalking()
        player.brain.loc.resetBall()

        if player.brain.play.isRole(GOALIE):
            player.brain.resetGoalieLocalization()
            if player.squatting:
                return player.goLater('squatted')
            return player.goLater('squat')

        if player.brain.play.isRole(CHASER):
            player.brain.tracker.trackBall()
        else:
            player.brain.tracker.activeLoc()

    return player.stay()

def gamePlaying(player):
    if player.firstFrame():
        player.brain.CoA.setRobotGait(player.brain.motion)
    if (player.firstFrame() and
        player.lastDiffState == 'gamePenalized'):
        player.brain.resetLocalization()

    roleState = player.getRoleState()
    return player.goNow(roleState)

def penaltyShotsGameReady(player):
    if player.firstFrame():
        player.brain.CoA.setRobotGait(player.brain.motion)
    if player.firstFrame():
        if player.lastDiffState == 'gamePenalized':
            player.brain.resetLocalization()
        player.brain.tracker.locPans()
        player.walkPose()
        if player.brain.play.isRole(GOALIE):
            player.brain.resetGoalieLocalization()

    return player.stay()

def penaltyShotsGameSet(player):
    if player.firstFrame():
        player.brain.CoA.setRobotGait(player.brain.motion)
    if player.firstFrame():
        player.stopWalking()
        player.brain.loc.resetBall()

        if player.lastDiffState == 'gamePenalized':
            player.brain.resetLocalization()
        if player.brain.play.isRole(GOALIE):
            player.brain.tracker.trackBall()
        else:
            player.brain.tracker.activeLoc()
    if player.brain.play.isRole(GOALIE):
        player.brain.resetGoalieLocalization()
        if player.squatting:
            return player.goLater('squatted')
        return player.goLater('squat')

    return player.stay()

def penaltyShotsGamePlaying(player):
    if player.firstFrame():
        player.brain.CoA.setRobotGait(player.brain.motion)
    if player.lastDiffState == 'gamePenalized' and \
            player.firstFrame():
        player.brain.resetLocalization()
    if player.brain.play.isRole(GOALIE):
        return player.goNow('penaltyGoalie')
    return player.goNow('penaltyKick')

def fallen(player):
    """
    Stops the player when the robot has fallen
    """
    player.isChasing = False
    player.inKickingState = False
    player.justKicked = False
    player.squatting = False
    player.brain.nav.switchTo('stopped')
    return player.stay()


def gameFinished(player):
    """
    Ensure we are sitting down and head is snapped forward.
    In the future, we may wish to make the head move a bit slower here
    Also, in the future, gameInitial may be responsible for turning off the gains
    """
    if player.firstFrame():
        player.isChasing = False
        player.inKickingState = False
        player.justKicked = False
        if player.squatting:
            player.executeMove(SweetMoves.GOALIE_SQUAT_STAND_UP)
            player.squatting = False
        else:
            player.stopWalking()

        player.zeroHeads()
        player.GAME_FINISHED_satDown = False
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
