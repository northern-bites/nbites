
import math

import man.motion as motion
import man.motion.SweetMoves as SweetMoves
import man.motion.MotionConstants as MotionConstants

sitDownAngles = SweetMoves.SIT_POS[0]

# ((1.57,0.0,-1.13,-1.0),\
#      (0.0,0.0,-0.96,2.18,\
#           -1.22,0.0),(0.0,0.0,\
#                           -0.96,2.18,-1.22,0.0),\
#      (1.57,0.0,1.13,1.01), 3.0, 1)
TO_RAD= math.pi/180.

def gameInitial(player):
    if player.firstFrame():
        x = motion.StiffnessCommand(.85)
        player.brain.motion.sendStiffness(x)
    return player.stay()
    #return player.goLater('walkstraight')

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
        walkcmd = motion.WalkCommand(x=0,y=10,theta=10)
        player.brain.motion.setNextWalkCommand(walkcmd)

    if player.counter == 20:
        return player.goLater('sitdown')
    return player.stay()


def walkstraight(player):
    if player.firstFrame():
        walkcmd = motion.WalkCommand(x=6,y=0,theta=0)
        player.brain.motion.setNextWalkCommand(walkcmd)
    if player.counter == 200:
        return player.goLater('sitdown')
    return player.stay()

def switchGait():
    pass

def walkturn(player):
    if player.firstFrame():
        walkcmd = motion.WalkCommand(x=0,y=0,theta=10)
        player.brain.motion.setNextWalkCommand(walkcmd)
    if player.counter == 80:
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
        move = motion.BodyJointCommand(sitDownAngles[4], #time
                                       sitDownAngles[0], #larm
                                       sitDownAngles[1], #lleg
                                       sitDownAngles[2], #rleg
                                       sitDownAngles[3], #rarm
                                       sitDownAngles[5], #interpolation time
                                       )
        player.brain.motion.enqueue(move)

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
