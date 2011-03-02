import man.motion.SweetMoves as SweetMoves
import man.motion.HeadMoves as HeadMoves

def gameInitial(player):
    if player.firstFrame():
        player.zeroHeads()
        player.executeMove(SweetMoves.SIT_POS)
    return player.stay()

def gameReady(player):
    if player.firstFrame():
        player.standup()
    return player.stay()

def gameSet(player):
    if player.firstFrame():
        player.brain.tracker.performHeadMove(HeadMoves.LOOK_UP)
    return player.stay()

def gamePlaying(player):
    if player.firstFrame():
        player.brain.sensors.startSavingFrames()
    return player.stay()

def gamePenalized(player):
    if player.firstFrame():
        player.brain.sensors.stopSavingFrames()
    return player.stay()

def gameFinished(player):
    if player.firstFrame():
        player.brain.sensors.stopSavingFrames()
        player.zeroHeads()
        player.executeMove(SweetMoves.SIT_POS)

    if not player.motion.isBodyActive() and  player.GAME_FINISHED_satDown:
        player.gainsOff()
    return player.stay()
