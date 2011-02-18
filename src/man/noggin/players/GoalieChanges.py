# new file for goalie decisions
# want to make in similar format to rest of the code?

from ..playbook import PBConstants as PBCon
from .. import NogginConstants as NogCon
import GoalieConstants as goalCon
import ChaseBallTransitions as chaseTran


def isDangerousBall(player):
    ball = player.brain.ball

    if (ball.RelX < 0 and ball.x < (NogCon.MY_GOALBOX_RIGHT_X + goalCon.BOX_BUFFER)
        and ball.y < (NogCon.MY_GOALBOX_TOP_Y + goalCon.BOX_BUFFER)
        and ball.y > (NogCon.MY_GOALBOX_BOTTOM_Y - goalCon.BOX_BUFFER))::
        return True

    return False

def shouldPrepareToSave(player):
    # Ball is moving toward the goal????

    return False

def shouldSave(player):
    #Ball is within distance from the goalie and
    #is prepared to save

    return False

def outOfPosition(player):

    if player.isChasing:
        return False

    # if not in player 1,2 or 3 

def shouldPositionCenter(player):
    ball = player.brain.ball

    if (shouldPositionRight(player)
        or shouldPositionLeft(player)):
        return False

    return True

def shouldPositionRight(player):
    ball = player.brain.ball

    if (ball.y < NogCon.LANDMARK_MY_GOAL_RIGHT_POST_Y - goalCon.BOX_BUFFER
        and ball.x < NogCon.MY_GOALBOX_RIGHT_X + goalCon.BOX_BUFFER):
        return True

    return False
    

def shouldPositionLeft(player):
    ball = player.brain.ball

    if (ball.y > NogCon.LANDMARK_MY_GOAL_LEFT_POST_Y + goalCon.BOX_BUFFER
        and ball.x < NogCon.MY_GOALBOX_RIGHT_X + goalCon.BOX_BUFFER):
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
    
    if player.currentlySaving:
        return False

    elif ball.RelX < 0:
        return False
    
    #elif (ball.x < (NogCon.MY_GOALBOX_RIGHT_X - BOX_BUFFER)
    #or ball.y < (NogCon.MY_GOALBOX_TOP_Y - BOX_BUFFER)
    #or ball.y > (NogCon.MY_GOALBOX_BOTTOM_Y + BOX_BUFFER)):
     #   return False

    elif (ball.x > goalCon.CHASE_RIGHT_X_LIMIT
          or ball.x < goalCon.CHASE_LEFT_X_LIMIT
          or ball.y < goalCon.CHASE_LOWER_Y_LIMIT
          or ball.y > goalCon.CHASE_UPPER_Y_LIMIT):
        return False

    # if not closest player return false
    
    return True

#Should stop chasing if
    #Ball is outside of the chase range
    #Ball is behind the goalie
def shouldStopChase(player):
    ball= player.brain.ball

    if (ball.x > goalCon.CHASE_RIGHT_X_LIMIT
        or ball.x < goalCon.CHASE_LEFT_X_LIMIT
        or ball.y < goalCon.CHASE_LOWER_Y_LIMIT
        or ball.y > goalCon.CHASE_UPPER_Y_LIMIT):
        return True

    elif ball.RelX < goalCon.CLOSE_BEHIND_GOALIE:
        return True


    return False


