
import math

import man.motion as motion
import man.motion.SweetMoves as SweetMoves
import man.motion.MotionConstants as MotionConstants

def nothing(player):
    """
    Do nothing
    """

    if player.counter == 10:
        return player.goLater('walking')
    return player.stay()

def gameInitial(player):
    print "In the players version of game controller state (overridden)"
    return player.goNow('nothing')

def moveHeadInitial(player):
    if player.firstFrame():
        time = 2.0
        headjoints = (0.,20.0)
        type = 1

        moveHead = motion.HeadJointCommand(time,headjoints,type)
        print "enqueing head motion in python"
        player.brain.motion.enqueue(moveHead)

    if player.counter == 10:
        return player.goLater('moveHeadLeft')    
    return player.stay()

def moveHeadLeft(player):
    if player.firstFrame():
        time = 4.0
        headjoints = (40.,20.0)
        type = 1
        
        moveHead = motion.HeadJointCommand(time,headjoints,type)
        print "enqueing head motion in python"
        player.brain.motion.enqueue(moveHead)
        
    if player.counter == 10:
        return player.goLater('moveHeadRight')
    return player.stay()

def moveHeadRight(player):
    if player.firstFrame():
        time = 4.0
        headjoints = (-40.,20.0)
        type = 1
        
        moveHead = motion.HeadJointCommand(time,headjoints,type)
        print "enqueing head motion in python"
        player.brain.motion.enqueue(moveHead)

    if player.counter == 10:
        return player.goLater('moveHeadLeft')
    return player.stay()

def standup(player):
    return player.stay()

def walking(player):
    if player.firstFrame():
        walkcmd = motion.WalkCommand(x=6,y=0,theta=0)
        #player.brain.motion.setNextWalkCommand(walkcmd)
    return player.goLater('done')

def done(player):
    ''' Do nothing'''
    return player.stay()

