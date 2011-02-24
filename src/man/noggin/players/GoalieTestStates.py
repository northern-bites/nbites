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
    return player.stay()

def gamePlaying(player):
    return player.goNow('goalieDecisionTest')

def gamePenalized(player):
    return player.stay()

def goalieDecisionTest(player):

    return player.stay()
