
import math

import man.motion as motion
import man.motion.SweetMoves as SweetMoves
import man.motion.RobotGaits as RobotGaits
import man.motion.MotionConstants as MotionConstants
from ..WebotsConfig import WEBOTS_ACTIVE
import man.noggin.util.PSO as PSO
from man.motion.gaits.GaitLearnBoundaries import gaitMins, gaitMaxs, gaitToArray, arrayToGaitTuple

#def gameInitial(player):
#    return player.stay()

# def gameReady(player):
#     if player.firstFrame():
#         walkcmd = motion.WalkCommand(x=0,y=0,theta=0)
#         player.brain.motion.setNextWalkCommand(walkcmd)

#     return player.stay()

# set up the PSO!
#

# Swarm(#particles, dimensionality, [mins], [maxs])
swarm = PSO.Swarm(20, 44, gaitToArray(gaitMins), gaitToArray(gaitMaxs))

def gamePlaying(player):
    print "In the players version of game controller state (overridden)"
    if player.firstFrame():
        player.gainsOn()
        player.brain.CoA.setRobotGait(player.brain.motion)
        player.brain.tracker.trackBall()
#        player.penalizeHeads();

    return player.goLater('stopandchangegait')

if WEBOTS_ACTIVE:
    gameInitial=gamePlaying
    print "Webots is active!!!!"
else:
    print "Webots is in-active!!!!"

def switchgaits1(player):
    if player.firstFrame():
        player.setSpeed(6,-4,0)

    if player.counter == 140:
        return player.goLater('switchgaits2')
    return player.stay()

def switchgaits2(player):
    if player.firstFrame():
        player.brain.CoA.setRobotGait(player.brain.motion)
        player.setSpeed(10,-10,0)

    if player.counter == 240:
        return player.goLater('switchgaits3')
    return player.stay()

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
        player.setSpeed(6,0,0)

    if player.counter == 400:
        return player.goLater('walkfast')
    return player.stay()

def walkfast(player):
    if player.firstFrame():
        player.setSpeed(10,0,0)

    if player.counter == 400:
        return player.goLater('walkright')
    return player.stay()

def walkstraightstop(player):
    if player.firstFrame():
        player.setSpeed(7,0,0)

    if player.counter == 400:
        # stability is number of frames we stood plus accX/Y stats (higher is better)
        stability = player.counter # + something involving avgX/Y

        swarm.getCurrParticle().setStability(stability)
        swarm.tickCurrParticle()

        return player.goLater('stopandchangegait')

    # collect inertial data every 10 frames
    if player.counter % 10 == 0:
        accX = player.brain.sensors.inertial.accX
        accY = player.brain.sensors.inertial.accY
        accZ = player.brain.sensors.inertial.accZ

        #print "accX/Y/Z: %.3f / %.3f / %.3f" % (accX, accY, accZ)

        # we have fallen down! report stability as the number of frames we stayed up
        if accZ > -5:
            print "(GaitLearning):: we've fallen down!"
            swarm.getCurrParticle().setStability(player.counter)
            swarm.tickCurrParticle()
            return player.goLater('standuplearn')

    return player.stay()

def walkturnstop(player):
    if player.firstFrame():
        player.setSpeed(0.0,0,10)

    if player.counter == 500:
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

    if player.counter == 200:
        return player.goLater('walkstraightstop')

    return player.stay()

def stopandchangegait(player):
    '''Return our stability to the PSO
    set new gait, and start walking again'''
    if player.firstFrame():
        player.setSpeed(0,0,0)

        # set gait from new particle
        gaitTuple = arrayToGaitTuple(swarm.getNextParticle().getPosition())

        newGait = motion.GaitCommand(gaitTuple[0],
                                     gaitTuple[1],
                                     gaitTuple[2],
                                     gaitTuple[3],
                                     gaitTuple[4],
                                     gaitTuple[5],
                                     gaitTuple[6],
                                     gaitTuple[7])

        player.brain.CoA.setDynamicGait(player.brain.motion, newGait)
        #player.brain.CoA.setRobotGait(player.brain.motion)

        #print "gait in tuple form:"
        #for tuple in gaitTuple:
        #    print tuple

    if player.counter == 50:
        return player.goLater('walkstraightstop')

    return player.stay()

def standup(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.ZERO_POS)
    return player.stay()

def standuplearn(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.ZERO_POS)

    return player.goLater('stopandchangegait')

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
        player.setSpeed(6.0,0,10)

    if player.counter == 400:
        return player.goLater('odostop')
    return player.stay()

def odostop(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.SIT_POS)

    elif not  player.brain.motion.isBodyActive():
        return player.goLater("printloc")

    return player.stay()

def printloc(player):
    if player.firstFrame():
        player.printf("Loc (X,Y,H) (%g,%g,%g"% 
                      (player.brain.my.x,
                       player.brain.my.y,
                       player.brain.my.h))
    return player.stay()
