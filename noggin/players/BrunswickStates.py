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
    if player.firstFrame() and \
            player.lastDiffState == 'gamePenalized':
        player.brain.resetLocalization()

    return player.goLater('playbookPosition')

def gameSet(player):
    """
    Fixate on the ball, or scan to look for it
    """
    if player.firstFrame() and player.lastDiffState == 'gamePenalized':
        player.brain.resetLocalization()

    if player.firstFrame():
        player.stopWalking()
        player.brain.loc.resetBall()

        if player.brain.playbook.role == GOALIE:
            player.brain.resetGoalieLocalization()
        elif player.brain.my.playerNumber == DEFAULT_CHASER_NUMBER:
            player.brain.tracker.trackBall()
        else:
            player.brain.tracker.activeLoc()

    return player.stay()

def gamePlaying(player):
    if player.firstFrame() and \
            player.lastDiffState == 'gamePenalized':
        player.brain.resetLocalization()

    roleState = player.getRoleState(player.currentRole)
    return player.goNow(roleState)

def penaltyShotsGameReady(player):
    if player.firstFrame():
        if player.lastDiffState == 'gamePenalized':
            player.brain.resetLocalization()
        player.brain.tracker.locPans()
        player.walkPose()
        if player.brain.playbook.role == GOALIE:
            player.brain.resetGoalieLocalization()
    return player.stay()

def penaltyShotsGameSet(player):
    if player.firstFrame():
        if player.lastDiffState == 'gamePenalized':
            player.brain.resetLocalization()
        if player.brain.playbook.role == GOALIE:
            player.brain.tracker.trackBall()
        else:
            player.brain.tracker.activeLoc()
    return player.stay()

def penaltyShotsGamePlaying(player):
    if player.lastDiffState == 'gamePenalized' and \
            player.firstFrame():
        player.brain.resetLocalization()

    if player.brain.playbook.role == GOALIE:
        return player.goNow('penaltyGoalie')
    return player.goNow('penaltyKick')
