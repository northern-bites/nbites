from ..playbook import PBConstants
from .. import NogginConstants as Constants
import GoalieConstants as goalCon
import ChaseBallTransitions as chaseTran

DEBUG = False

def goalieRunChecks(player):

    #if shouldGuardCorner(player) and not player.isChasing:
    #    player.shouldGuardCornerCounter+=1
    #    if player.shouldGuardCornerCounter >=3:
    #        player.isChasing = False
    #        player.shouldGuardCornerCounter = 0
    #        return 'guardCorner'
    #else:
    #    player.shouldGuardCornerCounter = 0


    if not player.isChasing:
       if shouldChaseLoc(player):
           player.shouldChaseCounter+=1
           if DEBUG: print "should chase: ", player.shouldChaseCounter
           if player.shouldChaseCounter >= goalCon.START_CHASE_BUFFER:
               player.shouldChaseCounter = 0
               player.shouldStopChaseCounter = 0
               player.shouldSaveCounter = 0
               player.isChasing = True
               return 'chase'
       else:
           player.shouldChaseCounter = 0

    #if shouldSave(player) and not player.isChasing:
    #    player.shouldSaveCounter+=1
    #    if player.shouldSaveCounter >= 3:
    #        player.isChasing = False
    #        player.shouldChaseCounter = 0
    #        player.shouldStopChaseCounter = 0
    #        return 'goalieSave'
    #else:
    #    player.shouldSaveCounter = 0
        
    if player.isChasing and not chaseTran.shouldntStopChasing(player):
        if shouldStopChaseLoc(player) :
            player.shouldStopChaseCounter+=1
            if DEBUG: print "should stop chase: ", player.shouldChaseCounter
            if player.shouldStopChaseCounter >= goalCon.STOP_CHASE_BUFFER:
                player.shouldStopChaseCounter = 0
                player.shouldSaveCounter = 0
                player.shouldChaseCounter = 0
                player.isChasing = False
                return 'goaliePosition'
        else:
            player.shouldStopChaseCounter = 0

    if player.currentState == 'gamePlaying':
        player.isChasing = False
        return 'goaliePosition'

    return player.currentState

def shouldPositionForSave(player):
    ball = player.brain.ball

    # Test velocity values as to which one would work:
    relVelX = ball.relVelX

    timeUntilSave = getTimeUntilSave(player)
    # No Time, Save now
    if (5  > timeUntilSave > goalCon.BALL_SAVE_LIMIT_TIME and
        ball.framesOn > 3. and relVelX < 0.
        and ball.relX < goalCon.MOVE_TO_SAVE_DIST_THRESH):
        if DEBUG: player.brain.sensors.saveFrame()
        #player.printf("relVelX = %g  timeUntilSave = %g" %
                      #(relVelX, timeUntilSave))
        return True;
    return False;

def strafeDirForSave(player):
    ball = player.brain.ball
    my = player.brain.my
    timeUntilSave = getTimeUntilSave(player)
    anticipatedY = (ball.relY + ball.relVelY * timeUntilSave)
    if anticipatedY < my.y - goalCon.CENTER_SAVE_THRESH:
        return -1
    elif anticipatedY > my.y + goalCon.CENTER_SAVE_THRESH:
        return 1
    else:
        return 0


def shouldSave(player):

    #ball = player.brain.ball
    #if ( ball.y > Constants.MY_GOALBOX_BOTTOM_Y
    #    and ball.y < Constants.MY_GOALBOX_TOP_Y
    #    and ball.x < Constants.MY_GOALBOX_RIGHT_X +
    #        goalCon.SAVE_OFFSET_X and 
    #        ball.velX > .5):
    #    return True
    return False;

def shouldHoldSave(player):
    # same as shouldSave() except for the ball.framesOn check
    # try to come up with better conditions to test
    """    if the ball is still in front of me and coming at me, hold save
    if it's going to arrive anytime soon  """

    ball = player.brain.ball

    timeUntilSave = getTimeUntilSave(player)
    if timeUntilSave < goalCon.BALL_SAVE_LIMIT_TIME*2 and ball.relVelX < 0 and \
            0 < ball.relX < goalCon.MOVE_TO_SAVE_DIST_THRESH:
        if DEBUG: player.brain.sensors.saveFrame()
        return True
    return False

def shouldChaseLoc(player):
    if player.penaltyKicking or player.saving:
        return False

    ball = player.brain.ball
    if (ball.y > Constants.MY_GOALBOX_BOTTOM_Y - goalCon.AGGRESSIVENESS_OFFSET_Y
        and ball.y < Constants.MY_GOALBOX_TOP_Y + goalCon.AGGRESSIVENESS_OFFSET_Y 
        and ball.x < Constants.MY_GOALBOX_RIGHT_X +
        goalCon.AGGRESSIVENESS_OFFSET_X) or \
        (ball.y > Constants.MY_GOALBOX_BOTTOM_Y + 10
        and ball.y < Constants.MY_GOALBOX_TOP_Y - 10
        and ball.x < Constants.MY_GOALBOX_RIGHT_X +
        150):
        return True
    return False

def shouldGuardCorner(player):
    if player.penaltyKicking or player.saving:
        return False
    
    ball = player.brain.ball
    if ((ball.y > 0 and ball.y < Constants.MY_GOALBOX_BOTTOM_Y - 
        goalCon.AGGRESSIVENESS_OFFSET_Y) or 
        (ball.y < Constants.FIELD_HEIGHT and ball.y > Constants.MY_GOALBOX_TOP_Y + 
        goalCon.AGGRESSIVENESS_OFFSET_Y)) and \
        (ball.x < Constants.MY_GOALBOX_RIGHT_X +
        goalCon.AGGRESSIVENESS_OFFSET_X):
        return True
    return False
    
def shouldStopChaseLoc(player):
    ball = player.brain.ball
    my = player.brain.my
    #change to use ball uncertainty
    if ball.uncertX < Constants.GOOD_LOC_XY_UNCERT_THRESH:
        if ball.x > (Constants.CENTER_FIELD_X/2):
            return True

    #makes sure we have seen the ball at least once for this method to return
    #true STOP_CHASE_BUFFER-times
    elif (ball.y < (Constants.MY_GOALBOX_BOTTOM_Y - 
                    goalCon.AGGRESSIVENESS_OFFSET_Y - 
                    goalCon.END_CLEAR_BUFFER) or
        ball.y > (Constants.MY_GOALBOX_TOP_Y +
                  goalCon.AGGRESSIVENESS_OFFSET_Y +
                  goalCon.END_CLEAR_BUFFER) or
        ball.x > (Constants.MY_GOALBOX_RIGHT_X
                  + goalCon.AGGRESSIVENESS_OFFSET_X +
                  goalCon.END_CLEAR_BUFFER )):
        return True

    elif (my.x > Constants.MY_GOALBOX_RIGHT_X + goalCon.END_CLEAR_BUFFER
          or my.y > Constants.MY_GOALBOX_TOP_Y + goalCon.END_CLEAR_BUFFER
          or my.y < Constants.MY_GOALBOX_BOTTOM_Y + goalCon.END_CLEAR_BUFFER) and \
          (ball.dist >= 10):
          return True

    elif (my.x > Constants.MY_GOALBOX_RIGHT_X
          or my.y > Constants.MY_GOALBOX_TOP_Y
          or my.y < Constants.MY_GOALBOX_BOTTOM_Y) and \
          (ball.dist >= 30):
          return True

    return False

def outOfPosition(player):
    if player.penaltyKicking:
        return False

    my = player.brain.my
    if (my.x > Constants.MY_GOALBOX_RIGHT_X and my.uncertX < 90)\
            or (my.x > Constants.MIDFIELD_X * 0.5):
        #print "my.x ", my.x, " my.uncertX ", my.uncertX
        return True
    return False

def dangerousBall(player):
    ball = player.brain.ball
    if ball.x < player.brain.my.x and ball.dist <= 40:
        return True
    #idea: draw ray from me to ball, see if it intersects goal
    return False

def getTimeUntilSave(player):
    ball = player.brain.ball
    time = 0
    if ball.relVelX < 0.0:
        time = ball.relX/ -ball.relVelX
    else:
        time = -1
    return time
