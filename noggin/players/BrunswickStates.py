
###
# Reimplementation of Game Controller States for pBrunswick
###
def gameReady(player):
    """
    Stand up, and pan for localization
    """
    if player.firstFrame():
        player.standup()
        player.brain.tracker.switchTo('locPans')
    return player.stay()

def gameSet(player):
    """
    Fixate on the ball, or scan to look for it
    """
    if player.firstFrame():
        player.stopWalking()
        if player.brain.my.playerNumber == 3:
            player.brain.tracker.trackBall()
        else:
            player.brain.tracker.activeLoc()

    return player.stay()

def gamePlaying(player):
    roleState = player.getRoleState(player.currentRole)
    return player.goNow(roleState)
