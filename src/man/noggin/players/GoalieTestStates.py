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
    dx = 0
    frameNum = -1
    framesTillLevel = -1

    if ball.framesOn > 0 :
        currentX = ball.relX
        currentY = ball.relY
        previousX = ball.lastRelX
        previousY = ball.lastRelY
        frameNum = player.counter
        
        dx = previousX - currentX
        dy = previousY - currentY
        
    if (dx != 0 and ball.framesOn > 0):
        changesX = currentX/dx
        framesTillLevel = 1 * int(changesX)
        endY = currentY - (changesX * dy)
    
    if player.counter != (frameNum + framesTillLevel):
        return player.stay()
    else:
        if( endY > goalCon.CENTER_SAVE_THRESH):
            print "Should be saving right "
            print endY
            player.executeMove(SweetMoves.GOALIE_TEST_DIVE_RIGHT)
            frameNum = 0
        elif( endY  <  -goalCon.CENTER_SAVE_THRESH):
            print "Should be saving left "
            print endY
            player.executeMove(SweetMoves.GOALIE_TEST_DIVE_LEFT)
            frameNum = 0
        else:
            print "Should be saving center "
            print endY
            player.executeMove(SweetMoves.GOALIE_TEST_CENTER_SAVE) 
            frameNum = 0
                
    return player.stay()
