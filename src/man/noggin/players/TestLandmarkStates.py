# Test class for landmark tracking localization system

import man.motion.SweetMoves as SweetMoves
from .. import NogginConstants

def gameInitial(player):
    player.gainsOn()
    return player.stay()

def gameReady(player):
    if player.firstFrame():
        player.gainsOn
        player.standup()
    return player.stay()

def gameSet(player):
    print "counter:",player.counter
    print "my loc:",player.brain.my.x,player.brain.my.y,player.brain.my.h

    if player.brain.my.locScoreXY == NogginConstants.OK_LOC:
        print "locScoreXY is OK"
    if player.brain.my.locScoreXY == NogginConstants.GOOD_LOC:
        print "locScoreXY is GOOD"
    if player.brain.my.locScoreTheta == NogginConstants.OK_LOC:
        print "locScoreTheta is OK"
    if player.brain.my.locScoreTheta == NogginConstants.GOOD_LOC:
        print "locScoreTheta is GOOD"

    return player.stay()

def gamePlaying(player):
    if player.firstFrame():
        pass#player.brain.tracker.newTrackBall()
    return player.stay()

def gamePenalized(player):
    if player.firstFrame():
        player.brain.tracker.stopHeadMoves()
    return player.stay()
