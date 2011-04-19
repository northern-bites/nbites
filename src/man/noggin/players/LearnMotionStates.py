from man.motion import SweetMoves

# Clear the motion queue
def gameInitial(player):
    if player.firstFrame():
        player.motionQueue = []
    return player.stay()

def gameReady(player):
    if player.firstFrame():
        player.gainsOn()
        player.standup()
    return player.stay()

# Put the robot in a position to be manipulated, probably with low stiffnesses
# somewhere
def gameSet(player):
    if player.firstFrame():
        player.executeLearnedMove(SweetMoves.LEARN_KICK_LEFT)
    return player.stay()

# Save the motion angles for later replay
def gamePlaying(player):
    if player.firstFrame():
        player.saveMotionFrame()
    return player.stay()

# Same as playing
def gamePenalized(player):
    if player.firstFrame():
        player.saveMotionFrame()
    return player.stay()

# Execute the move created in playing/penalized
def gameFinished(player):
    if player.firstFrame():
        player.gainsOn()
        for move in player.motionQueue:
            player.executeLearnedMove(move)
    return player.stay()
