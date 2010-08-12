"""
Similar to WalkUnitTest, if a Gait makes it through all the Walk tests
without falling then it is considered stable and passes

"""

import man.motion as motion
import man.motion.SweetMoves as SweetMoves
from ..WebotsConfig import WEBOTS_ACTIVE
from .GaitLearnStates import setWalkVector, setGait, revertWebots
import man.noggin.util.GaitTestData as data

from os.path import isfile

try:
   import cPickle as pickle
except:
   import pickle

#Types (see WalkTestStates.py)
WALK = 0

DURATION = 250

if WEBOTS_ACTIVE:
    PICKLE_FILE_PREFIX = ''
    START_DELAY = 100
else:
    PICKLE_FILE_PREFIX = '/home/nao/gaits/'
    START_DELAY = 30

GAITS = ('PSO_endGait.pickle.2198',
         'PSO_endGait.pickle.2230',
         'PSO_endGait.pickle.2322',
         )

WORK_ON_NAO = ('PSO_endGait.pickle.1271',
               )

UNIT_TEST1 = ((WALK, (.4, 0, 0), 2*DURATION),
              (WALK, (0, .4, 0), DURATION),
              (WALK, (.2, .2, 0), DURATION),
              (WALK, (.4, -.4, 0), DURATION),
              (WALK, (.5, 0, .5), DURATION),
              (WALK, (0, 0, 0), DURATION/4),
              )

# higher speeds
UNIT_TEST_FAST = ((WALK, (.6, 0, 0), DURATION),
                  (WALK, (0, 0, 0), DURATION/4),
                  (WALK, (.8, 0, 0), DURATION),
                  (WALK, (0, 0, 0), DURATION/4),
                  (WALK, (1, 0, 0), DURATION),
                  )

TEST_DATA_FILE = PICKLE_FILE_PREFIX + 'gaitUnitTest.pickle'

def gamePlaying(player):
    if player.firstFrame():
        player.gainsOn()
        player.brain.tracker.stopHeadMoves()

        initTestData(player)
        getDataFromClass(player)

    if player.counter == 50:
       return player.goLater('gaitTest')

    return player.stay()

def gameReady(player):
    return player.goLater('gamePlaying')

def saveAndEnd(player):
    syncDataToClass(player)
    saveTestData(player)

    if WEBOTS_ACTIVE:
       revertWebots(player)
    else:
        return player.goLater('sitdown')

if WEBOTS_ACTIVE:
    gameInitial=gamePlaying
    print "Webots is active!!!!"
else:
    gamePenalized = saveAndEnd
    print "Webots is in-active!!!!"

def gaitTest(player):
    if player.counter == START_DELAY:
        if player.gaitCounter >= len(player.gaitTest):
            return player.goLater('printResultsStop')

        try:
            gaitFile = PICKLE_FILE_PREFIX + \
                player.gaitTest[player.gaitCounter]
            print "gaitFile: ", gaitFile
            loadAndSetGait(player, gaitFile)
        except:
            print "error setting gait: ", player.gaitTest[player.gaitCounter]
            player.goLater('sitdown')

        player.gaitCounter += 1
        player.testCounter = 0

    if player.counter == START_DELAY*2:
        return player.goLater('walkTest')

    return player.stay()

def walkTest(player):
    """
    This method processes the list of commands until there are
    none left
    """
    isFallen = player.brain.stability.isWBFallen()

    if player.firstFrame():
        if player.testCounter >= len(player.unitTest):
            setWalkVector(player, 0, 0, 0)

            player.gaitTestResults.append(True)
            return player.goLater('saveAndEnd')

        setTestDirection(player)

    if isFallen:
        print "(GaitUnitTest):: we've fallen down!"
        player.gaitTestResults.append(False)

        return player.goLater('saveAndEnd')

    if player.counter == player.testFrames:
        return player.goNow('switchDirections')

    return player.stay()

def switchDirections(player):
    return player.goNow('walkTest')

def printResultsStop(player):
   if player.firstFrame() and WEBOTS_ACTIVE:
      i = 0
      for gait in player.gaitTest:
         print "gait file ", gait, \
             " basically stable -> ", player.gaitTestResults[i]
         i += 1

   return player.goNow('saveAndStop')

   return player.stay()

def sitdown(player):
    if player.firstFrame():
       setWalkVector(player, 0,0,0)
       player.executeMove(SweetMoves.SIT_POS)
       player.brain.tracker.stopHeadMoves()

    return player.stay()

def loadAndSetGait(player, gaitPickleFile):
    f = open(gaitPickleFile, 'r')
    newGait = pickle.load(f)
    f.close()

    setGait(player, newGait)

def initTestData(player):
   if isfile(TEST_DATA_FILE):
      loadTestData(player)
   else:
      newTestData(player)
      saveTestData(player)

def loadTestData(player):
   try:
      f = open(TEST_DATA_FILE, 'r')
      player.testData = pickle.load(f)
      f.close()
   except:
      print "could not load test data file ", TEST_DATA_FILE

def newTestData(player):
   print "Initializing new Gait Unit Test Data"
   player.testData = data.GaitTestData(GAITS,
                                       UNIT_TEST1)

def saveTestData(player):
   print "Saving test data file: ", TEST_DATA_FILE
   try:
      f = open(TEST_DATA_FILE, 'w')
      pickle.dump(player.testData, f)
      f.close()
   except:
      print "could not save test data to file ", TEST_DATA_FILE

# Ugly, ugly, ugly.
def syncDataToClass(player):
   td = player.testData

   td.gaitTest = player.gaitTest
   td.unitTest = player.unitTest
   td.gaitTestResults = player.gaitTestResults
   td.testCounter = player.testCounter
   td.gaitCounter = player.gaitCounter

def getDataFromClass(player):
   td = player.testData

   player.gaitTest = td.gaitTest
   player.unitTest = td.unitTest
   player.gaitTestResults = td.gaitTestResults
   player.testCounter = td.testCounter
   player.gaitCounter = td.gaitCounter

def setTestDirection(player):
    currentCommand  = player.unitTest[player.testCounter]
    player.testCounter += 1
    player.testFrames = currentCommand[2]
    currentVector = currentCommand[1]

    if currentCommand[0] == WALK:
        setWalkVector(player,
                      currentVector[0],
                      currentVector[1],
                      currentVector[2],)
    else:
        player.printf("WARNING! Unrecognized command"
                      " type in WalkUnitTest")
