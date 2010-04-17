from ..playbook import PBConstants
from .. import NogginConstants as Constants
import GoalieConstants as goalCon
import ChaseBallTransitions as chaseTran
#arbitrary, currently same as AIBO

DEBUG = False

def goalieRunChecks(player):

    setRelX(player)
    setRelY(player)

    if not player.isChasing:
        if shouldChaseLoc(player):
            player.shouldChaseCounter+=1
            if DEBUG: print "should chase: ", player.shouldChaseCounter
            if player.shouldChaseCounter >= goalCon.START_CHASE_BUFFER:
                player.shouldChaseCounter = 0
                return 'chasePrepare'
        else:
            player.shouldChaseCounter = 0

    elif player.isChasing and\
                not chaseTran.shouldntStopChasing(player):
        if shouldStopChaseLoc(player) :
            player.shouldChaseCounter+=1
            if DEBUG: print "should stop chase: ", player.shouldChaseCounter
            if player.shouldChaseCounter >= goalCon.STOP_CHASE_BUFFER:
                player.shouldChaseCounter = 0
                player.isChasing = False
                return 'goaliePosition'
        else:
            player.shouldChaseCounter = 0

    return player.currentState

def useClosePosition(player):
    return (0 < player.brain.ball.x <=
            (PBConstants.BALL_LOC_LIMIT - goalCon.BUFFER))

def useFarPosition(player):
    if player.penaltyKicking:
        return False

    ball = player.brain.ball
    #switch out if we lose the ball for multiple frames
    return (not (0 <= ball.x <= PBConstants.BALL_LOC_LIMIT + goalCon.BUFFER) or
            ball.framesOff > 500)

def useLeftStrafeClose(player):
    return (player.ballRelY > goalCon.CENTER_SAVE_THRESH and
            player.stepsOffCenter < goalCon.MAX_STEPS_OFF_CENTER)

def useRightStrafeClose(player):
    return (player.ballRelY < -goalCon.CENTER_SAVE_THRESH and\
            player.stepsOffCenter > -goalCon.MAX_STEPS_OFF_CENTER)

def useLeftStrafeCloseSpeed(player):
    strafeLeftThresh = getLeftStrafeThresh(player)
    return (player.ballRelY > strafeLeftThresh and\
            player.framesFromCenter < goalCon.MAX_FRAMES_OFF_CENTER )

def useRightStrafeCloseSpeed(player):
    strafeRightThresh = getRightStrafeThresh(player)
    return (player.ballRelY < strafeRightThresh and\
            player.framesFromCenter > -goalCon.MAX_FRAMES_OFF_CENTER)

def useLeftStrafeFar(player):
    return (player.stepsOffCenter <= -goalCon.STRAFE_STEPS)

def useLeftStrafeFarSpeed(player):
    return (player.framesFromCenter <= -1)

def useRightStrafeFar(player):
    return (player.stepsOffCenter >= goalCon.STRAFE_STEPS)

def useRightStrafeFarSpeed(player):
    return (player.framesFromCenter >= 1)

def strafeLeftSpeed(player):
    player.setWalk(-.75, goalCon.STRAFE_SPEED, .25)
    player.framesFromCenter += 1

def strafeLeft(player):
    if player.setSteps(0, goalCon.STRAFE_SPEED, 0, goalCon.STRAFE_STEPS):
        player.stepsOffCenter += goalCon.STRAFE_STEPS

def strafeRightSpeed(player):
    player.setWalk(-.75, -goalCon.STRAFE_SPEED, -.25)
    player.framesFromCenter -= 1

def strafeRight(player):
    if player.setSteps(0, -goalCon.STRAFE_SPEED, 0, goalCon.STRAFE_STEPS):
        player.stepsOffCenter -= goalCon.STRAFE_STEPS

def getLeftStrafeThresh(player):
    '''return a positive number'''
    ffc = player.framesFromCenter
    if abs(ffc) > goalCon.MAX_FRAMES_OFF_CENTER:
        print "!! ffc: ", ffc
    thresh = 0
    if ffc < 0:
        thresh = goalCon.STRAFE_THRESH_ONE
    elif 0 <= ffc <= 100:
        thresh = goalCon.STRAFE_THRESH_ONE
    elif 100 < ffc <= 160:
        thresh = goalCon.STRAFE_THRESH_TWO
    elif 160 < ffc <= 220:
        thresh =  goalCon.STRAFE_THRESH_THREE
    elif 220 < ffc <= 280:
        thresh = goalCon.STRAFE_THRESH_FOUR
    elif 280 < ffc <= 340:
        thresh = goalCon.STRAFE_THRESH_FIVE
    else:
        thresh = goalCon.STRAFE_THRESH_SIX
    return thresh

def getRightStrafeThresh(player):
    '''return a negative number'''
    ffc = player.framesFromCenter
    if abs(ffc) > goalCon.MAX_FRAMES_OFF_CENTER:
        print "!! ffc: ", ffc
    thresh = 0
    if ffc > 0:
        thresh = -goalCon.STRAFE_THRESH_ONE
    elif -100 <= ffc <= 0:
        thresh = -goalCon.STRAFE_THRESH_ONE
    elif -160 <= ffc <= -100:
        thresh = -goalCon.STRAFE_THRESH_TWO
    elif -220 <= ffc <= -160:
        thresh = -goalCon.STRAFE_THRESH_THREE
    elif -280 <= ffc <= -220:
        thresh =  -goalCon.STRAFE_THRESH_FOUR
    elif -340 <= ffc <= -280:
        thresh = -goalCon.STRAFE_THRESH_FIVE
    else:
        thresh = -goalCon.STRAFE_THRESH_SIX
    return thresh

def shouldPositionForSave(player):
    ball = player.brain.ball

    # Test velocity values as to which one would work:
    relVelX = ball.relVelX

    timeUntilSave = getTimeUntilSave(player)
    # No Time, Save now
    if (5  > timeUntilSave > goalCon.BALL_SAVE_LIMIT_TIME and
        ball.framesOn > 3. and relVelX < 0.
        and player.ballRelX < goalCon.MOVE_TO_SAVE_DIST_THRESH):
        if DEBUG: player.brain.sensors.saveFrame()
        #player.printf("relVelX = %g  timeUntilSave = %g" %
                      #(relVelX, timeUntilSave))
        return True;
    return False;

def strafeDirForSave(player):
    ball = player.brain.ball
    my = player.brain.my
    timeUntilSave = getTimeUntilSave(player)
    anticipatedY = (player.ballRelY + ball.relVelY * timeUntilSave)
    if anticipatedY < my.y - goalCon.CENTER_SAVE_THRESH:
        return -1
    elif anticipatedY > my.y + goalCon.CENTER_SAVE_THRESH:
        return 1
    else:
        return 0


def shouldSave(player):
    ball = player.brain.ball

    # Test velocity values as to which one would work:
    relVelX = ball.relVelX

    if relVelX < 0.0:
        timeUntilSave = player.ballRelX / -relVelX
        if DEBUG: player.printf(("relVelX = %g   timeUntilSave = %g") %
                      (relVelX, timeUntilSave))
    else:
        timeUntilSave = -1
    # No Time, Save now
    if (timeUntilSave < goalCon.BALL_SAVE_LIMIT_TIME and
        ball.framesOn > 3. and relVelX < 0.
        and player.ballRelX < goalCon.MOVE_TO_SAVE_DIST_THRESH):
        if DEBUG: player.brain.sensors.saveFrame()
        #player.printf("relVelX = %g  timeUntilSave = %g" %
                      #(relVelX, timeUntilSave))
        return True;
    return False;

def shouldHoldSave(player):
    # same as shouldSave() except for the ball.framesOn check
    # try to come up with better conditions to test
    '''
    if the ball is still in front of me and coming at me, hold save
    if it's going to arrive anytime soon
    '''
    ball = player.brain.ball

    relVelX = ball.relVelX
    timeUntilSave = getTimeUntilSave(player)
    if timeUntilSave < goalCon.BALL_SAVE_LIMIT_TIME*2 and relVelX < 0 and\
            0 < player.ballRelX < goalCon.MOVE_TO_SAVE_DIST_THRESH:
        if DEBUG: player.brain.sensors.saveFrame()
        return True
    return False

def shouldChaseLoc(player):
    if player.penaltyKicking:
        return False

    ball = player.brain.ball
    my = player.brain.my
    if (player.squatting and ball.locDist <= goalCon.CHASE_FROM_SQUAT_DIST and
        abs(ball.locBearing) <= goalCon.CHASE_FROM_SQUAT_BEARING and
        abs(ball.velX) <= goalCon.CHASE_FROM_SQUAT_VEL and
        abs(ball.velY) <= goalCon.CHASE_FROM_SQUAT_VEL):
        return True
    if not player.squatting:
        if (ball.y > Constants.MY_GOALBOX_BOTTOM_Y + goalCon.GOALBOX_Y_REDUCTION
            and ball.y < Constants.MY_GOALBOX_TOP_Y -goalCon.GOALBOX_Y_REDUCTION
            and ball.x < Constants.MY_GOALBOX_RIGHT_X +
            goalCon.AGGRESSIVENESS_OFFSET):
            return True
    return False

def shouldStopChaseLoc(player):
    my = player.brain.my
    ball = player.brain.ball
    #change to use ball uncertainty
    if ball.uncertX < Constants.GOOD_LOC_XY_UNCERT_THRESH:
        if ball.x > (Constants.CENTER_FIELD_X/2):
            return True

    #makes sure we have seen the ball at least once for this method to return
    #true STOP_CHASE_BUFFER-times
    if ball.framesOff <= goalCon.STOP_CHASE_BUFFER - 1:
        if (ball.y < (Constants.MY_GOALBOX_BOTTOM_Y -
                      goalCon.END_CLEAR_BUFFER) or
            ball.y > (Constants.MY_GOALBOX_TOP_Y +
                      goalCon.END_CLEAR_BUFFER) or
            ball.x > (Constants.MY_GOALBOX_RIGHT_X
                      + goalCon.AGGRESSIVENESS_OFFSET +
                      goalCon.END_CLEAR_BUFFER )):
            return True

    '''
    elif (my.x < Constants.MY_GOALBOX_RIGHT_X
          or my.y < Constants.MY_GOALBOX_TOP_Y
          or my.y > Constants.MY_GOALBOX_BOTTOM_Y) and\
          (ball.locDist >= 70 or ball.dist >= 70):
          return True
    '''
    return False

def outOfPosition(player):
    if player.penaltyKicking:
        return False

    my = player.brain.my
    if (my.x > Constants.MY_GOALBOX_RIGHT_X and my.uncertX < 90)\
            or (my.x > Constants.MIDFIELD_X * 0.5):
        #print "my.x ", my.x, " my.uncertX ", my.uncertX
        return True
    return False

def dangerousBall(player):
    ball = player.brain.ball
    if (ball.x < player.brain.my.x or
        player.brain.myGoalLeftPost.on or
        player.brain.myGoalRightPost.on or
        player.brain.myGoalCrossbar.on) and ball.locDist <= 40:
        return True
    #idea: draw ray from me to ball, see if it intersects goal
    return False

def useOmni(player):
    '''the '+' and '-' for y are intentionally reversed because of the
    difficulty of using omniGoTo to get back from the points outside
    of the goalposts'''
    my = player.brain.my
    if my.x < Constants.MY_GOALBOX_RIGHT_X + goalCon.BUFFER and\
            my.y < Constants.MY_GOALBOX_TOP_Y - goalCon.BUFFER and\
            my.y > Constants.MY_GOALBOX_BOTTOM_Y + goalCon.BUFFER:
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

def getTimeUntilSave(player):
    ball = player.brain.ball
    time = 0
    if ball.relVelX < 0.0:
        time = player.ballRelX/ -ball.relVelX
    else:
        time = -1
    return time
