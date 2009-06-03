
import math

import man.motion as motion
import man.motion.SweetMoves as SweetMoves
import man.motion.RobotGaits as RobotGaits
import man.motion.MotionConstants as MotionConstants


TO_RAD= math.pi/180.

# def gameInitial(player):
#     if player.firstFrame():
#         x = motion.StiffnessCommand(.85)
#         player.brain.motion.sendStiffness(x)


#     return player.stay()

# def gameReady(player):
#     if player.firstFrame():
#         walkcmd = motion.WalkCommand(x=0,y=0,theta=0)
#         player.brain.motion.setNextWalkCommand(walkcmd)

#     return player.stay()

def gamePlaying(player):
    print "In the players version of game controller state (overridden)"
    if player.firstFrame():
        player.gainsOn()
        player.brain.CoA.setRobotTurnGait(player.brain.motion)

    return player.goLater('walkstraightstop')

#gameInitial=gamePlaying

def switchGaits(player):
    pass

def walkleft(player):
    if player.firstFrame():
        player.setSpeed(0,4,0)

    if player.counter == 100:
        return player.goLater('turnright')
    return player.stay()

def walkright(player):
    if player.firstFrame():
        player.setSpeed(0,-4,0)

    if player.counter == 100:
        return player.goLater('walkleft')
    return player.stay()

def walktest(player):
    if player.firstFrame():
        player.setSpeed(4,0,0)

    if player.counter  == 100:
        player.brain.motion.resetWalk()
        #player.executeMove(SweetMoves.ZERO_POS)

    if player.counter == 200:
        player.setSpeed(4,0,0)

    if player.counter == 300:
        return player.goLater('stopwalking')

    return player.stay()

def walkstraight(player):
    if player.firstFrame():
        player.setSpeed(4,0,0)

    if player.counter == 200:
        return player.goLater('walkright')
    return player.stay()


def walkstraightstop(player):
    if player.firstFrame():
        player.setSpeed(8.0,0,0)

    if player.counter == 200:
        return player.goLater('stopwalking')
    return player.stay()

def turnleft(player):
    if player.firstFrame():
        player.setSpeed(0,0,15)

    if player.counter == 180:
        return player.goLater('stopwalking')
    return player.stay()

def turnright(player):
    if player.firstFrame():
        player.setSpeed(0,0,-15)
    if player.counter == 180:
        return player.goLater('turnleft')
    return player.stay()


def stopwalking(player):
    ''' Do nothing'''
    if player.firstFrame():
        player.setSpeed(0,0,0)

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
