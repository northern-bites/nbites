from math import fabs
from . import PBConstants as PBCon
from .. import NogginConstants as NogCon
from ..players import GoalieConstants as goalCon

#********
# GOALIE*
#********

def shouldSave(team):
    #Ball is within distance from the goalie and
    #is prepared to save
    ball = team.brain.ball

    if fabs(ball.dx) > 5: #goalCon.VEL_THRES:
        #inside goal box plus save buffer
        if (ball.x < (NogCon.MY_GOALBOX_RIGHT_X + goalCon.SAVE_BUFFER)
             and ball.y < (NogCon.MY_GOALBOX_TOP_Y + goalCon.SAVE_BUFFER)
             and ball.y > (NogCon.MY_GOALBOX_BOTTOM_Y - goalCon.SAVE_BUFFER)):
            return True

    return False

def shouldPosition(team):
    #if team.penaltyKicking:
        #return False

    my = team.brain.my
    # checks if in front of box or a quarter of the way up the field
    #check if this really should be MY_GOALBOX_RIGHT_X
    if (my.x > NogCon.MY_GOALBOX_RIGHT_X and my.uncertX < 90)\
            or (my.x > NogCon.MIDFIELD_X * 0.5):
        #print "my.x ", my.x, " my.uncertX ", my.uncertX
        return True
    return False

def shouldChase(team):
    ball = team.brain.ball

    ## FIND OUT IF WE HAVE A SUBROLE FOR THIS!!!!!
    #if team.penaltyKicking:
        #return False

    #if not chaseTran.shouldChaseBall(team):
        #return False

    #if (ball.framesOff > 45):
        #print "no ball"
        #team.shouldChaseCounter = 0
        #team.shouldStopChaseCounter = 0
        #return False


    # checks if the ball is really far away
    #  make sure we dont chase if really far
    #if ball.relX > 300 :
        #return false

    # close enough to chase
    if (ball.x < goalCon.CHASE_RIGHT_X_LIMIT
          and ball.x > goalCon.CHASE_LEFT_X_LIMIT
          and ball.y > goalCon.CHASE_LOWER_Y_LIMIT
          and ball.y < goalCon.CHASE_UPPER_Y_LIMIT):
        team.shouldChaseCounter += 1

    if team.shouldChaseCounter > 3:
        team.shouldChaseCounter = 0
        team.shouldStopChaseCounter = 0
        return True

    return False

def shouldStopChase(team):
    ball= team.brain.ball

    #if(ball.framesOff > 45):
       # print "1"
       # team.shouldStopChaseCounter = 4

    if (ball.x > goalCon.CHASE_RIGHT_X_LIMIT
          or ball.x < goalCon.CHASE_LEFT_X_LIMIT
          or ball.y < goalCon.CHASE_LOWER_Y_LIMIT
          or ball.y > goalCon.CHASE_UPPER_Y_LIMIT):
        team.shouldStopChaseCounter += 1

    #i dont think this works right now?
    #elif(chaseTran.shouldntStopChasing(player)):
        #print "3"
        #return False

    if team.shouldStopChaseCounter > 3:
        team.shouldStopChaseCounter = 0
        team.shouldChaseCounter = 0
        return True

    return False



def shouldPositionCenter(team):
    ball = team.brain.ball

    if (not shouldPositionRight(team) and not shouldPositionLeft(team)):
        team.shouldPositionCenterCounter += 1
        if team.shouldPositionCenterCounter > 3:
            team.shouldPositionCenterCounter = 0
            team.shouldPositionLeftCounter = 0
            team.shouldPositionRightCounter = 0
        return True

    return False

def shouldPositionRight(team):
    ball = team.brain.ball

    if (ball.y < NogCon.LANDMARK_MY_GOAL_RIGHT_POST_Y - goalCon.BOX_BUFFER
        and ball.x < NogCon.MY_GOALBOX_RIGHT_X + goalCon.BOX_BUFFER):
        team.shouldPositionRightCounter += 1
        if team.shouldPositionRightCounter > 3:
            team.shouldPositionRightCounter = 0
            team.shouldPositionLeftCounter = 0
            team.shouldPositionCenterCounter = 0
        return True

    return False


def shouldPositionLeft(team):
    ball = team.brain.ball

    if (ball.y > NogCon.LANDMARK_MY_GOAL_LEFT_POST_Y + goalCon.BOX_BUFFER
        and ball.x < NogCon.MY_GOALBOX_RIGHT_X + goalCon.BOX_BUFFER):
        team.shouldPositionLeftCounter += 1
        if team.shouldPositionLeftCounter > 3:
            team.shouldPositionRightCounter = 0
            team.shouldPositionLeftCounter = 0
            team.shouldPositionCenterCounter = 0
        return True

    return False
