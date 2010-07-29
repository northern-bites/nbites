import man.motion as motion
import man.motion.SweetMoves as SweetMoves
import man.motion.RobotGaits as RobotGaits
import man.motion.MotionConstants as MotionConstants
import man.noggin.typeDefs.Location as Location
import man.noggin.navigator.NavHelper as helper
from ..WebotsConfig import WEBOTS_ACTIVE
import man.noggin.util.PSO as PSO
from man.motion.gaits.GaitLearnBoundaries import \
    gaitMins, gaitMaxs, \
    gaitToArray, arrayToGaitTuple

from os.path import isfile
from math import fabs
import random

# Webots Controller functions, so we can do supervisor stuff
# for the import to work you must also copy add Webots/lib/ to
# your library search path. Hack Hack Hack.  -Nathan

if WEBOTS_ACTIVE:
    PICKLE_FILE_PREFIX = ''
    try:
        import sys
        sys.path.append('/Applications/Webots/lib/python')
        from controller import *
    except:
        print "could not load webots controller libraries"
else:
    PICKLE_FILE_PREFIX = '/home/nao/gaits/'

try:
   import cPickle as pickle
except:
   import pickle

PSO_STATE_FILE = PICKLE_FILE_PREFIX + "PSO_pGaitLearner.pickle"
BEST_GAIT_FILE = PICKLE_FILE_PREFIX + "PSO_endGait.pickle."
OPTIMIZE_FRAMES = 1000
RANDOM_WALK_DURATION = 150

SWARM_ITERATION_LIMIT = 25 # wikipedia says this should be enough to converge
NUM_PARTICLES = 30

POSITION_UPDATE_FRAMES = 15
MINIMUM_REQUIRED_DISTANCE = 100
DISTANCE_PENALTY = -400

RUN_ONCE_STOP = True

def gamePlaying(player):
    print "In the players version of game controller state (overridden)"
    if player.firstFrame():
        player.gainsOn()
        player.brain.tracker.stopHeadMoves()

        startPSO(player)

    if WEBOTS_ACTIVE:
       enableGPS(player)
    else:
       player.haveWbGPS = False

    return player.goLater('stopandchangegait')

if WEBOTS_ACTIVE:
    gameInitial=gamePlaying
    print "Webots is active!!!!"
else:
    print "Webots is in-active!!!!"

def gamePenalized(player):
    if player.firstFrame():
        player.stopWalking()
        player.penalizeHeads()

    return player.stay()

def walkstraightstop(player):
    stability = player.brain.stability

    stability.updateStability()

    if player.firstFrame():
        setWalkVector(player, 1,0,0)
        stability.resetData()
        player.straightWalkCounter = 0

        player.startOptimizeLocation = getCurrentLocation(player)

    # too computationally expensive to use every point
    if player.counter % POSITION_UPDATE_FRAMES == 0:
        stability.updatePosition(getCurrentLocation(player))

    if robotFellOver(player):
        player.endStraightWalkLoc = getCurrentLocation(player)
        player.straightWalkCounter = player.counter

        scoreGaitPerformance(player)

        print "(GaitLearning):: we've fallen down!"

        return player.goLater('newOptimizeParameters')

    if player.counter == OPTIMIZE_FRAMES:
        player.endStraightWalkLoc = getCurrentLocation(player)
        player.straightWalkCounter = player.counter

        # gaits that don't move at all can sometimes crash webots when it 
        # tells them to omni walk - there is a malloc() bug in the motion
        # engine somewhere that needs to be tracked down
        if distancePenalty(player,
                           player.endStraightWalkLoc.distTo(player.startOptimizeLocation)) < 0:
            scoreGaitPerformance(player)
            return player.goLater('newOptimizeParameters')

        return player.goLater('timedRandomWalk')

    return player.stay()

def timedRandomWalk(player):
    stability = player.brain.stability

    stability.updateStability()

    if robotFellOver(player):
        scoreGaitPerformance(player)

        print "(GaitLearning):: we've fallen down!"

        return player.goLater('newOptimizeParameters')

    if player.counter % RANDOM_WALK_DURATION == 0 or \
            player.firstFrame():
        r_x = random.uniform(.5, 1) # we're making forwards gaits
        r_y = random.uniform(-1, 1)
        r_theta = random.uniform(-.5, .5)

        print "set random walk vector to :", r_x, r_y, r_theta

        setWalkVector(player,
                      r_x,
                      r_y,
                      r_theta)

    if player.counter == OPTIMIZE_FRAMES:
        scoreGaitPerformance(player)
        return player.goLater('newOptimizeParameters')

    return player.stay()

def scoreGaitPerformance(player):
   stability = player.brain.stability

   frames_stood = player.counter + player.straightWalkCounter # (sometimes 0)
   path_linearity = stability.getPathLinearity()
   stability_penalty = stability.getStabilityHeuristic()

## TODO:
# use ball or post distance to determine distance traveled on Nao
   if WEBOTS_ACTIVE:
       distance_traveled = player.endStraightWalkLoc.distTo \
           (player.startOptimizeLocation)
   else:
       distance_traveled = 0

   # we maximize on the heuristic
   heuristic = frames_stood \
       + distancePenalty(player, distance_traveled) \
       + path_linearity \
       - stability_penalty

   print "total distance traveled with this gait is ", \
       distancePenalty(player, distance_traveled)
   print "robot stood for %s frames during random walk period" % \
       (frames_stood - player.straightWalkCounter)
   print "heuristic for this run is %s" % heuristic

   player.swarm.getCurrParticle().setHeuristic(heuristic)
   player.swarm.tickCurrParticle()
   savePSO(player)

def newOptimizeParameters(player):
   """
   Controls what we do after every optimization run
   pointer state to more interesting places
   """
   if player.swarm.getIterations() > SWARM_ITERATION_LIMIT:
      print "Swarm is done optimizing!"
      return player.goLater('reportBestGait')

   elif RUN_ONCE_STOP:
      if WEBOTS_ACTIVE:
         revertWebots(player)
      else:
         return player.goLater('stopwalking')
   else:
      return player.goLater('stopandchangegait')

def stopwalking(player):
    ''' Do nothing'''
    if player.firstFrame():
        setWalkVector(player, 0,0,0)

    return player.stay()

def stopandchangegait(player):
    '''Set new gait and start walking again'''

    if player.firstFrame():
        setWalkVector(player, 0,0,0)

        gaitTuple = arrayToGaitTuple(player.swarm.getNextParticle().getPosition())

        setGait(player, gaitTuple)

    if player.counter == 100:
        return player.goLater('walkstraightstop')

    return player.stay()

def reportBestGait(player):
   if player.firstFrame():
      (bestGaitArray, gaitScore) = player.swarm.getBestSolution()
      bestGaitTuple = arrayToGaitTuple(bestGaitArray)

      print "best found gait's heuristic score was: ", gaitScore

      try:
         gaitScore = int(gaitScore)
         output = BEST_GAIT_FILE + str(gaitScore)

         i = 1
         while isfile(output):
            output = BEST_GAIT_FILE + str(gaitScore) + "." + str(i)
            i += 1

         print "best gait saved to file: ", output
         f = open(output, 'w')
         pickle.dump(bestGaitTuple, f)
         f.close()

      except:
         print "error pickling gait"

   return player.goLater('restartOptimization')

def restartOptimization(player):
   if player.firstFrame():
      newPSO(player)
      savePSO(player)

   return player.goLater('newOptimizeParameters')

def printloc(player):
    if player.firstFrame():
        player.printf("Loc (X,Y,H) (%g,%g,%g"%
                      (player.brain.my.x,
                       player.brain.my.y,
                       player.brain.my.h))
    return player.stay()

def startPSO(player):
    if isfile(PSO_STATE_FILE):
        loadPSO(player)
    else:
        newPSO(player)

def savePSO(player):
    print "Saving PSO state to: ", PSO_STATE_FILE
    f = open(PSO_STATE_FILE, 'w')
    pickle.dump(player.swarm, f)
    f.close()

def newPSO(player):
   print "Initializing new PSO"
   player.swarm = PSO.Swarm(NUM_PARTICLES,
                            44, gaitToArray(gaitMins), gaitToArray(gaitMaxs))

def loadPSO(player):
    print "Loading PSO state from: ", PSO_STATE_FILE
    f = open(PSO_STATE_FILE, 'r')
    player.swarm = pickle.load(f)
    f.close()

def revertWebots(player):
    '''Uses WB supervisor calls to revert the simulator'''
    supervisor = Supervisor()
    supervisor.simulationRevert()

def enableGPS(player):
    try:
        player.wbGPS = GPS('gps')
        player.wbGPS.enable(30)
        player.haveWbGPS = True
        print "Enabled Webots GPS"
    except:
        player.haveWbGPS = False

def setWalkVector(player, x, y, theta):
    """
    Use this guy because all the wrappings through Nav tend
    to reset our gait parameters
    """
    x_cms, y_cms, theta_degs = helper.convertWalkVector(player.brain, x, y, theta)

    walk = motion.WalkCommand(x=x_cms,y=y_cms,theta=theta_degs)
    player.brain.motion.setNextWalkCommand(walk)

def robotFellOver(player):
    if WEBOTS_ACTIVE:
        return player.brain.stability.isWBFallen()
    else:
        return player.brain.fallController.isFallen()

def setGait(player, gaitTuple):
    newGait = motion.GaitCommand(gaitTuple[0],
                                 gaitTuple[1],
                                 gaitTuple[2],
                                 gaitTuple[3],
                                 gaitTuple[4],
                                 gaitTuple[5],
                                 gaitTuple[6],
                                 gaitTuple[7])

    player.brain.CoA.setRobotDynamicGait(player.brain.motion, newGait)

def getCurrentLocation(player):
    if player.haveWbGPS:
        wbPosition = player.wbGPS.getValues()
        return Location.Location(wbPosition[0],
                                 wbPosition[1],
                                 player.brain.my.h)
    else:
        return Location.Location(player.brain.my.x,
                                 player.brain.my.y,
                                 player.brain.my.h)


def distancePenalty(player, distance_traveled):
    weight = 1

    # webots uses different units than our localization
    if player.haveWbGPS:
        weight = 100

    weightedDistance = weight * distance_traveled

    # prevents gaits that don't move at all
    if weightedDistance < MINIMUM_REQUIRED_DISTANCE:
        return DISTANCE_PENALTY

    else:
        return weightedDistance
