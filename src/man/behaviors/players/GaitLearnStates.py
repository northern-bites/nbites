#
# This is our walk-engine parameter optimizer. It uses a particle swarm (util/PSO.py)
# to search a predefined gait space (motion/gaits/GaitLearnBoundaries.py) for an
# optimal set of parameters.
#
# Things that need to happen before team-wide gait optimization can start:
# (list current as of 5/7/11)
#
# @todo (Motion) change engine to walk in place before beginning to move
#
# @author Nathan Merritt
# @date May 2011

import man.motion as motion
import man.motion.SweetMoves as SweetMoves
import man.motion.RobotGaits as RobotGaits
import man.motion.MotionConstants as MotionConstants
from objects import Location
import man.noggin.navigator.NavHelper as helper
import man.noggin.navigator.WalkHelper as walkhelper

import man.noggin.util.PSO as PSO
from man.noggin.util.GaitOptimizeHelpers import (gaitToArray,
                                                 arrayToGaitTuple,
                                                 writeGaitToFile)
from man.motion.gaits.GaitLearnBoundaries import (gaitMins,
                                                  gaitMaxs)

from os.path import isfile, dirname, exists
from os import makedirs
from math import fabs
import random

try:
   import cPickle as pickle
except:
   import pickle

PICKLE_FILE_PREFIX = '/home/nao/gaits/'
NUM_PARTICLES = 20

PSO_STATE_FILE = PICKLE_FILE_PREFIX + "PSO_pGaitLearner.pickle"
BEST_GAIT_FILE = PICKLE_FILE_PREFIX + "PSO_gait"

SWARM_ITERATION_LIMIT = 25 # wikipedia says this should be enough to converge?
RESART_PSO_ON_COMPLETION = False # after 25 iterations, restart or exit?

RUN_ONCE_STOP = True
REPORT_AFTER_RUN = True # saves the best gait after each run (may save duplicates)

# Set of walk vectors that the robot must be able to complete before we start
# giving it random omni-walks. These go from easier to harder, in theory
WALK_VECTOR_DURATION = 150
NUMBER_RANDOM_WALKS = 20
REQUIRED_WALKS = ((0.5, 0, 0),
                  (0, .5, 0),
                  (-.5, 0, 0),
                  (0, -.5, 0),
                  (0, 0, .5), # Forward/L/Back/R/Rotate (slow)
                  (1, 0, 0),
                  (-1, 0, 0),
                  (0, 0, 1), # full forward/back/rotate
                  (0, -1, 1),
                  (.75, .75, 0),
                  (0, -.75, .75),
                  (-.75, 0, .75),
                  (0, .75, -.75),
                  (.75, -.75, .75), # some omni variations
                  )

def gamePlaying(player):
   player.printf("In the pGaitLearner's version of game controller state (overridden)")
   if player.firstFrame():
      player.gainsOn()
      player.brain.tracker.trackBall()

      # we don't want to auto-stand after falling
      player.brain.fallController.executeStandup = False
      player.brain.fallController.enabled = False

      # robots that talk are annoying
      player.brain.speech.disable()

      startPSO(player)

   return player.goLater('stopChangeGait')

def gamePenalized(player):
    if player.firstFrame():
        player.stopWalking()
        player.penalizeHeads()

    return player.stay()

def walkTest(player):
   ''' Runs the robot through a list of walk vectors,
   WALK_VECTOR_DURATION frames each, and then NUMBER_RANDOM_WALKS of
   random walk vectors. Score the current gait after the robot falls,
   or finishes all the walk vectors
   '''
   stability = player.brain.stability

   if player.firstFrame():
      stability.resetData()
      player.straightWalkCounter = 0
      player.onTest = 0
      player.onRandomWalk = 0

   stability.updateStability() # save sensor data, to calculate variance

   # check to see if we've fallen over
   if player.brain.roboguardian.isRobotFallen():
      player.gainsOff()
      player.walkCounter = player.counter

      scoreGaitPerformance(player)
      player.printf("(GaitLearning):: We've fallen down!\n")
      return player.goLater('newOptimizeParameters')

   # change the walk vector, either from the unit test list or generate one
   if player.counter % WALK_VECTOR_DURATION == 0 or \
          player.firstFrame():

      if player.onTest < len(REQUIRED_WALKS):
         thisVector = REQUIRED_WALKS[player.onTest]
         player.printf("Setting new walk vector (%s/%s): %s"
                       % (player.onTest, len(REQUIRED_WALKS), thisVector))

         setWalkVectorCustomGait(player, thisVector[0], thisVector[1], thisVector[2])
         player.onTest += 1

      elif player.onRandomWalk < NUMBER_RANDOM_WALKS:
         r_x = random.uniform(-1, 1) # anything goes, try to kill ourselves
         r_y = random.uniform(-1, 1)
         r_theta = random.uniform(-1, 1)

         setWalkVectorCustomGait(player, r_x, r_y, r_theta)
         player.onRandomWalk += 1

         player.printf("Setting random walk vector (%s/%s): %s, %s, %s"
                       % (player.onRandomWalk, NUMBER_RANDOM_WALKS, r_x, r_y, r_theta))

      else:
         player.printf("Finished this gait run, scoring and changing gait")
         player.walkCounter = player.counter
         scoreGaitPerformance(player)
         return player.goLater('newOptimizeParameters')

   return player.stay()

def scoreGaitPerformance(player):
   '''
   Scores the last gait that the robot tried, based on how long
   the robot stayed standing (good) and the amount of variance in
   accelerometer data (lots of variance is generally bad)

   This method also passes the heuristic to the PSO and ticks the
   particle. The PSO is saved (pickled) every time a particle is ticked
   '''
   stability = player.brain.stability

   stability_penalty = stability.getStabilityHeuristic()

   frames_stood = player.walkCounter

   # the PSO maximizes on the heuristic
   heuristic = frames_stood - stability_penalty

   player.printf("robot stood for %s frames this run" % frames_stood)
   player.printf("stability penalty is %s" % stability_penalty)
   player.printf("heuristic for this run is %s" % heuristic)

   player.swarm.getCurrentParticle().setHeuristic(heuristic)
   player.swarm.tickCurrentParticle()
   savePSO(player)

def newOptimizeParameters(player):
   """
   Controls what we do after every optimization run,
   pointer state to more interesting places
   """
   if REPORT_AFTER_RUN:
      reportBestGait(player)

   if player.swarm.getIterations() > SWARM_ITERATION_LIMIT:
       print "Swarm is done optimizing!"
       reportBestGait(player)
       return player.goLater('gamePenalized')

   elif RUN_ONCE_STOP:
       return player.goLater('gamePenalized')

   # pause, then start a new optimization run (once the robot is ready)
   elif not player.brain.roboguardian.isRobotFallen() \
          and player.brain.roboguardian.isFeetOnGround():
      return player.goLater('stopChangeGait')

   else:
      return player.stay()

def stopChangeGait(player):
    '''Set new gait and start walking again'''

    if player.firstFrame():
       setWalkVectorCustomGait(player, 0,0,0)

       gaitTuple = arrayToGaitTuple(player.swarm.getCurrentParticle().getPosition())

       setGait(player, gaitTuple)

    if player.counter == 50:
       return player.goLater('walkTest')

    return player.stay()

def reportBestGait(player):
   (bestGaitArray, gaitScore) = player.swarm.getBestSolution()
   bestGaitTuple = arrayToGaitTuple(bestGaitArray)

   player.printf("best found gait's heuristic score was: %s" % gaitScore)

   try:
      gaitScore = int(gaitScore)
      output = BEST_GAIT_FILE + str(gaitScore) + ".py"

      player.printf("best gait saved to file: %s" % output)
      f = open(output, 'w')
      writeGaitToFile(f, bestGaitTuple, gaitScore)
      f.close()

   except:
      player.printf("error writing out gait")

def restartOptimization(player):
   '''
   Restarts the PSO
   '''
   if player.firstFrame():
      newPSO(player)
      savePSO(player)

      return player.goLater('newOptimizeParameters')

def startPSO(player):
    if isfile(PSO_STATE_FILE):
        loadPSO(player)
    else:
        newPSO(player)

def savePSO(player):
    player.printf("Saving PSO state to: %s" % PSO_STATE_FILE)
    ensure_dir(PSO_STATE_FILE)
    f = open(PSO_STATE_FILE, 'w')
    pickle.dump(player.swarm, f)
    f.close()

def newPSO(player):
    player.printf("Initializing new PSO")
    player.swarm = PSO.Swarm(NUM_PARTICLES,
                             44, gaitToArray(gaitMins), gaitToArray(gaitMaxs))

def loadPSO(player):
    player.printf("Loading PSO state from: %s" % PSO_STATE_FILE)
    ensure_dir(PSO_STATE_FILE)
    f = open(PSO_STATE_FILE, 'r')
    player.swarm = pickle.load(f)
    f.close()

def setWalkVectorCustomGait(player, x, y, theta):
    """
    Use this method because all the wrappings through Nav will set the
    robot's gait to one of our predefined values
    """
    x_cms, y_cms, theta_degs = helper.convertWalkVector(player.brain, x, y, theta)

    walk = motion.WalkCommand(x=x_cms,y=y_cms,theta=theta_degs)
    player.brain.motion.setNextWalkCommand(walk)


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

# makes any necessary directories so our file writes won't fail
def ensure_dir(filename):
    d = dirname(filename)

    if not exists(d):
        makedirs(d)
