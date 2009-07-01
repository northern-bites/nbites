import man.motion.SweetMoves as SweetMoves

def gameReady(player):
    if player.firstFrame():
        player.standup()
    if player.counter == 10:
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
        player.gainsOff()
    return player.stay()

def gamePenalized(player):
    if player.firstFrame():
        player.penalizeHeads()
    return player.stay()
