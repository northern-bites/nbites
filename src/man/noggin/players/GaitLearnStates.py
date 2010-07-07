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
        player.brain.tracker.trackBall()
#        player.penalizeHeads();

    return player.goLater('stopandchangegait')

if WEBOTS_ACTIVE:
    gameInitial=gamePlaying
    print "Webots is active!!!!"
else:
    print "Webots is in-active!!!!"

def walkstraightstop(player):
    # TODO
    #X_STABILITY_WEIGHT
    #Y_STABILITY_WEIGHT

    if player.firstFrame():
        # TODO :: make this more flexible
        setWalkVector(player, 15,0,0)
        player.brain.stability.resetData()

    if player.counter == 400:
        # we optimize towards high stability
        frames_stood = player.counter

        stability = frames_stood #+

        swarm.getCurrParticle().setStability(stability)
        swarm.tickCurrParticle()

        return player.goNow('stopandchangegait')

    if player.counter % 5 == 0 and False:
        print "X stability variance: ", \
            player.brain.stability.getStability_X()
        print "Y stability variance: ", \
            player.brain.stability.getStability_Y()

    if player.brain.fallController.isFallen():
        print "(GaitLearning):: we've fallen down!"
        swarm.getCurrParticle().setStability(player.counter)
        swarm.tickCurrParticle()
        return player.goLater('standuplearn')

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
        player.setWalk(0,0,0)

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

        player.brain.CoA.setRobotDynamicGait(player.brain.motion, newGait)

        #print "gait in tuple form:"
        #for tuple in gaitTuple:
        #    print tuple

    if player.counter == 30:
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

def printloc(player):
    if player.firstFrame():
        player.printf("Loc (X,Y,H) (%g,%g,%g"% 
                      (player.brain.my.x,
                       player.brain.my.y,
                       player.brain.my.h))
    return player.stay()

def setWalkVector(player, x, y, theta):
    """
    Use this guy because all the wrappings through Nav tend
    to reset our gait parameters
    """
    walk = motion.WalkCommand(x=x,y=y,theta=theta)
    player.brain.motion.setNextWalkCommand(walk)
