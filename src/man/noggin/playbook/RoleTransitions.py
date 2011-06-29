from .. import NogginConstants as NogCon
from ..players import GoalieConstants as goalCon

#********
# GOALIE*
#********


def shouldPositionForSave(team):
    """ We should get ready to save because a robot is
    in a position to shoot or is getting close """

    ball = team.brain.ball

    if ball.heat > goalCon.HEAT_BUFFER:
        team.shouldSaveCounter += 1
        if team.shouldSaveCounter > 1:
            resetCounters(team)
            return True

    return False


def shouldChase(team):
    """ Ball is inside the field cross and we should
    get it and kick it away """

    ball = team.brain.ball

    # Not allowed to touch the ball outside of the box
    if (team.brain.player.penaltyKicking):
        return False

    if (ball.framesOff > goalCon.BALL_LOST):
        return False

    # close enough to chase
    if (ball.x < goalCon.CHASE_RIGHT_X_LIMIT - goalCon.CHASE_BUFFER
        and ball.relX < goalCon.CHASE_RELX_BUFFER
        and ball.on
          #and ball.y > goalCon.CHASE_LOWER_Y_LIMIT
          #and ball.y < goalCon.CHASE_UPPER_Y_LIMIT
        ):
        team.shouldChaseCounter += 1

    if team.shouldChaseCounter > goalCon.CHANGE_THRESH:
        resetCounters(team)
        return True

    return False


def shouldStopChase(team):
    """ We should stop chasing because:
    We are 1/4 way up the field
    We have lost the ball
    We are outside the box and ball not chasable.
    Ball is not in chase area. """

    ball= team.brain.ball
    my = team.brain.my

    if(ball.framesOff > goalCon.BALL_LOST):
        resetCounters(team)
        return True

    if my.x > NogCon.MIDFIELD_X * 0.5:
        resetCounters(team)
        return True

    if (my.x > NogCon.MY_GOALBOX_RIGHT_X  and my.uncertX < 90):
        if(ball.x > goalCon.CHASE_RIGHT_X_LIMIT + goalCon.CHASE_BUFFER):
            resetCounters(team)
            return True

    elif (ball.x > goalCon.CHASE_RIGHT_X_LIMIT + goalCon.CHASE_BUFFER
        or ball.relX > goalCon.STOP_CHASE_RELX_BUFFER
          #or ball.y < goalCon.CHASE_LOWER_Y_LIMIT
          #or ball.y > goalCon.CHASE_UPPER_Y_LIMIT
        ):
        team.shouldStopChaseCounter += 1

    if team.shouldStopChaseCounter > goalCon.CHANGE_THRESH:
        resetCounters(team)
        return True

    return False


def shouldPositionCenter(team):
    """ Right now we only position Center so this
    is not really necessary but will be when
    position mulitple places """

    ball = team.brain.ball

    if ball.framesOff > goalCon.BALL_LOST:
        team.shouldStopChaseCounter = 0
        team.shouldChaseCounter = 0
        team.shouldPositionCenterCounter = 0
        team.shouldPositionLeftCounter = 0
        team.shouldPositionRightCounter = 0
        team.shouldSaveCounter = 0
        team.shouldStopSaveCounter = 0
        return True

    if ball.x > goalCon.CHASE_RIGHT_X_LIMIT + goalCon.CHASE_BUFFER:
        team.shouldPositionCenterCounter += 1
        if team.shouldPositionCenterCounter > goalCon.CHANGE_THRESH:
            resetCounters(team)
            return True

    return False


def shouldPositionRight(team):
    """ Position right not used """

    ball = team.brain.ball

    if ball.framesOff > goalCon.BALL_LOST:
        return False

    if (ball.y < goalCon.CHASE_LOWER_Y_LIMIT + goalCon.CHASE_BUFFER
        and ball.x < goalCon.CHASE_RIGHT_X_LIMIT
        and ball.on):
        team.shouldPositionRightCounter += 1
        if team.shouldPositionRightCounter > goalCon.CHANGE_THRESH:
            resetCounters(team)
            return True

    return False


def shouldPositionLeft(team):
    """ Position left not used """

    ball = team.brain.ball

    if ball.framesOff > goalCon.BALL_LOST:
        return False

    if (ball.y > goalCon.CHASE_UPPER_Y_LIMIT + goalCon.CHASE_BUFFER
        and ball.x < goalCon.CHASE_RIGHT_X_LIMIT
        and ball.on):
        team.shouldPositionLeftCounter += 1
        if team.shouldPositionLeftCounter > goalCon.CHANGE_THRESH:
            resetCounters(team)
            return True

    return False

def shouldNotSave(team):
    ball = team.brain.ball

    if team.brain.player.penaltyKicking:
        return False

    if (ball.relAccX < 0.5 and ball.heat == 0):
        team.shouldStopSaveCounter += 1
        if team.shouldStopSaveCounter > 30:
            resetCounters(team)
            return True

    # Want to stop saving when no longer worried about
    # A robot shooting and the ball is not moving ??

    return False


# Reset counters for role transitions
def resetCounters(team):

    team.shouldStopChaseCounter = 0
    team.shouldChaseCounter = 0
    team.shouldPositionRightCounter = 0
    team.shouldPositionLeftCounter = 0
    team.shouldPositionCenterCounter = 0
    team.shouldSaveCounter = 0
    team.shouldStopSaveCounter = 0
