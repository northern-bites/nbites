import man.motion.SweetMoves as SweetMoves
import man.noggin.NogginConstants as NogginConstants

SPIN_TIME = 360
WAIT_TIME = 45
WALK_TIME = 200
TARGET_X = NogginConstants.OPP_GOALBOX_LEFT_X
TARGET_Y = NogginConstants.CENTER_FIELD_Y
COUNT_TOTAL = 250

def gamePlaying(player):
    player.brain.loc.reset()
    player.brain.out.startLocLog()
    player.DIST += 50
    return player.goLater('goToPoint')

def collectDistData(player):
    if player.firstFrame():
        player.distData = []
        player.widthData = []
        player.heightData = []
        player.brain.tracker.switchTo('locPans')

    if player.brain.ygrp.on:
        player.distData.append(player.brain.ygrp.dist)
        player.widthData.append(player.brain.ygrp.width)
        player.heightData.append(player.brain.ygrp.height)
        player.printf("YGGP is on")
        player.printf("Count is " + str(len(player.distData)))

    if player.brain.yglp.on:
        player.printf("YGLP is on")

    if len(player.distData) == COUNT_TOTAL:
        return player.goLater('doneCollecting')

    return player.stay()

def doneCollecting(player):
    if player.firstFrame():
        distMean = sum(player.distData) / COUNT_TOTAL
        heightMean = sum(player.heightData) / COUNT_TOTAL
        widthMean = sum(player.widthData) / COUNT_TOTAL
        player.brain.out.stopLocLog()
        player.brain.tracker.stopHeadMoves()

        player.printf("At dist " + str(player.DIST) + " over " +
                      str(COUNT_TOTAL) + " frames YGRP:")
        player.printf("\t average dist is " + str(distMean))
        player.printf("\t average height is " + str(heightMean))
        player.printf("\t average width is " + str(widthMean))

    return player.stay()

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
        player.brain.out.stopLocLog()
        return player.stay()

    if not player.brain.motion.isBodyActive():
        player.gainsOff()
        return player.goLater('doneDone')

    return player.stay()

def doneDone(player):
    return player.stay()
