"""
Similar to WalkUnitTest, if a Gait makes it through all the Walk tests
without falling then it is considered stable and passes

"""

import man.motion as motion
from ..WebotsConfig import WEBOTS_ACTIVE
from .GaitLearnStates import setWalkVector, setGait

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

def gamePlaying(player):
    if player.firstFrame():
        player.gainsOn()
        player.brain.tracker.stopHeadMoves()

        player.unitTest = UNIT_TEST1
        player.gaitTest = GAITS

        player.gaitTestResults = []

        player.testCounter = 0
        player.gaitCounter = 0

        print "correct gamePlaying method!!"

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

    return player.goNow('sitdown')

def sitdown(player):
    if player.firstFrame():
        setWalkVector(player, 0,0,0)
        player.executeMove(SweetMoves.SIT_POS)
        player.brain.tracker.stopHeadMoves()

    return player.stay()

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
