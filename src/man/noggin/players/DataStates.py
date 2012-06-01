import man.motion.SweetMoves as SweetMoves
import man.motion.HeadMoves as HeadMoves

NUM_FRAMES_TO_SAVE = 500
MAX_DIST_TO_MEASURE = 300
DIST_INCREMENT = 50
FRAMES_TO_WAIT = 400

def gameReady(player):
    player.brain.resetLocalization()
    return player.goNow('standup')

def gameSet(player):
    player.brain.resetLocalization()
    return player.goNow('standup')

def gamePlaying(player):
    player.brain.resetLocalization()
    return player.goNow('standup')

def standup(player):
    if player.firstFrame():
        player.brain.tracker.stopHeadMoves()
        player.stand()

    elif not player.brain.motion.isBodyActive():
        return player.goLater('saveFrames')

    return player.stay()

def saveFrames(player):
    if player.firstFrame():
        player.brain.tracker.startScan(HeadMoves.DATA_PAN)

    #player.brain.sensors.saveFrame()
    player.savePostInfo()

    if player.counter > NUM_FRAMES_TO_SAVE:
        player.postDistance += DIST_INCREMENT

        if player.postDistance > MAX_DIST_TO_MEASURE:
            return player.goLater('doneState')
        return player.goLater('waitBetweenDists')

    return player.stay()

def waitBetweenDists(player):
    # if player.firstFrame():
    #     player.zeroHeads()
    if player.counter > FRAMES_TO_WAIT:
        return player.goLater('saveFrames')
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
