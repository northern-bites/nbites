import man.motion.SweetMoves as SweetMoves

FRAME_SAVE_RATE = 5
NUM_FRAMES_TO_SAVE = 150

def gamePlaying(player):
    return player.goNow('saveFrames')

def saveFrames(player):
    if player.firstFrame():
        player.brain.tracker.switchTo('locPans')
        player.setSpeed(6,0,0)
    if player.counter % FRAME_SAVE_RATE == 0:
        player.brain.sensors.saveFrame()
    if player.counter == 800:
        player.setSpeed(0,0,0)
    if player.counter > FRAME_SAVE_RATE * NUM_FRAMES_TO_SAVE:
        return player.goNow('doneState')

    return player.stay()

def doneState(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.SIT_POS)
        player.brain.tracker.stopHeadMoves()
        player.brain.sensors.resetSaveFrame()

    if player.stateTime > 8.0:
        shutoff = motion.StiffnessCommand(0.0)
        player.brain.motion.sendStiffness(shutoff)

    return player.stay()
