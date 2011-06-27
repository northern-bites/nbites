from .. import NogginConstants as NogCon
from ..players import GoalieConstants as goalCon

#********
# GOALIE*
#********

""" We should get ready to save because a robot is
    in a position to shoot or is getting close """
def shouldPositionForSave(team):
    ball = team.brain.ball

    if ball.heat > 10 :
        team.shouldSaveCounter += 1
        if team.shouldSaveCounter > 1:
            team.shouldChaseCounter = 0
            team.shouldStopChaseCounter = 0
            team.shouldPositionCenterCounter = 0
            team.shouldPositionLeftCounter = 0
            team.shouldPositionRightCounter = 0
            team.shouldSaveCounter = 0
            return True

    return False

""" Ball is inside the field cross and we should
    get it and kick it away """
def shouldChase(team):
    ball = team.brain.ball

    # Not allowed to touch the ball outside of the box
    if (team.brain.player.penaltyKicking):
        return False

    if (ball.framesOff > 30):
        return False

    # close enough to chase
    if (ball.x < goalCon.CHASE_RIGHT_X_LIMIT - goalCon.BOX_BUFFER
        and ball.relX < goalCon.CHASE_RELX_BUFFER
        and ball.on
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
        team.shouldSaveCounter = 0
        return True

    return False

""" We should stop chasing because:
         We are 1/4 way up the field
         We have lost the ball
         We are outside the box and ball not
         chasable.
         Ball is not in chase area. """

def shouldStopChase(team):
    ball= team.brain.ball
    my = team.brain.my

    if(ball.framesOff > 30):
        team.shouldStopChaseCounter = 0
        team.shouldChaseCounter = 0
        team.shouldPositionCenterCounter = 0
        team.shouldPositionLeftCounter = 0
        team.shouldPositionRightCounter = 0
        team.shouldSaveCounter = 0
        return True

    if my.x > NogCon.MIDFIELD_X * 0.5:
        team.shouldChaseCounter = 0
        team.shouldStopChaseCounter = 0
        team.shouldPositionCenterCounter = 0
        team.shouldPositionLeftCounter = 0
        team.shouldPositionRightCounter = 0
        team.shouldSaveCounter = 0
        return True

    if (my.x > NogCon.MY_GOALBOX_RIGHT_X  and my.uncertX < 90):
        if(ball.x > goalCon.CHASE_RIGHT_X_LIMIT + goalCon.CHASE_BUFFER):
            team.shouldChaseCounter = 0
            team.shouldStopChaseCounter = 0
            team.shouldPositionCenterCounter = 0
            team.shouldPositionLeftCounter = 0
            team.shouldPositionRightCounter = 0
            team.shouldSaveCounter = 0
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
        team.shouldSaveCounter = 0
        return True

    return False

""" Right now we only position Center so this
    is not really necessary but will be when
    position mulitple places """
def shouldPositionCenter(team):
    ball = team.brain.ball

    if ball.framesOff > 30:
        team.shouldStopChaseCounter = 0
        team.shouldChaseCounter = 0
        team.shouldPositionCenterCounter = 0
        team.shouldPositionLeftCounter = 0
        team.shouldPositionRightCounter = 0
        team.shouldSaveCounter = 0
        return True

    if ball.x > goalCon.CHASE_RIGHT_X_LIMIT + goalCon.CHASE_BUFFER:
        team.shouldPositionCenterCounter += 1
        if team.shouldPositionCenterCounter > 3:
            team.shouldStopChaseCounter = 0
            team.shouldChaseCounter = 0
            team.shouldPositionCenterCounter = 0
            team.shouldPositionLeftCounter = 0
            team.shouldPositionRightCounter = 0
            team.shouldSaveCounter = 0
        return True

    return False

""" Position right not used """
def shouldPositionRight(team):
    ball = team.brain.ball

    if ball.framesOff > 30:
        return False

    if (ball.y < goalCon.CHASE_LOWER_Y_LIMIT + goalCon.CHASE_BUFFER
        and ball.x < goalCon.CHASE_RIGHT_X_LIMIT
        and ball.on):
        team.shouldPositionRightCounter += 1
        if team.shouldPositionRightCounter > 3:
            team.shouldStopChaseCounter = 0
            team.shouldChaseCounter = 0
            team.shouldPositionRightCounter = 0
            team.shouldPositionLeftCounter = 0
            team.shouldPositionCenterCounter = 0
            team.shouldSaveCounter = 0
        return True

    return False

""" Position left not used """
def shouldPositionLeft(team):
    ball = team.brain.ball

    if ball.framesOff > 30:
        return False

    if (ball.y > goalCon.CHASE_UPPER_Y_LIMIT + goalCon.CHASE_BUFFER
        and ball.x < goalCon.CHASE_RIGHT_X_LIMIT
        and ball.on):
        team.shouldPositionLeftCounter += 1
        if team.shouldPositionLeftCounter > 3:
            team.shouldStopChaseCounter = 0
            team.shouldChaseCounter = 0
            team.shouldPositionRightCounter = 0
            team.shouldPositionLeftCounter = 0
            team.shouldPositionCenterCounter = 0
            team.shouldSaveCounter = 0
        return True

    return False
