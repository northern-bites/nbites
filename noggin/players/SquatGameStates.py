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
    if player.squatting:
        return player.goLater('squatted')
    return player.goLater('squat')

def gameSet(player):
    if player.squatting:
        return player.goLater('squatted')
    return player.goLater('squat')

def gamePlaying(player):
    if player.squatting:
        return player.goLater('squatted')
    return player.goLater('squat')

def gameFinished(player):
    """
    Ensure we are sitting down and head is snapped forward.
    In the future, we may wish to make the head move a bit slower here
    Also, in the future, gameInitial may be responsible for turning off the gains
    """
    if player.squatting:
        player.executeMove(SweetMoves.GOALIE_SQUAT_STAND_UP)
        player.squatting = False

    if player.firstFrame():
        player.zeroHeads()
        player.GAME_FINISHED_satDown = False
        return player.stay()

    # Sit down once we've finished walking
    if player.brain.nav.isStopped() and not player.GAME_FINISHED_satDown:
        player.GAME_FINISHED_satDown = True
        player.executeMove(SweetMoves.SIT_POS)
        return player.stay()

    if not player.motion.isBodyActive() and player.GAME_FINISHED_satDown:
        player.gainsOff()
    return player.stay()

def gamePenalized(player):
    if player.firstFrame():
        player.penalizeHeads()
    return player.stay()
