import man.motion as motion
import man.motion.SweetMoves as SweetMoves
import man.motion.RobotGaits as RobotGaits
import man.motion.MotionConstants as MotionConstants
import man.noggin.typeDefs.Location as Location
from ..WebotsConfig import WEBOTS_ACTIVE
import man.noggin.util.PSO as PSO
from man.motion.gaits.GaitLearnBoundaries import \
    gaitMins, gaitMaxs, \
    gaitToArray, arrayToGaitTuple
from os.path import isfile

try:
   import cPickle as pickle
except:
   import pickle

PSO_STATE_FILE = "PSO_pGaitLearner.pickle"
OPTIMIZE_FRAMES = 800
POSITION_UPDATE_FRAMES = 15

RUN_ONCE_STOP = True

def gamePlaying(player):
    print "In the players version of game controller state (overridden)"
    if player.firstFrame():
        player.gainsOn()
        player.brain.tracker.trackBall()

        startPSO(player)

    return player.goLater('stopandchangegait')

if WEBOTS_ACTIVE:
    gameInitial=gamePlaying
    print "Webots is active!!!!"
else:
    print "Webots is in-active!!!!"

def walkstraightstop(player):
    stability = player.brain.stability

    stability.updateStability()
    isFallen = stability.isWBFallen()

    if player.firstFrame():
       # actual motion command clipped by gait parameter
       setWalkVector(player, 30,0,0)
       stability.resetData()

       player.startOptimizeLocation = getCurrentLocation(player)

    # too computationally expensive to use every point
    if player.counter % POSITION_UPDATE_FRAMES == 0:
       stability.updatePosition(player.counter, getCurrentLocation(player))

    if player.counter == OPTIMIZE_FRAMES or isFallen:
       scoreGaitPerformance(player)

       if isFallen:
          print "(GaitLearning):: we've fallen down!"

       return player.goLater('newOptimizeParameters')

    return player.stay()

def scoreGaitPerformance(player):
   stability = player.brain.stability

   frames_stood = player.counter
   endOptimizeLocation = getCurrentLocation(player)
   distance_traveled = endOptimizeLocation.distTo(player.startOptimizeLocation)
   path_linearity = stability.getPathLinearity()
   stability_penalty = stability.getStabilityHeuristic()

   # we maximize on the heuristic
   heuristic = frames_stood + distance_traveled + path_linearity - stability_penalty

   print "total distance traveled with this gait is ", distance_traveled

   player.swarm.getCurrParticle().setHeuristic(heuristic)
   player.swarm.tickCurrParticle()
   savePSO(player)

def newOptimizeParameters(player):
   if RUN_ONCE_STOP:
      print "stopping optimization, restart webots"
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

        newGait = motion.GaitCommand(gaitTuple[0],
                                     gaitTuple[1],
                                     gaitTuple[2],
                                     gaitTuple[3],
                                     gaitTuple[4],
                                     gaitTuple[5],
                                     gaitTuple[6],
                                     gaitTuple[7])

        player.brain.CoA.setRobotDynamicGait(player.brain.motion, newGait)

    if player.counter == 20:
        return player.goLater('walkstraightstop')

    return player.stay()

def standup(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.ZERO_POS)
    return player.stay()

def standuplearn(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.STAND_UP_FRONT)

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

def startPSO(player):
    if isfile(PSO_STATE_FILE):
        loadPSO(player)
    else:
        print "Initializing new PSO"
        player.swarm = PSO.Swarm(25, 44, gaitToArray(gaitMins), gaitToArray(gaitMaxs))

def savePSO(player):
    print "Saving PSO state to: ", PSO_STATE_FILE
    f = open(PSO_STATE_FILE, 'w')
    pickle.dump(player.swarm, f)
    f.close()

def loadPSO(player):
    print "Loading PSO state from: ", PSO_STATE_FILE
    f = open(PSO_STATE_FILE, 'r')
    player.swarm = pickle.load(f)
    f.close()

def setWalkVector(player, x, y, theta):
    """
    Use this guy because all the wrappings through Nav tend
    to reset our gait parameters
    """
    walk = motion.WalkCommand(x=x,y=y,theta=theta)
    player.brain.motion.setNextWalkCommand(walk)

def getCurrentLocation(player):
   return Location.Location(player.brain.my.x,
                            player.brain.my.y,
                            player.brain.my.h)


