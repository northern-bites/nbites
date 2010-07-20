"""
Similar to WalkUnitTest, if a Gait makes it through all the Walk tests
without falling then it is considered stable and passes

"""

import man.motion as motion
from ..WebotsConfig import WEBOTS_ACTIVE
from .GaitLearnStates import setWalkVector, setGait, revertWebots
import man.noggin.util.GaitTest as GaitTest

from os.path import isfile

try:
   import cPickle as pickle
except:
   import pickle

#Types (see WalkTestStates.py)
WALK = 0

DURATION = 200

GAITS = ('PSO_endGait.pickle.1395',
         'PSO_endGait.pickle.1445',
         'PSO_endGait.pickle.1446',
         )

UNIT_TEST1 = ((WALK, (15,0,0), DURATION),
              (WALK, (0,10,0), DURATION),
              (WALK, (6,6,0), DURATION),
              (WALK, (6,-6,0), DURATION),
              (WALK, (8,0,20), DURATION),
              (WALK, (8,0,-20), DURATION),
              )

TEST_DATA_FILE = 'gaitUnitTest.pickle'

def gamePlaying(player):
    if player.firstFrame():
        player.gainsOn()
        player.brain.tracker.stopHeadMoves()

        initTestData(player)
        getDataFromClass(player)

    return player.goLater('gaitTest')

if WEBOTS_ACTIVE:
    gameInitial=gamePlaying
    print "Webots is active!!!!"
else:
    print "Webots is in-active!!!!"

def gaitTest(player):
    if player.firstFrame():
        if player.gaitCounter >= len(player.gaitTest):
            return player.goLater('printResultsSitdown')

        try:
            gaitFile = player.gaitTest[player.gaitCounter]
            print "gaitFile: ", gaitFile
            loadAndSetGait(player, gaitFile)
        except:
            print "error setting gait: ", player.gaitTest[player.gaitCounter]
            player.goLater('sitdown')

        player.gaitCounter += 1
        player.testCounter = 0

    if player.counter == 20:
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
            return player.goLater('gaitTest')

        setTestDirection(player)

    if isFallen:
        player.gaitTestResults.append(False)
        return player.goLater('gaitTest')

    if player.counter == player.testFrames:
        return player.goNow('switchDirections')

    return player.stay()

def switchDirections(player):
    return player.goNow('walkTest')

def printResultsSitDown(player):
   if player.firstFrame():
      i = 0
      for gait in player.gaitTest:
         print "gait file ", gait, \
             " stability ", player.gaitTestResults[i]
         i += 1

   #return player.goNow('sitdown')
   return player.stay()

def sitdown(player):
    if player.firstFrame():
       if WEBOTS_ACTIVE:
          return player.goNow('saveAndRevert')

       else:
          setWalkVector(player, 0,0,0)
          player.executeMove(SweetMoves.SIT_POS)
          player.brain.tracker.stopHeadMoves()

    return player.stay()

def saveAndRevert(player):
   syncDataToClass(player)
   saveTestData(player)
   player.goNow('revertWebots')

def loadAndSetGait(player, gaitPickleFile):
    if isfile(gaitPickleFile):
        try:
            f = open(gaitPickleFile, 'r')
            newGait = pickle.load(f)
            f.close()

            setGait(player, newGait)
        except:
            print "error loading pickle file: ", gaitPickleFile
    else:
        print "bad file name: ", gaitPickleFile

def initTestData(player):
   if isfile(TEST_DATA_FILE):
      loadTestData(player)
   else:
      newTestData(player)

def loadTestData(player):
   try:
      f = open(TEST_DATA_FILE, 'r')
      player.testData = pickle.load(f)
      f.close()
   except:
      print "could not load test data file ", TEST_DATA_FILE

def newTestData(player):
   print "Initializing new Gait Unit Test Data"
   player.testData = GaitTestData(GAITS,
                                  UNIT_TEST1)

def saveTestData(player):
   try:
      f = open(TEST_DATA_FILE, 'w')
      pickle.dump(player.testData, f)
      f.close()
   except:
      print "could not save test data to file ", TEST_DATA_FILE

# Ugly, ugly, ugly.
def syncDataToClass(player):
   player.testData = td

   td.gaitTest = player.gaitTest
   td.unitTest = player.unitTest
   td.gaitTestResults = player.gaitTestResults
   td.testCounter = player.testCounter
   td.gaitCounter = player.gaitCounter

def getDataFromClass(player):
   player.testData = td

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
