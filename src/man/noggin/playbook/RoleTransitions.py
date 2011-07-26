import noggin_constants as NogCon
from ..players import GoalieConstants as goalCon

#********
# GOALIE*
#********
#
# The transitions decide if the goalie is going to change roles.
# They have counters built in which keep track of the number of
# times the transition is true.

def shouldPositionForSave(team):
    """ We should get ready to save because a robot is
    in a position to shoot or is getting close """

    ball = team.brain.ball

    if ball.vis.heat > goalCon.HEAT_BUFFER:
        team.shouldSaveCounter += 1
        if team.shouldSaveCounter > goalCon.CHANGE_THRESH:
            team.resetGoalieRoleCounters()
            return True

    elif ball.loc.relAccX < goalCon.ACCEL_SAVE_THRESH:
        team.shouldSaveCounter += 1
        if team.shouldSaveCounter > goalCon.CHANGE_THRESH:
            team.resetGoalieRoleCounters()
            return True

    else:
        team.shouldSaveCounter = 0
        return False

def shouldNotSave(team):
    """ Want to stop saving when no longer worried about
    A robot shooting and the ball is not moving """

    ball = team.brain.ball

    if team.brain.player.penaltyKicking:
        return False

    if (ball.loc.relAccX < goalCon.BALL_NO_MOVEMENT
        and ball.vis.heat == 0):
        team.shouldStopSaveCounter += 1
        if team.shouldStopSaveCounter > goalCon.OUT_OF_SAVE:
            team.brain.player.isSaving = False
            team.resetGoalieRoleCounters()
            return True

    elif (ball.loc.inMyGoalBox() and
          ball.loc.relAccX < goalCon.BALL_NO_MOVEMENT):
        team.shouldStopSaveCounter += 1
        if team.shouldStopSaveCounter > goalCon.OUT_OF_SAVE:
            team.brain.player.isSaving = False
            team.resetGoalieRoleCounters()
            return True

    return False

def shouldChase(team):
    """ Ball is inside the field cross and we should
    get it and kick it away """

    ball = team.brain.ball

    # Not allowed to touch the ball outside of the box
    if (team.brain.player.penaltyKicking):
        return False

    if (ball.vis.framesOff > goalCon.BALL_LOST):
        return False

    if (ball.loc.x < goalCon.CHASE_RIGHT_X_LIMIT - goalCon.CHASE_BUFFER
        and ball.loc.relX < goalCon.CHASE_RELX_BUFFER
        and ball.vis.on):
        team.shouldChaseCounter += 1

    if team.shouldChaseCounter > goalCon.CHANGE_THRESH:
        team.resetGoalieRoleCounters()
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

    if(ball.vis.framesOff > goalCon.BALL_LOST):
        team.resetGoalieRoleCounters()
        return True

    if my.x > NogCon.MIDFIELD_X * 0.5:
        team.resetGoalieRoleCounters()
        return True

    if (my.x > NogCon.MY_GOALBOX_RIGHT_X):
        if(ball.loc.x > goalCon.CHASE_RIGHT_X_LIMIT):
            team.resetGoalieRoleCounters()
            return True

    if (ball.loc.x > goalCon.CHASE_RIGHT_X_LIMIT
        or ball.loc.relX > goalCon.STOP_CHASE_RELX_BUFFER):
        team.shouldStopChaseCounter += 1

    if team.shouldStopChaseCounter > goalCon.CHANGE_THRESH:
        team.resetGoalieRoleCounters()
        return True

    return False


def shouldPositionCenter(team):
    """ Right now we only position Center so this
    is not really necessary but will be when
    position mulitple places """

    ball = team.brain.ball

    if ball.vis.framesOff > goalCon.BALL_LOST:
        team.resetGoalieRoleCounters()
        return True

    if ball.loc.x > goalCon.CHASE_RIGHT_X_LIMIT + goalCon.CHASE_BUFFER:
        team.shouldPositionCenterCounter += 1
        if team.shouldPositionCenterCounter > goalCon.CHANGE_THRESH:
            team.resetGoalieRoleCounters()
            return True

    return False


def shouldPositionRight(team):
    """ Position right not used """

    ball = team.brain.ball

    if ball.vis.framesOff > goalCon.BALL_LOST:
        return False

    if (ball.loc.y < goalCon.CHASE_LOWER_Y_LIMIT + goalCon.CHASE_BUFFER
        and ball.loc.x < goalCon.CHASE_RIGHT_X_LIMIT
        and ball.vis.on):
        team.shouldPositionRightCounter += 1
        if team.shouldPositionRightCounter > goalCon.CHANGE_THRESH:
            team.resetGoalieRoleCounters()
            return True

    return False


def shouldPositionLeft(team):
    """ Position left not used """

    ball = team.brain.ball

    if ball.vis.framesOff > goalCon.BALL_LOST:
        return False

    if (ball.loc.y > goalCon.CHASE_UPPER_Y_LIMIT + goalCon.CHASE_BUFFER
        and ball.loc.x < goalCon.CHASE_RIGHT_X_LIMIT
        and ball.vis.on):
        team.shouldPositionLeftCounter += 1
        if team.shouldPositionLeftCounter > goalCon.CHANGE_THRESH:
            team.resetGoalieRoleCounters()
            return True

    return False



