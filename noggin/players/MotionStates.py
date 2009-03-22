
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
        gait = motion.GaitCommand(31.00, # com height
                                  1.40,  # hip offset x
                                  0.50,   # step duration
                                  0.1,   # fraction in double support
                                  1.65,  # stepHeight
                                  0.0,  # footLengthX
                                  0.4,   # zmp static percentage
                                  5.0,   # left swing hip roll addition
                                  5.0,   # right swing hip roll addition
                                  0.90,  # left zmp off
                                  0.90,  # right zmp off
                                  10.0,  # max x speed
                                  5.0,   # max y speed
                                  30.0)  # max theta speed
        player.brain.motion.setGait(gait)
    return player.stay()
    #return player.goLater('walkstraight')

def gameReady(player):
    if player.firstFrame():
        walkcmd = motion.WalkCommand(x=0,y=0,theta=0)
        player.brain.motion.setNextWalkCommand(walkcmd)

    return player.stay()

def gamePlaying(player):
    print "In the players version of game controller state (overridden)"
    # gait = motion.GaitCommand(31.00,
#                               1.90,
#                               0.5,
#                               0.1,
#                               1.65,
#                               1.00,
#                               0.4,
#                               4.0,
#                               4.0,
#                               1.50,
#                               1.50,
#                               10.0,
#                               5.0,
#                               30.0)
#    player.brain.motion.setGait(gait)
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

def  nothing(player):
    return player.stay()
