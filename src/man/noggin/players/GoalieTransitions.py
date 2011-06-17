from math import fabs
from ..playbook import PBConstants as PBCon
from .. import NogginConstants as NogCon
import GoalieConstants as goalCon
import ChaseBallTransitions as chaseTran


DEBUG = False

#SAVING TRANSITIONS

#not using right now
#uses ball velocity information which
#im not sure I want to use
def getTimeUntilSave(player):
    ball = player.brain.ball
    time = 0
    if ball.relVelX < 0.0:
        time = ball.relX/ -ball.relVelX
    else:
        time = -1
    return time

def shouldPositionForSave(player):
    ball = player.brain.ball
    #add a counter
    #player.shouldSaveCounter already exists
    #need to test velocity values
    if (fabs(ball.dx) > goalCon.VEL_THRES):
        # if coming towards the goal
        #left front
        if (ball.relX > 0 and ball.relY > 0 and ball.dx > 0 and ball.dy > 0):
            return True
        #right front
        if (ball.relX > 0 and ball.relY < 0 and ball.dx > 0 and ball.dy < 0):
            return True
        #right back
        if (ball.relX < 0 and ball.relY < 0 and ball.dx < 0 and ball.dy < 0):
            return True
        #left back
        if (ball.relX < 0 and ball.relY > 0 and ball.dx < 0 and ball.dy > 0):
            return True

        # this will have an issue with balls that cross close to the goalie
        #need to adjust for this...

    return False


# not used right now
#should move goalie but with dive right now shouldnt need
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
    #Ball is within distance from the goalie and
    #is prepared to save
    ball = player.brain.ball

    if fabs(ball.dx) > 5: #goalCon.VEL_THRES:
        #inside goal box plus save buffer
        if (ball.x < (NogCon.MY_GOALBOX_RIGHT_X + goalCon.SAVE_BUFFER)
             and ball.y < (NogCon.MY_GOALBOX_TOP_Y + goalCon.SAVE_BUFFER)
             and ball.y > (NogCon.MY_GOALBOX_BOTTOM_Y - goalCon.SAVE_BUFFER)):
            return True

    return False

def shouldSaveRight(player):
    ball= player.brain.ball

    if(ball.endY > goalCon.CENTER_SAVE_THRESH):
        player.counterRightSave += 1
        if(player.counterRightSave > 3):
            player.counterRightSave = 0
            player.counterLeftSave = 0
            player.counterCenterSave = 0
            return True
    return False

def shouldSaveLeft(player):
    ball= player.brain.ball

    if(ball.endY < -goalCon.CENTER_SAVE_THRESH):
        player.counterLeftSave += 1
        if( player.counterLeftSave > 3) :
            player.counterLeftSave = 0
            player.counterRightSave = 0
            player.counterCenterSave = 0
            return True

    return False

def shouldSaveCenter(player):
    ball= player.brain.ball

    if(not shouldSaveRight and not shouldSaveLeft):
        player.counterCenterSave += 1
        if (player.counterCenterSave > 3):
            player.counterCenterSave = 0
            player.counterLeftSave = 0
            player.counterRightSave = 0
            return True
    return False

# need to figure out how this works
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

#POSITION TRANSITIONS

# player is inside the box with a small buffer
#localization not good enough for this?
def goalieInBox(player):
    my = player.brain.my

    if  (my.x < (NogCon.MY_GOALBOX_RIGHT_X + 10)
         and my.x > (NogCon.MY_GOALBOX_LEFT_X - 10)
         and my.y < (NogCon.MY_GOALBOX_TOP_Y + 10)
         and my.y > (NogCon.MY_GOALBOX_BOTTOM_Y - 10)):
        return True

    return False


#CHASE TRANSITIONS

def dangerousBall(player):
    ball = player.brain.ball

    # in box and behind me and close to me
    # if inBox(player):
    if (ball.relX < 0):
        #and ball.dist < 30):
        return True

    return False

