from ..playbook.PBConstants import DEFAULT_CHASER_NUMBER, GOALIE

###
# Reimplementation of Game Controller States for pBrunswick
###

def gameReady(player):
    """
    Stand up, and pan for localization
    """
    player.standup()
    player.brain.tracker.locPans()
    if player.lastDiffState == 'gameInitial':
        return player.goLater('relocalize')
    return player.goLater('playbookPosition')

def gameSet(player):
    """
    Fixate on the ball, or scan to look for it
    """
    if player.firstFrame():
        player.stopWalking()
        if player.brain.my.playerNumber == DEFAULT_CHASER_NUMBER:
            player.brain.tracker.trackBall()
        else:
            player.brain.tracker.activeLoc()
        if player.brain.playbook.role == GOALIE:
            pass#player.brain.resetGoalieLocalization()

    return player.stay()

def gamePlaying(player):
    roleState = player.getRoleState(player.currentRole)
    return player.goNow(roleState)

def penaltyShotsGameReady(player):
    if player.firstFrame():
        player.brain.tracker.locPans()
        player.walkPose()
        if player.brain.playbook.role == GOALIE:
            pass#player.brain.resetGoalieLocalization()
    return player.stay()

def penaltyShotsGameSet(player):
    if player.firstFrame():
        if player.brain.playbook.role == GOALIE:
            player.brain.tracker.trackBall()
        else:
            player.brain.tracker.activeLoc()
    return player.stay()

def penaltyShotsGamePlaying(player):
    if player.brain.playbook.role == GOALIE:
        return player.goNow('penaltyGoalie')
    return player.goNow('penaltyKick')
