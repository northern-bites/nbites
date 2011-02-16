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
    # add counter as a buffer on making decisions
    # add decision buffer so that there is a gap where it wont be
    # just switching back and forth
    # make separate states for the different goalie decisions?
    ball = player.brain.ball
    my = player.brain.my
    dx = 0

    if ball.framesOn > 0 and dx != 0:
        currentX = ball.relX
        currentY = ball.relY
        previousX = ball.lastRelX
        previousY = ball.lastRelY
      #  frameNum = player.counter
        
        dx = previousX - currentX
        dy = previousY - currentY

        changesX = currentX/dx
       # framesTillLevel = 1 * int(changesX)
        endY = currentY - (changesX * dy)
    
#    if player.counter != (frameNum + framesTillLevel):
#        return player.stay()
#     else:
#         if( endY > goalCon.CENTER_SAVE_THRESH):
#             player.countRightSave += 1
#             if(player.countRightSave > 3):
#                 print "Should be saving right "
#                 print endY
#                 frameNum = 0
#                 return player.goNow('goalieSaveRight')

#         elif( endY  <  -goalCon.CENTER_SAVE_THRESH):
#             player.countLeftSave += 1
#             if( player.counterLeftSave > 3) :
#                 print "Should be saving left "
#                 print endY
#                 frameNum = 0
#                 return player.goNow('goalieSaveLeft')
#         else:
#             player.countCenterSave += 1
#             if (player.countCenterSave > 3):
#                 print "Should be saving center "
#                 print endY
#                 frameNum = 0
#                 return player.goNow('goalieSaveCenter')
                
    return player.stay()

def goalieSaveRight(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.GOALIE_TEST_DIVE_RIGHT)
    if player.counter > 50:
        player.executeMove(SweetMoves.INITIAL_POS)
        return player.goNow('goalieDecisionTest')
    return player.stay()

def goalieSaveLeft(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.GOALIE_TEST_DIVE_LEFT)
    if player.counter > 50:
        player.executeMove(SweetMoves.INITIAL_POS)
        return player.goNow('goalieDecisionTest')
    return player.stay()


def goalieSaveCenter(player):
    if player.firstFrame():
        player.executeMove(SweetMoves.GOALIE_TEST_CENTER_SAVE)
    if player.counter > 50:
        player.executeMove(SweetMoves.INITIAL_POS)
        return player.goNow('goalieDecisionTest')
    return player.stay()

