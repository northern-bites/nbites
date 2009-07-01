import man.motion.SweetMoves as SweetMoves

def gameInitial(player):
    """
    Ensure we are sitting down and head is snapped forward.
    In the future, we may wish to make the head move a bit slower here
    Also, in the future, gameInitial may be responsible for turning off the gains
    """
    if player.firstFrame() and not player.squatting:
        player.stopWalking()
        player.gainsOn()
        player.zeroHeads()
        player.GAME_INITIAL_satDown = False

    elif player.brain.nav.isStopped() and not player.GAME_INITIAL_satDown and\
            not player.squatting:
        player.GAME_INITIAL_satDown = True
        player.executeMove(SweetMoves.SIT_POS)

    return player.stay()

def gameReady(player):
    if player.firstFrame() and not player.squatting:
        player.standup()
        player.squatting = True
        player.executeMove(SweetMoves.GOALIE_CENTER_SAVE)
    return player.stay()

def gameSet(player):
    if player.firstFrame():
        player.brain.tracker.trackBall()
    return player.stay()

def gamePlaying(player):
    if player.firstFrame():
        player.brain.tracker.trackBall()
    return player.stay()

def gameFinished(player):
    if player.firstFrame():
        player.squatting = False
        player.gainsOff()
    return player.stay()

def gamePenalized(player):
    if player.firstFrame():
        player.penalizeHeads()
    return player.stay()
