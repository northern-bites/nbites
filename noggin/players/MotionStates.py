
import math

import man.motion as motion
import man.motion.SweetMoves as SweetMoves
import man.motion.RobotGaits as RobotGaits
import man.motion.MotionConstants as MotionConstants


TO_RAD= math.pi/180.

def gameInitial(player):
    if player.firstFrame():
        x = motion.StiffnessCommand(.85)
        player.brain.motion.sendStiffness(x)


    return player.stay()

def gameReady(player):
    if player.firstFrame():
        walkcmd = motion.WalkCommand(x=0,y=0,theta=0)
        player.brain.motion.setNextWalkCommand(walkcmd)

    return player.stay()

def gamePlaying(player):
    print "In the players version of game controller state (overridden)"

    return player.goLater('walkstraight')

def switchGaits(player):
    pass

def walkleft(player):
    if player.firstFrame():
        walkcmd = motion.WalkCommand(x=0,y=4,theta=0)
        player.brain.motion.setNextWalkCommand(walkcmd)

    if player.counter == 100:
        return player.goLater('sitdown')
    return player.stay()


def walkstraight(player):
    if player.firstFrame():
        walkcmd = motion.WalkCommand(x=4,y=0,theta=0)
        player.brain.motion.setNextWalkCommand(walkcmd)
    if player.counter == 200:
        return player.goLater('sitdown')
    return player.stay()

def switchGait():
    pass

def walkturn(player):
    if player.firstFrame():
        walkcmd = motion.WalkCommand(x=0,y=0,theta=20)
        player.brain.motion.setNextWalkCommand(walkcmd)
    if player.counter == 180:
        return player.goLater('stopwalking')
    return player.stay()

def stopwalking(player):
    ''' Do nothing'''
    if player.firstFrame():
        walkcmd = motion.WalkCommand(x=0,y=0,theta=0)
        player.brain.motion.setNextWalkCommand(walkcmd)
    return player.goLater('sitdown')
def standup(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.ZERO_POS)
    return player.stay()
def sitdown(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.SIT_POS)

    return player.stay()

def shutoffgains(player):
    if player.firstFrame():
        shutoff = motion.StiffnessCommand(0.0)
        player.brain.motion.sendStiffness(shutoff)

    return player.goLater('nothing')

def  nothing(player):
    return player.stay()

def gameFinished(player):
    if player.firstFrame():
        player.motion.stopBodyMoves()
        player.brain.tracker.stopHeadMoves()
        player.setHeads(0.,0.)
        player.executeMove(SweetMoves.SIT_POS)

    #Need time for sitdown and for waiting for the walk to stop
    #it would be better if we could see when the motion engine was inactive
    if player.stateTime > 8.0:
        return player.goNow('shutoffgains')
    return player.stay()
