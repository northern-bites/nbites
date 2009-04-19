import man.motion.SweetMoves as SweetMoves
import man.motion as motion

SPIN_TIME = 180
WAIT_TIME = 45
WALK_TIME = 200

def gamePlaying(player):
    player.brain.loc.reset()
    return player.goNow('spinLocalize')

def spinLocalize(player):
    if player.firstFrame():
        player.brain.tracker.switchTo('locPans')
        player.setSpeed(0,2,15)
    if player.counter == SPIN_TIME:
        player.stopWalking()
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
        return player.goLater('doneState')
    return player.stay()

def doneState(player):
    if player.firstFrame():
        player.stopWalking()
        player.brain.tracker.stopHeadMoves()
    if player.brain.nav.currentState == 'stopped':
        return player.goLater('sitDown')
    return player.stay()

def sitDown(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.SIT_POS)

    if not motion.isBodyActive:
        player.gainsOff()

    return player.stay()
