from math import fabs
from ..playbook import PBConstants as PBCon
from .. import NogginConstants as NogCon
import GoalieConstants as goalCon
import ChaseBallTransitions as chaseTran
#am I suposed to do this?
from ..playbook import GoTeam as GoTeam

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

    if (shouldPrepareToSave):
        #inside goal box plus save buffer
        if  (ball.x < (NogCon.MY_GOALBOX_RIGHT_X + goalCon.SAVE_BUFFER)
             and ball.y < (NogCon.MY_GOALBOX_TOP_Y + goalCon.SAVE_BUFFER)
             and ball.y > (NogCon.MY_GOALBOX_BOTTOM_Y - goalCon.SAVE_BUFFER)):
            return True

    return False

def shouldSaveRight(player):
    ball= player.brain.ball

    if ball.dx < goalCon.VEL_THRES and ball.dx > -goalCon.VEL_THRES:
        return False

    elif(ball.endY > goalCon.CENTER_SAVE_THRESH):
        player.countRightSave += 1
        if(player.countRightSave > 3):
            player.counterRightSave = 0
            player.counterLeftSave = 0
            player.counterCenterSave = 0
            return True
    return False

def shouldSaveLeft(player):
    ball= player.brain.ball

    if ball.dx < goalCon.VEL_THRES and ball.dx > -goalCon.VEL_THRES:
        return False

    if(ball.endY < -goalCon.CENTER_SAVE_THRESH):
        player.countLeftSave += 1
        if( player.counterLeftSave > 3) :
            player.counterLeftSave = 0
            player.counterRightSave = 0
            player.counterCenterSave = 0
            return True

    return False

def shouldSaveCenter(player):
    ball= player.brain.ball

    if(ball.dx < goalCon.VEL_THRES and ball.dx > -goalCon.VEL_THRES):
        if ball.relX < 10:
            player.countCenterSave += 1

    elif( not shouldDiveRight and not shouldDiveLeft):
        player.countCenterSave += 1

    if (player.countCenterSave > 3):
            player.counterCenterSave = 0
            player.counterLeftSave = 0
            player.counterRightSave = 0
            return True
    return False

#need to figure out how this works
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

def outOfPosition(player):
    if player.penaltyKicking:
        return False

    #I need to figure out what this does
    my = player.brain.my
    if (my.x > NogCon.MY_GOALBOX_RIGHT_X and my.uncertX < 90)\
            or (my.x > NogCon.MIDFIELD_X * 0.5):
        #print "my.x ", my.x, " my.uncertX ", my.uncertX
        return True
    return False

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

# add a counter to this
def shouldPositionCenter(player):
    ball = player.brain.ball

    if (ball.y > NogCon.LANDMARK_MY_GOAL_RIGHT_POST_Y + goalCon.BOX_BUFFER
        and ball.y < NogCon.LANDMARK_MY_GOAL_LEFT_POST_Y - goalCon.BOX_BUFFER):
        player.shouldPositionCenter += 1
        if player.shouldPositionCenter > 3:
            player.shouldPositionCenterCounter = 0
            player.shouldPositionLeftCounter = 0
            player.shouldPositionRightCounter = 0
            return True

    return False

def shouldPositionRight(player):
    ball = player.brain.ball

    if (ball.y < NogCon.LANDMARK_MY_GOAL_RIGHT_POST_Y - goalCon.BOX_BUFFER
        and ball.x < NogCon.MY_GOALBOX_RIGHT_X + goalCon.BOX_BUFFER):
        player.shouldPositionRightCounter += 1
        if player.shouldPositionRightCounter > 3:
            player.shouldPositionRightCounter = 0
            player.shouldPositionLeftCounter = 0
            player.shouldPositionCenterCounter = 0
            return True

    return False


def shouldPositionLeft(player):
    ball = player.brain.ball

    if (ball.y > NogCon.LANDMARK_MY_GOAL_LEFT_POST_Y + goalCon.BOX_BUFFER
        and ball.x < NogCon.MY_GOALBOX_RIGHT_X + goalCon.BOX_BUFFER):
        player.shouldPositionLeftCounter += 1
        if player.shouldPositionLeftCounter > 3:
            player.shouldPositionRightCounter = 0
            player.shouldPositionLeftCounter = 0
            player.shouldPositionCenterCounter = 0
            return True

    return False

def dangerousBall(player):
    ball = player.brain.ball

    # in box and behind me and close to me
    # if inBox(player):
    if (ball.relX < 0 and ball.relX < 30
        and ball.relY < 30
        and ball.relY > -30):
        return True

    return False

#Decisions for when to chase.
#Will chase when:
    # Inside the chasing box

def shouldChase(player):
    ball = player.brain.ball

    #how does the penalty kicker work?
    if player.penaltyKicking:
        return False

    # checks if others are chasing
    # what checks who is closest person to ball
    #can i use this?
    # elif not goTeam.goalieShouldChase(player):
        # return False

    #if not chaseTran.shouldChaseBall(player):
        #return False

    #if (ball.framesOff > 45):
        #print "no ball"
        #player.shouldChaseCounter = 0
        #player.shouldStopChaseCounter = 0
        #return False

    # close enough to chase
    elif (ball.x < goalCon.CHASE_RIGHT_X_LIMIT
          and ball.x > goalCon.CHASE_LEFT_X_LIMIT
          and ball.y > goalCon.CHASE_LOWER_Y_LIMIT
          and ball.y < goalCon.CHASE_UPPER_Y_LIMIT):
        player.shouldChaseCounter += 1

    if player.shouldChaseCounter > 3:
        player.shouldChaseCounter = 0
        player.shouldStopChaseCounter = 0
        return True

    return False

#Should stop chasing if
    #Ball is outside of the chase range
def shouldStopChase(player):
    ball= player.brain.ball

    #if(ball.framesOff > 45):
       # print "1"
       # player.shouldStopChaseCounter = 4

    if (ball.x > goalCon.CHASE_RIGHT_X_LIMIT
          or ball.x < goalCon.CHASE_LEFT_X_LIMIT
          or ball.y < goalCon.CHASE_LOWER_Y_LIMIT
          or ball.y > goalCon.CHASE_UPPER_Y_LIMIT):
        player.shouldStopChaseCounter += 1

    #i dont think this works right now?
    #elif(chaseTran.shouldntStopChasing(player)):
        #print "3"
        #return False

    if player.shouldStopChaseCounter > 3:
        player.shouldStopChaseCounter = 0
        player.shouldChaseCounter = 0
        return True

    return False
