# new file for goalie decisions
# want to make in similar format to rest of the code?

from ..playbook import PBConstants as PBCon
from .. import NogginConstants as NogCon
import GoalieConstants as goalCon
import ChaseBallTransitions as chaseTran
#am I suposed to do this?
from ..playbook import GoTeam as GoTeam


def isDangerousBall(player):
    ball = player.brain.ball

    # in box and behind me
    if (ball.RelX < 0 and ball.x < (NogCon.MY_GOALBOX_RIGHT_X + goalCon.BOX_BUFFER)
        and ball.y < (NogCon.MY_GOALBOX_TOP_Y + goalCon.BOX_BUFFER)
        and ball.y > (NogCon.MY_GOALBOX_BOTTOM_Y - goalCon.BOX_BUFFER)
        and ball. c > (Nogcon.MY_GOALBOX_LEFT_X + goalCon.BOX_BUFFER)):
        return True

    return False

#add a counter 
#player.shouldSaveCounter already exists
def shouldPrepareToSave(player):
    #need to test velocity values
    if (abs(dx) > goalCon.VEL_THRES):
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

def outOfPosition(player):

    if player.isChasing:
        return False
    elif player.isSaving:
        return False

    # ask lizzie about how to check this

    return True

    # if not in position 1,2 or 3 

def shouldPositionCenter(player):
    ball = player.brain.ball

    if player.isChasing or player.isSaving:
        return False

    if (shouldPositionRight(player)
        or shouldPositionLeft(player)):
        return False

    return True

def shouldPositionRight(player):
    ball = player.brain.ball

    if player.isChasing or player.isSaving:
        return False

    elif (ball.y < NogCon.LANDMARK_MY_GOAL_RIGHT_POST_Y - goalCon.BOX_BUFFER
        and ball.x < NogCon.MY_GOALBOX_RIGHT_X + goalCon.BOX_BUFFER):
        player.shouldPositionRightCounter += 1
        if player.shouldPositionRightCounter > 3:
            player.shouldPositionRightCounter = 0
            return True

    return False
    

def shouldPositionLeft(player):
    ball = player.brain.ball

    if player. isChasing or player.isSaving:
        return False

    if (ball.y > NogCon.LANDMARK_MY_GOAL_LEFT_POST_Y + goalCon.BOX_BUFFER
        and ball.x < NogCon.MY_GOALBOX_RIGHT_X + goalCon.BOX_BUFFER):
        player.shouldPositionLeftCounter += 1
        if player.shouldPositionLeftCounter > 3:
            player.shouldPositionLeftCounter = 0
            return True

    return False

#Decisions for when to chase.
#Will chase when:
    # Inside the chasing box
    # Not inside the goalie box
    # Not saving
    # Closest Player to the ball
def shouldChase(player):
    ball = player.brain.ball
    
    # in the middle of saving
    if player.isSaving:
        return False

    #behind you and in box  want to aprroach dangerous ball
    if (ball.RelX < 0 and ball.y < NogCon.MY_GOALBOX_BOTTOM_Y 
        and ball.y > NogCon.MY_GOALBOX_TOP_Y):
        return False
    
    # checks if others are chasing
    # what checks who is closest person to ball
    elif not goTeam.goalieShouldChase(player):
        return False

    # close enough to chase
    elif (ball.x > goalCon.CHASE_RIGHT_X_LIMIT
          or ball.x < goalCon.CHASE_LEFT_X_LIMIT
          or ball.y < goalCon.CHASE_LOWER_Y_LIMIT
          or ball.y > goalCon.CHASE_UPPER_Y_LIMIT):
        return False
    
    
    return True

#Should stop chasing if
    #Ball is outside of the chase range
    #Ball is behind the goalie
def shouldStopChase(player):
    ball= player.brain.ball

    if not player.isChasing:
        return False

    if(chaseTran.shouldntStopChasing(player)):
        return False

    if (ball.x > goalCon.CHASE_RIGHT_X_LIMIT
        or ball.x < goalCon.CHASE_LEFT_X_LIMIT
        or ball.y < goalCon.CHASE_LOWER_Y_LIMIT
        or ball.y > goalCon.CHASE_UPPER_Y_LIMIT):
        player.shouldStopChaseCounter += 1

    elif ball.RelX < goalCon.CLOSE_BEHIND_GOALIE:
        player.shouldStopChaseCounter += 1
    
    if player.shouldStopChaseCounter > 3:
        player.shouldStopChaseCounter = 0
        return True

    return False

def shouldDiveRight(player):
    ball= player.brain.ball

    if ball.dx < goalCon.VEL_THRES and ball.dx > -goalCon.VEL_THRES:
        return False

    elif(ball.endY > goalCon.CENTER_SAVE_THRESH):
        player.countRightSave += 1
        if(player.countRightSave > 3):
            player.counterRightSave = 0
            return True
    return False

def shouldDiveLeft(player):
    ball= player.brain.ball

    if ball.dx < goalCon.VEL_THRES and ball.dx > -goalCon.VEL_THRES:
        return False

    if(ball.endY < -goalCon.CENTER_SAVE_THRESH):
        player.countLeftSave += 1
        if( player.counterLeftSave > 3) :
            player.counterLeftSave = 0
            return True
    return False

def shouldSaveCenter(player):
    ball= player.brain.ball

    if(ball.dx < goalCon.VEL_THRES and ball.dx > -goalCon.VEL_THRES):
        if ball.relX < 10:
            return True

    elif( not shouldDiveRight and not shouldDiveLeft):
        player.countCenterSave += 1
        if (player.countCenterSave > 3):
            player.counterCenterSave = 0
            return True
    return False
            
