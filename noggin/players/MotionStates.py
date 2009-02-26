
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
    print "In the players version of game controller state (overridden)"
    gait = motion.GaitCommand(310.0,
                              19.0,
                              0.5,
                              0.1,
                              16.5,
                              10.0,
                              0.4,
                              4.0*TO_RAD,
                              4.0*TO_RAD,
                              15.0,
                              15.0)
    player.brain.motion.setGait(gait)

    return player.goLater('walkleft')

def switchGaits(player):
    pass

def walkleft(player):
    if player.firstFrame():
        walkcmd = motion.WalkCommand(x=0,y=1,theta=0)
        player.brain.motion.setNextWalkCommand(walkcmd)
    if player.counter == 30:
        return player.goLater('sitdown')
    return player.stay()


def walkstraight(player):
    if player.firstFrame():
        walkcmd = motion.WalkCommand(x=6,y=0,theta=0)
        player.brain.motion.setNextWalkCommand(walkcmd)
    if player.counter == 80:
        return player.goLater('sitdown')
    return player.stay()

def switchGait():
    pass

def walkturn(player):
    if player.firstFrame():
        walkcmd = motion.WalkCommand(x=1,y=1,theta=10)
        player.brain.motion.setNextWalkCommand(walkcmd)
    if player.counter == 30:
        return player.goLater('stopwalking')
    return player.stay()

def stopwalking(player):
    ''' Do nothing'''
    if player.firstFrame():
        walkcmd = motion.WalkCommand(x=0,y=0,theta=0)
        player.brain.motion.setNextWalkCommand(walkcmd)
    return player.goLater('sitdown')

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

def  nothing(player):
    return player.stay()
