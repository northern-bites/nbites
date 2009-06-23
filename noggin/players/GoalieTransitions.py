from ..playbook import PBConstants
from .. import NogginConstants as Constants
#arbitrary, currently same as AIBO
BALL_SAVE_LIMIT_TIME = 2.5
MOVE_TO_SAVE_DIST_THRESH = 200.
CENTER_SAVE_THRESH = 15.
ORTHO_GOTO_THRESH = Constants.CENTER_FIELD_X * 0.5
STRAFE_ONLY = True
STRAFE_SPEED = 6
STRAFE_STEPS = 5
MAX_STEPS_OFF_CENTER = 50
BUFFER = 50

def goalieRunChecks(player):

    setRelX(player)
    setRelY(player)

    if shouldSave(player):
        player.shouldSaveCounter += 1
        if player.shouldSaveCounter >= 2:
            player.shouldSaveCounter = 0
            return player.goNow('goalieSave')

    if player.subRole == PBConstants.GOALIE_CHASER:
        if shouldStopChase(player):
            player.shouldChaseCounter+=1
            if player.shouldChaseCounter >= 3:
                player.shouldChaseCounter = 0
                return player.goNow('goaliePosition')

    elif shouldChaseLoc(player):
        player.shouldChaseCounter+=1
        if player.shouldChaseCounter >= 3:
            player.shouldChaseCounter = 0
            return player.goNow('chase')

def useClosePosition(player):
    return (0 < player.brain.ball.dist <= (PBConstants.BALL_LOC_LIMIT - BUFFER))

def useFarPosition(player):
    ball = player.brain.ball
    #switch out if we lose the ball for multiple frames
    return (not (0 <= ball.dist <= PBConstants.BALL_LOC_LIMIT + BUFFER) or
        ball.framesOff > 15)

def useLeftStrafeClose(player):
    return (player.ballRelY > CENTER_SAVE_THRESH and\
            player.stepsOffCenter < MAX_STEPS_OFF_CENTER )

def useRightStrafeClose(player):
    return (player.ballRelY < -CENTER_SAVE_THRESH and\
            player.stepsOffCenter > -MAX_STEPS_OFF_CENTER)

def useLeftStrafeFar(player):
    return (player.stepsOffCenter <= -STRAFE_STEPS)

def useRightStrafeFar(player):
    return (player.stepsOffCenter >= STRAFE_STEPS)

def strafeLeft(player):
    if player.setSteps(0, STRAFE_SPEED, 0, STRAFE_STEPS):
        player.stepsOffCenter += STRAFE_STEPS

def strafeRight(player):
    if player.setSteps(0, -STRAFE_SPEED, 0, STRAFE_STEPS):
        player.stepsOffCenter -= STRAFE_STEPS

def shouldSave(player):
    ball = player.brain.ball

    # Test velocity values as to which one would work:
    relVelX = ball.relVelX

    if relVelX < 0.0:
        timeUntilSave = player.ballRelX / -relVelX
    else:
        timeUntilSave = -1
        '''player.printf(("relVelX = ", relVelX, " relVelY = ", relVelY,
        " timeUntilSave = ", timeUntilSave,
        " anticipatedY = ", anticipatedY))'''
    # No Time, Save now
    if (timeUntilSave < BALL_SAVE_LIMIT_TIME and
        ball.framesOn > 3. and relVelX < 0.
        and 0 < player.ballRelX < MOVE_TO_SAVE_DIST_THRESH):
        #player.brain.sensors.saveFrame()
        #player.printf("relVelX = %g  relVelY = %g  timeUntilSave = %g" %
                      #(relVelX, relVelY, timeUntilSave))
        return True;
    return False;

def shouldHoldSave(player):
    # same as shouldSave() except for the ball.framesOn check
    # try to come up with better conditions to test
    '''if the ball is still in front of me and coming at me, hold save
    if it's going to arrive anytime soon'''
    ball = player.brain.ball

    relVelX = ball.relVelX
    if relVelX < 0.0:
        timeUntilSave = player.ballRelX/ -relVelX
    else:
        timeUntilSave = -1
    if 0 < timeUntilSave < BALL_SAVE_LIMIT_TIME*2 and relVelX < 0 and\
            0 < player.ballRelX < MOVE_TO_SAVE_DIST_THRESH:
        return True
    return False

def shouldChase(player):
    if (player.subRole == PBConstants.GOALIE_CHASER)\
        and (0 < player.brain.ball.locDist <= 60):
        return True
    return False

def shouldChaseLoc(player):
    ball = player.brain.ball
    if (ball.y > Constants.MY_GOALBOX_BOTTOM_Y and
        ball.y < Constants.MY_GOALBOX_TOP_Y and
        ball.x < Constants.MY_GOALBOX_RIGHT_X) and\
        player.subRole == PBConstants.GOALIE_CHASER:
        return True
    return False

def shouldStopChaseLoc(player):
    my = player.brain.my
    if (my.x > Constants.MY_GOALBOX_RIGHT_X + PBConstants.END_CLEAR_BUFFER
        or my.y > Constants.MY_GOALBOX_TOP_Y + PBConstants.END_CLEAR_BUFFER
        or my.y < Constants.MY_GOALBOX_BOTTOM_Y + PBConstants.END_CLEAR_BUFFER):
        return True
    return False

def shouldStopChase(player):
    if player.brain.ball.locDist > 60:
        return True
    return False

def outOfPosition(player):
    my = player.brain.my
    if ((my.x > Constants.MY_GOALBOX_RIGHT_X + 20) and my.uncertX < 90)\
            or (my.x > Constants.MIDFIELD_X * 0.5):
        #print "my.x ", my.x, " my.uncertX ", my.uncertX
        return True
    return False

def setRelY(player):
    ball = player.brain.ball
    if ball.on:
        player.ballRelY = ball.relY
    else:
        player.ballRelY = 0

def setRelX(player):
    ball = player.brain.ball
    if ball.on:
        player.ballRelX = ball.relX
    else:
        player.ballRelX = ball.locRelX
