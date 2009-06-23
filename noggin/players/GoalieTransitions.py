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
DEBUG = False

def goalieRunChecks(player):

    setRelX(player)
    setRelY(player)
    if not player.saving:
        if shouldSave(player):
            player.shouldSaveCounter += 1
            if player.shouldSaveCounter >= 2:
                player.shouldSaveCounter = 0
                return 'goalieSave'
            else:
                player.shouldSaveCounter = 0

    if player.currentState != 'goaliePositionForSave':
        if shouldPositionForSave(player):
            player.posForSaveCounter += 1
            if player.posForSaveCounter >= 2:
                player.posForSaveCounter = 0
                return 'goaliePositionForSave'
            else:
                player.posForSaveCounter = 0

    if not player.isChasing:
        if shouldChaseLoc(player):
            player.shouldChaseCounter+=1
            print "should chase: ", player.shouldChaseCounter
            if player.shouldChaseCounter >= 3:
                player.shouldChaseCounter = 0
                player.isChasing = True
                return 'chase'
        else:
            player.shouldChaseCounter = 0

    elif player.isChasing:
        if shouldStopChaseLoc(player):
            player.shouldChaseCounter+=1
            print "should stop chase: ", player.shouldChaseCounter
            if player.shouldChaseCounter >= 3:
                player.shouldChaseCounter = 0
                player.isChasing = False
                return 'goaliePosition'
        else:
            player.shouldChaseCounter = 0

    return player.currentState

def useClosePosition(player):
    return (0 < player.brain.ball.dist <= (PBConstants.BALL_LOC_LIMIT - BUFFER))

def useFarPosition(player):
    ball = player.brain.ball
    #switch out if we lose the ball for multiple frames
    return (not (0 <= ball.dist <= PBConstants.BALL_LOC_LIMIT + BUFFER) or
            ball.framesOff > 150)

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

def shouldPositionForSave(player):
    ball = player.brain.ball

    # Test velocity values as to which one would work:
    relVelX = ball.relVelX

    if relVelX < 0.0:
        timeUntilSave = player.ballRelX / -relVelX
        player.printf(("relVelX = %g   timeUntilSave = %g") %
                      (relVelX, timeUntilSave))
    else:
        timeUntilSave = -1
    # No Time, Save now
    if (20  > timeUntilSave > BALL_SAVE_LIMIT_TIME and
        ball.framesOn > 3. and relVelX < 0.
        and player.ballRelX < MOVE_TO_SAVE_DIST_THRESH):
        if DEBUG: player.brain.sensors.saveFrame()
        #player.printf("relVelX = %g  timeUntilSave = %g" %
                      #(relVelX, timeUntilSave))
        return True;
    return False;

def strafeDirForSave(player):
    ball = player.brain.ball
    my = player.brain.my
    timeUntilSave = player.ballRelX / -ball.relVelX
    anticipatedY = (player.ballRelY + ball.relVelY * timeUntilSave)
    if anticipatedY < my.y - CENTER_SAVE_THRESH:
        return 'right'
    elif anticipatedY > my.y + CENTER_SAVE_THRESH:
        return 'left'
    else:
        return 'none'


def shouldSave(player):
    ball = player.brain.ball

    # Test velocity values as to which one would work:
    relVelX = ball.relVelX

    if relVelX < 0.0:
        timeUntilSave = player.ballRelX / -relVelX
        player.printf(("relVelX = %g   timeUntilSave = %g") %
                      (relVelX, timeUntilSave))
    else:
        timeUntilSave = -1
    # No Time, Save now
    if (timeUntilSave < BALL_SAVE_LIMIT_TIME and
        ball.framesOn > 3. and relVelX < 0.
        and player.ballRelX < MOVE_TO_SAVE_DIST_THRESH):
        if DEBUG: player.brain.sensors.saveFrame()
        #player.printf("relVelX = %g  timeUntilSave = %g" %
                      #(relVelX, timeUntilSave))
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
    if timeUntilSave < BALL_SAVE_LIMIT_TIME*2 and relVelX < 0 and\
            0 < player.ballRelX < MOVE_TO_SAVE_DIST_THRESH:
        if DEBUG: player.brain.sensors.saveFrame()
        return True
    return False

def shouldChaseLoc(player):
    ball = player.brain.ball
    my = player.brain.my
    if (ball.y > Constants.MY_GOALBOX_BOTTOM_Y - 10  and
        ball.y < Constants.MY_GOALBOX_TOP_Y + 10 and
        ball.x < Constants.MY_GOALBOX_RIGHT_X + 10) and\
        player.subRole == PBConstants.GOALIE_CHASER:
        return True
    elif (ball.y > Constants.MY_GOALBOX_BOTTOM_Y - 5  and
          ball.y < Constants.MY_GOALBOX_TOP_Y + 5 and
          ball.x < Constants.MY_GOALBOX_RIGHT_X + 5):
        return True
    elif (my.x < Constants.MY_GOALBOX_RIGHT_X + 5 and
          my.y < Constants.MY_GOALBOX_TOP_Y + 5 and
          my.y > Constants.MY_GOALBOX_BOTTOM_Y - 5 and
          (0 < ball.locDist <= 60 or 0 < ball.dist <= 60)):
        return True
    return False

def shouldStopChaseLoc(player):
    my = player.brain.my
    if (my.x > Constants.MY_GOALBOX_RIGHT_X + PBConstants.END_CLEAR_BUFFER
        or my.y > Constants.MY_GOALBOX_TOP_Y + PBConstants.END_CLEAR_BUFFER
        or my.y < Constants.MY_GOALBOX_BOTTOM_Y + PBConstants.END_CLEAR_BUFFER):
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
        player.ballRelY = ball.locRelY

def setRelX(player):
    ball = player.brain.ball
    if ball.on:
        player.ballRelX = ball.relX
    else:
        player.ballRelX = ball.locRelX
