from math import fabs
from . import PBConstants as PBCon
from .. import NogginConstants as NogCon
from ..players import GoalieConstants as goalCon
from ..players import ChaseBallTransitions as chaseTran

#********
# GOALIE*
#********

def shouldSave(team):
    ball = team.brain.ball

     ## NEED TO FIGURE THIS OUT ###
    return False

# def shouldPosition(team):
#     my = team.brain.my
#     ball = team.brain.ball

#     # checks if in front of box or a quarter of the way up the field
#     #check if this really should be MY_GOALBOX_RIGHT_X
#     if (my.x > NogCon.MY_GOALBOX_RIGHT_X  and my.uncertX < 90):
#         if(ball.x > goalCon.CHASE_RIGHT_X_LIMIT + goalCon.CHASE_BUFFER
#            or ball.relX > CHASE_RELX_BUFFER):
#             print "ball not chaseable"
#             team.shouldChaseCounter = 0
#             team.shouldStopChaseCounter = 0
#             team.shouldPositionCenterCounter = 0
#             team.shouldPositionLeftCounter = 0
#             team.shouldPositionRightCounter = 0
#             return True
#     elif my.x > NogCon.MIDFIELD_X * 0.5:
#         print "1/4"
#         team.shouldChaseCounter = 0
#         team.shouldStopChaseCounter = 0
#         team.shouldPositionCenterCounter = 0
#         team.shouldPositionLeftCounter = 0
#         team.shouldPositionRightCounter = 0
#         return True

#     return False

def shouldChase(team):
    ball = team.brain.ball

    if (ball.framesOff > 30):
        print "no ball"
        return False

    # close enough to chase
    if (ball.x < goalCon.CHASE_RIGHT_X_LIMIT - goalCon.BOX_BUFFER
        and ball.relX < goalCon.CHASE_RELX_BUFFER
          #and ball.y > goalCon.CHASE_LOWER_Y_LIMIT
          #and ball.y < goalCon.CHASE_UPPER_Y_LIMIT
        ):
        team.shouldChaseCounter += 1

    if team.shouldChaseCounter > 3:
        team.shouldChaseCounter = 0
        team.shouldStopChaseCounter = 0
        team.shouldPositionCenterCounter = 0
        team.shouldPositionLeftCounter = 0
        team.shouldPositionRightCounter = 0
        return True

    return False

def shouldStopChase(team):
    ball= team.brain.ball
    my = team.brain.my

    if(ball.framesOff > 30):
        team.shouldStopChaseCounter = 0
        team.shouldChaseCounter = 0
        team.shouldPositionCenterCounter = 0
        team.shouldPositionLeftCounter = 0
        team.shouldPositionRightCounter = 0
        return True

    # I'm 1/4 of the way up the field
    if my.x > NogCon.MIDFIELD_X * 0.5:
        print "1/4"
        team.shouldChaseCounter = 0
        team.shouldStopChaseCounter = 0
        team.shouldPositionCenterCounter = 0
        team.shouldPositionLeftCounter = 0
        team.shouldPositionRightCounter = 0
        return True

    if (my.x > NogCon.MY_GOALBOX_RIGHT_X  and my.uncertX < 90):
        if(ball.x > goalCon.CHASE_RIGHT_X_LIMIT):
            print "ball not chaseable"
            team.shouldChaseCounter = 0
            team.shouldStopChaseCounter = 0
            team.shouldPositionCenterCounter = 0
            team.shouldPositionLeftCounter = 0
            team.shouldPositionRightCounter = 0
            return True

    elif (ball.x > goalCon.CHASE_RIGHT_X_LIMIT + goalCon.CHASE_BUFFER
        or ball.relX > goalCon.STOP_CHASE_RELX_BUFFER
          #or ball.y < goalCon.CHASE_LOWER_Y_LIMIT
          #or ball.y > goalCon.CHASE_UPPER_Y_LIMIT
        ):
        team.shouldStopChaseCounter += 1

    if team.shouldStopChaseCounter > 3:
        team.shouldStopChaseCounter = 0
        team.shouldChaseCounter = 0
        team.shouldPositionCenterCounter = 0
        team.shouldPositionLeftCounter = 0
        team.shouldPositionRightCounter = 0
        return True

    return False



def shouldPositionCenter(team):
    ball = team.brain.ball

    if ball.framesOff > 30:
        team.shouldStopChaseCounter = 0
        team.shouldChaseCounter = 0
        team.shouldPositionCenterCounter = 0
        team.shouldPositionLeftCounter = 0
        team.shouldPositionRightCounter = 0
        return True

    if ball.x > goalCon.CHASE_RIGHT_X_LIMIT:
        team.shouldPositionCenterCounter += 1
        if team.shouldPositionCenterCounter > 3:
            team.shouldStopChaseCounter = 0
            team.shouldChaseCounter = 0
            team.shouldPositionCenterCounter = 0
            team.shouldPositionLeftCounter = 0
            team.shouldPositionRightCounter = 0
        return True

    return False

def shouldPositionRight(team):
    ball = team.brain.ball

    if ball.framesOff > 30:
        return False

    if (ball.y < goalCon.CHASE_LOWER_Y_LIMIT
        and ball.x < goalCon.CHASE_RIGHT_X_LIMIT
        and ball.on):
        team.shouldPositionRightCounter += 1
        if team.shouldPositionRightCounter > 3:
            team.shouldStopChaseCounter = 0
            team.shouldChaseCounter = 0
            team.shouldPositionRightCounter = 0
            team.shouldPositionLeftCounter = 0
            team.shouldPositionCenterCounter = 0
        return True

    return False


def shouldPositionLeft(team):
    ball = team.brain.ball

    if ball.framesOff > 30:
        return False

    if (ball.y > goalCon.CHASE_UPPER_Y_LIMIT
        and ball.x < goalCon.CHASE_RIGHT_X_LIMIT
        and ball.on):
        team.shouldPositionLeftCounter += 1
        if team.shouldPositionLeftCounter > 3:
            team.shouldStopChaseCounter = 0
            team.shouldChaseCounter = 0
            team.shouldPositionRightCounter = 0
            team.shouldPositionLeftCounter = 0
            team.shouldPositionCenterCounter = 0
        return True

    return False
