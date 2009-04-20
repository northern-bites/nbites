import man.motion.SweetMoves as SweetMoves
import man.noggin.NogginConstants as NogginConstants

SPIN_TIME = 360
WAIT_TIME = 45
WALK_TIME = 200
TARGET_X = NogginConstants.OPP_GOALBOX_LEFT_X
TARGET_Y = NogginConstants.CENTER_FIELD_Y

def gamePlaying(player):
    #player.brain.loc.reset()
    return player.goLater('goToPoint')

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
        return player.goNow('goToPoint')
    return player.stay()

def goToPoint(player):
    if player.firstFrame():
        player.brain.tracker.switchTo('locPans')
        player.brain.nav.goTo(TARGET_X, TARGET_Y)
        return player.stay()

    if player.brain.nav.isStopped():
        return player.goLater('doneState')

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
    if player.brain.nav.isStopped():
        return player.goLater('sitDown')
    return player.stay()

def sitDown(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.SIT_POS)
        return player.stay()

    if not player.brain.motion.isBodyActive():
        player.gainsOff()
        return player.goLater('doneDone')

    return player.stay()

def doneDone(player):
    return player.stay()
