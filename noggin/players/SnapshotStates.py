import man.motion.SweetMoves as SweetMoves
import man.motion.HeadMoves as HeadMoves

####Change these for picture taking####
FRAME_SAVE_RATE = 1
NUM_FRAMES_TO_SAVE = 500

def gameReady(player):
    player.brain.resetLocalization()
    return player.goNow('saveFrames')

def gameSet(player):
    player.brain.resetLocalization()
    return player.goNow('saveFrames')

def gamePlaying(player):
    player.brain.resetLocalization()
    return player.goNow('saveFrames')

def saveFrames(player):
    if player.firstFrame():
        player.brain.tracker.startScan(HeadMoves.FORWARD_COMB_PAN)
        ##replace <TYPE_SNAPSHOT_PAN> with any PHOTO PAN in
        ##    man/motion/HeadMoves.py
        player.standup()
    if player.counter % FRAME_SAVE_RATE == 0:
        player.brain.sensors.saveFrame()
    if player.counter == 800:
        player.stopWalking()
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

#gameInitial = gameReady
