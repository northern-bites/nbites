
import math

import man.motion as motion
import man.motion.SweetMoves as SweetMoves
import man.motion.RobotGaits as RobotGaits
import man.motion.MotionConstants as MotionConstants
from ..WebotsConfig import WEBOTS_ACTIVE

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
        player.brain.CoA.setRobotGait(player.brain.motion)
        player.brain.tracker.trackBall()
#        player.penalizeHeads();

    return player.goLater('switchgaits1')

if WEBOTS_ACTIVE:
    gameInitial=gamePlaying
    print "Webots is active!!!!"
else:
    print "Webots is in-active!!!!"

def switchgaits1(player):
    if player.firstFrame():
        player.setWalk(6,-4,10)

    if player.counter == 140:
        return player.goLater('switchgaits2')
    return player.stay()

def switchgaits2(player):
    if player.firstFrame():
        player.brain.CoA.setRobotDribbleGait(player.brain.motion)
        player.setWalk(10,-10,20)

    if player.counter == 240:
        return player.goLater('switchgaits3')
    return player.stay()
def switchgaits3(player):
    if player.firstFrame():
        player.brain.CoA.setRobotGait(player.brain.motion)

    if player.counter == 140:
        return player.goLater('sitdown')
    return player.stay()



def walkleft(player):
    if player.firstFrame():
        player.setWalk(0,4,0)

    if player.counter == 100:
        return player.goLater('turnright')
    return player.stay()

def walkright(player):
    if player.firstFrame():
        player.setWalk(0,-4,0)

    if player.counter == 100:
        return player.goLater('walkleft')
    return player.stay()

def walktest(player):
    if player.firstFrame():
        player.setWalk(4,0,0)

    if player.counter  == 100:
        player.brain.motion.resetWalk()
        #player.executeMove(SweetMoves.ZERO_POS)

    if player.counter == 200:
        player.setWalk(4,0,0)

    if player.counter == 300:
        return player.goLater('stopwalking')

    return player.stay()

def walkstraight(player):
    if player.firstFrame():
        player.setWalk(4,0,0)

    if player.counter == 200:
        return player.goLater('walkright')
    return player.stay()


def walkstraightstop(player):
    if player.firstFrame():
        player.setWalk(7,0,0)

    if player.counter == 300:
        return player.goLater('stopwalking')
    return player.stay()

def walkturnstop(player):
    if player.firstFrame():
        player.setWalk(0.0,0,10)

    if player.counter == 500:
        return player.goLater('stopwalking')
    return player.stay()

def turnleft(player):
    if player.firstFrame():
        player.setWalk(0,0,15)

    if player.counter == 180:
        return player.goLater('stopwalking')
    return player.stay()

def turnright(player):
    if player.firstFrame():
        player.setWalk(0,0,-15)
    if player.counter == 180:
        return player.goLater('turnleft')
    return player.stay()


def stopwalking(player):
    ''' Do nothing'''
    if player.firstFrame():
        player.stopWalking()

    return player.goLater('sitdown')

def standup(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.ZERO_POS)
    return player.stay()

def sitdown(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.SIT_POS)

    elif player.brain.motion.isBodyActive():
        return player.goLater("printloc")

    return player.stay()

def shutoffgains(player):
    if player.firstFrame():
        shutoff = motion.StiffnessCommand(0.0)
        player.brain.motion.sendStiffness(shutoff)

    return player.goLater('nothing')


def odotune(player):
    if player.firstFrame():
        player.brain.loc.reset()
        player.setWalk(6.0,0,10)

    if player.counter == 400:
        return player.goLater('odostop')
    return player.stay()

def odostop(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.SIT_POS)

    elif not  player.brain.motion.isBodyActive():
        return player.goLater("printloc")

    return player.stay()

def  printloc(player):
    if player.firstFrame():
        player.printf("Loc (X,Y,H) (%g,%g,%g"% 
                      (player.brain.my.x,
                       player.brain.my.y,
                       player.brain.my.h))
    return player.stay()
