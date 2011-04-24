import man.motion as motion
import man.motion.SweetMoves as SweetMoves
import man.motion.RobotGaits as RobotGaits
import man.motion.MotionConstants as MotionConstants
import man.noggin.typeDefs.Location as Location
import man.noggin.navigator.NavHelper as helper
import man.noggin.navigator.WalkHelper as walkhelper

import man.noggin.util.PSO as PSO
from man.motion.gaits.GaitLearnBoundaries import \
    gaitMins, gaitMaxs, \
    gaitToArray, arrayToGaitTuple

from os.path import isfile, dirname, exists
from os import makedirs
from math import fabs
import random

PICKLE_FILE_PREFIX = '/home/nao/gaits/'
OPTIMIZE_FRAMES = 200
NUM_PARTICLES = 10

try:
   import cPickle as pickle
except:
   import pickle

PSO_STATE_FILE = PICKLE_FILE_PREFIX + "PSO_pGaitLearner.pickle"
BEST_GAIT_FILE = PICKLE_FILE_PREFIX + "PSO_endGait.pickle."

SWARM_ITERATION_LIMIT = 25 # wikipedia says this should be enough to converge

RANDOM_WALK_DURATION = 150

POSITION_UPDATE_FRAMES = 15
MINIMUM_REQUIRED_DISTANCE = 100
DISTANCE_PENALTY = -400

RUN_ONCE_STOP = True

def gamePlaying(player):
    player.print("In the pGaitLearner's version of game controller state (overridden)")
    if player.firstFrame():
        player.gainsOn()
        player.brain.tracker.trackBall()

        # we don't want to auto-stand after falling
        player.brain.fallController.executeStandup = False
        player.brain.fallController.FALLEN_THRESH = 60
        player.brain.fallController.FALL_COUNT_THRESH = 10

        startPSO(player)

    return player.goLater('stopandchangegait')

def gamePenalized(player):
    if player.firstFrame():
        player.stopWalking()
        player.penalizeHeads()

    return player.stay()

def walkstraightstop(player):
    stability = player.brain.stability

    stability.updateStability()

    if player.firstFrame():
        stability.resetData()
        player.straightWalkCounter = 0

        ball_x = 1.0
        ball_y = ball_theta = 0              # TODO
        setWalkVector(player, ball_x, ball_y, ball_theta)

        player.startBallDistance = player.brain.ball.lastVisionDist

    # @todo rework this?
    #stability.updatePosition(getCurrentLocation(player))

    if robotFellOver(player):
        player.gainsOff()

        player.endStraightWalkLoc = getCurrentLocation(player)
        player.straightWalkCounter = player.counter

        scoreGaitPerformance(player)

        player.printf("(GaitLearning):: we've fallen down!\n")

        return player.goLater('newOptimizeParameters')

    if player.counter == OPTIMIZE_FRAMES:
        player.endStraightWalkLoc = getCurrentLocation(player)
        player.straightWalkCounter = player.counter

        scoreGaitPerformance(player)

        return player.goLater('newOptimizeParameters')

    return player.stay()
            # on a real robot, walk towards the ball at a slower speed
def timedRandomWalk(player):
    stability = player.brain.stability

    stability.updateStability()

    if robotFellOver(player):
        scoreGaitPerformance(player)

        player.printf("(GaitLearning):: we've fallen down!")

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

   stability_penalty = stability.getStabilityHeuristic()
   path_linearity = stability.getPathLinearity()

   frames_stood = player.straightWalkCounter # no random walk on the Nao

   # on the Nao, use distance to a stationary ball
   # @todo remove ball distance stuff
   distance_traveled = player.startBallDistance - player.brain.ball.lastVisionDist
   player.printf("difference in ball distance %s - %s = %s " %
                  (player.startBallDistance,
                   player.brain.ball.lastVisionDist,
                   distance_traveled))

   # we maximize on the heuristic
   heuristic = frames_stood \
       + path_linearity \
       - stability_penalty

   player.printf("total distance traveled with this gait is ",
                 distancePenalty(player, distance_traveled))
   player.printf("stability penalty is %s" % stability_penalty)
   player.printf("robot stood for %s frames during random walk period" %
                 (frames_stood - player.straightWalkCounter))
   player.printf(print "heuristic for this run is %s" % heuristic)

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

    if player.counter == 50:
        return player.goLater('walkstraightstop')

    return player.stay()

def reportBestGait(player):
   if player.firstFrame():
      (bestGaitArray, gaitScore) = player.swarm.getBestSolution()
      bestGaitTuple = arrayToGaitTuple(bestGaitArray)

      player.printf("best found gait's heuristic score was: ", gaitScore)

      try:
         gaitScore = int(gaitScore)
         output = BEST_GAIT_FILE + str(gaitScore)

         i = 1
         while isfile(output):
            output = BEST_GAIT_FILE + str(gaitScore) + "." + str(i)
            i += 1

      player.printf("best gait saved to file: ", output)
         f = open(output, 'w')
         pickle.dump(bestGaitTuple, f)
         f.close()

      except:
         player.printf("error pickling gait")

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
    player.printf("Saving PSO state to: ", PSO_STATE_FILE)
    ensure_dir(PSO_STATE_FILE)
    f = open(PSO_STATE_FILE, 'w')
    pickle.dump(player.swarm, f)
    f.close()

def newPSO(player):
    player.printf("Initializing new PSO")
    player.swarm = PSO.Swarm(NUM_PARTICLES,
                             44, gaitToArray(gaitMins), gaitToArray(gaitMaxs))

def loadPSO(player):
    player.printf("Loading PSO state from: ", PSO_STATE_FILE)
    ensure_dir(PSO_STATE_FILE)
    f = open(PSO_STATE_FILE, 'r')
    player.swarm = pickle.load(f)
    f.close()


def setWalkVector(player, x, y, theta):
    """
    Use this guy because all the wrappings through Nav tend
    to reset our gait parameters
    """
    x_cms, y_cms, theta_degs = helper.convertWalkVector(player.brain, x, y, theta)

    walk = motion.WalkCommand(x=x_cms,y=y_cms,theta=theta_degs)
    player.brain.motion.setNextWalkCommand(walk)

def robotFellOver(player):
    return player.brain.fallController.isFallen()

def setGait(player, gaitTuple):
    """
    Builds a new gait using gaitTuple, creates it as a GaitCommand
    and sets it as the Robot's current gait. No bounds checking is done 
    here, so you can give the robot horrible values.
    """
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
    return Location.Location(player.brain.my.x,
                             player.brain.my.y,
                             player.brain.my.h)


def distancePenalty(player, distance_traveled):
    weight = 1 # @todo decide about this

    weightedDistance = weight * distance_traveled

    # prevents gaits that don't move at all
    if weightedDistance < MINIMUM_REQUIRED_DISTANCE:
        return DISTANCE_PENALTY

    else:
        return weightedDistance

# makes any necessary directories so our file writes won't fail
def ensure_dir(filename):
    d = dirname(filename)

    if not exists(d):
        makedirs(d)

