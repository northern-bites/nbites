#
# For testing new goalie logic for saving.
#

import math

import man.motion as motion
import man.motion.SweetMoves as SweetMoves
import man.motion.MotionConstants as MotionConstants
import GoalieConstants as goalCon

def gameInitial(player):
    if player.firstFrame():
        player.gainsOn()
    return player.stay()

def gameReady(player):
    if player.firstFrame():
        player.brain.tracker.trackBall()
        player.standup()

    return player.stay()

def gameSet(player):
    return player.goNow('goalieDecisionTest')

def gamePlaying(player):
    return player.goNow('goalieDecisionTest')

def gamePenalized(player):
    return player.goNow('goalieDecisionTest')

def goalieDecisionTest(player):
    ball = player.brain.ball
    my = player.brain.my

    if player.counter % 100 != 0:
        return player.stay()

    if( ball.relY > goalCon.CENTER_SAVE_THRESH):
        print "Should be saving right "
        player.executeMove(SweetMoves.GOALIE_TEST_DIVE_RIGHT)
    elif( ball.relY <  -goalCon.CENTER_SAVE_THRESH):
        print "Should be saving left "
        player.executeMove(SweetMoves.GOALIE_TEST_DIVE_LEFT)
    else:
        print "Should be saving center "
        player.executeMove(SweetMoves.GOALIE_TEST_CENTER_SAVE) 

    return player.stay()
