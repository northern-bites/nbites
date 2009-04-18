import man.motion.SweetMoves as SweetMoves
import man.motion as motion

def gamePlaying(player):
    player.brain.loc.reset()
    return player.goNow('saveFrames')

def saveFrames(player):
    if player.firstFrame():
        player.brain.tracker.switchTo('locPans')
        player.setSpeed(0,0,20)
    if player.counter == 18*15:
        player.setSpeed(4,0,0)
    if player.counter > 800:
        return player.goNow('doneState')

    return player.stay()

def doneState(player):
    if player.firstFrame():
        player.setSpeed(0,0,0)
        player.executeMove(SweetMoves.SIT_POS)
        player.brain.tracker.stopHeadMoves()
        player.brain.sensors.resetSaveFrame()

    if player.stateTime > 8.0:
        shutoff = motion.StiffnessCommand(0.0)
        player.brain.motion.sendStiffness(shutoff)

    return player.stay()
