import man.motion.SweetMoves as SweetMoves
import man.motion as motion

SPIN_TIME = 180
WAIT_TIME = 15
WALK_TIME = 600

def gamePlaying(player):
    player.brain.loc.reset()
    return player.goNow('spinLocalize')

def spinLocalize(player):
    if player.firstFrame():
        player.brain.tracker.switchTo('locPans')
        player.setSpeed(0,0,15)
    if player.counter == SPIN_TIME:
        player.setSpeed(0,0,0)
        return player.goNow('waitToMove')

    return player.stay()

def waitToMove(player):
    if player.counter > WAIT_TIME:
        return player.goNow('walkForward')
    return player.stay()

def walkForward(player):
    if player.firstFrame():
        player.setSpeed(4,0,0)
    if player.counter > WALK_TIME:
        return player.goLater(doneState)
    return player.stay()

def doneState(player):
    if player.firstFrame():
        player.setSpeed(0,0,0)
        player.executeMove(SweetMoves.SIT_POS)
        player.brain.tracker.stopHeadMoves()
        #player.brain.sensors.resetSaveFrame()

    if player.stateTime > 8.0:
        shutoff = motion.StiffnessCommand(0.0)
        player.brain.motion.sendStiffness(shutoff)

    return player.stay()
