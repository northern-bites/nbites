import man.motion.SweetMoves as SweetMoves

def gameInitial(player):
    return player.stay()

def gameReady(player):
    return player.stay()

def gameSet(player):
    return player.stay()

def gamePlaying(player):
    if player.firstFrame():
        player.gainsOn()
        player.stand()
        player.brain.tracker.setNeutralHead()
        return player.stay()

    # Wait until the sensors are calibrated before moving.
    while (not player.brain.motion.calibrated()):
        return player.stay()

    # enter chaseBallStates
    return player.goLater('chase')

def gamePenalized(player):
    if player.firstFrame():
        player.stopWalking()
        player.penalizeHeads()

    return player.stay()

def gameFinished(player):
    if player.firstFrame():
        player.stopWalking()
        player.zeroHeads()
        player.executeMove(SweetMoves.SIT_POS)
        return player.stay()

    if player.brain.nav.isStopped():
        player.gainsOff()

    return player.stay()
