import man.motion.SweetMoves as SweetMoves
import man.motion as motion

FRAME_SAVE_RATE = 5
NUM_FRAMES_TO_SAVE = 150

def gameReady(player):
    player.brain.loc.reset()
    return player.goNow('saveFrames')

def gameSet(player):
    player.brain.loc.reset()
    return player.goNow('saveFrames')

def gamePlaying(player):
    player.brain.loc.reset()
    return player.goNow('saveFrames')

def saveFrames(player):
    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.standup()
        player.setSpeed(0,0,0)
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

#     if player.stateTime > 8.0:
#         shutoff = motion.StiffnessCommand(0.0)
#         player.brain.motion.sendStiffness(shutoff)

    return player.stay()
