from ..playbook import PBConstants
from .. import NogginConstants as Constants
import GoalieConstants as goalCon
import ChaseBallTransitions as chaseTran
#arbitrary, currently same as AIBO

DEBUG = False

def goalieRunChecks(player):

    setRelX(player)
    setRelY(player)
    if not player.saving:
        '''
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
                '''
        if not player.isChasing:
            if shouldChaseLoc(player):
                player.shouldChaseCounter+=1
                if DEBUG: print "should chase: ", player.shouldChaseCounter
                if player.shouldChaseCounter >= 3:
                    player.shouldChaseCounter = 0
                    player.isChasing = True
                    return 'chase'
            else:
                player.shouldChaseCounter = 0

        elif player.isChasing:
            if shouldStopChaseLoc(player) and\
                    not chaseTran.shouldntStopChasing(player):
                player.shouldChaseCounter+=1
                if DEBUG: print "should stop chase: ", player.shouldChaseCounter
                if player.shouldChaseCounter >= 3:
                    player.shouldChaseCounter = 0
                    player.isChasing = False
                    return 'goaliePosition'
            else:
                player.shouldChaseCounter = 0

    return player.currentState

def useClosePosition(player):
    return (0 < player.brain.ball.dist <= (PBConstants.BALL_LOC_LIMIT - goalCon.BUFFER))

def useFarPosition(player):
    if player.penaltyKicking:
        return False

    ball = player.brain.ball
    #switch out if we lose the ball for multiple frames
    return (not (0 <= ball.dist <= PBConstants.BALL_LOC_LIMIT + goalCon.BUFFER) or
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
    player.setSpeed(-.75, goalCon.STRAFE_SPEED, .25)
    player.framesFromCenter += 1

def strafeLeft(player):
    if player.setSteps(0, goalCon.STRAFE_SPEED, 0, goalCon.STRAFE_STEPS):
        player.stepsOffCenter += goalCon.STRAFE_STEPS

def strafeRightSpeed(player):
    player.setSpeed(-.75, -goalCon.STRAFE_SPEED, -.25)
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
    '''if the ball is still in front of me and coming at me, hold save
    if it's going to arrive anytime soon'''
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
    if player.subRole == PBConstants.GOALIE_CHASER and\
            (ball.y > Constants.MY_GOALBOX_BOTTOM_Y - 10  and
             ball.y < Constants.MY_GOALBOX_TOP_Y + 10 and
             ball.x < Constants.MY_GOALBOX_RIGHT_X + 10):
            return True
    else:
        if (ball.y > Constants.MY_GOALBOX_BOTTOM_Y  and
            ball.y < Constants.MY_GOALBOX_TOP_Y and
            ball.x < Constants.MY_GOALBOX_RIGHT_X):
            return True
    if (my.x < Constants.MY_GOALBOX_RIGHT_X and
        my.y < Constants.MY_GOALBOX_TOP_Y and
        my.y > Constants.MY_GOALBOX_BOTTOM_Y and
        (0 < ball.locDist <= 50 or 0 < ball.dist <= 50)):
        return True
    return False

def shouldStopChaseLoc(player):
    my = player.brain.my
    ball = player.brain.ball
    if player.subRole == PBConstants.GOALIE_CHASER and\
            (ball.y < Constants.MY_GOALBOX_BOTTOM_Y - 10  and
             ball.y > Constants.MY_GOALBOX_TOP_Y + 10 and
             ball.x < Constants.MY_GOALBOX_RIGHT_X + 10):
            return True
    else:
        if (ball.y < Constants.MY_GOALBOX_BOTTOM_Y - 5  and
              ball.y > Constants.MY_GOALBOX_TOP_Y + 5 and
              ball.x > Constants.MY_GOALBOX_RIGHT_X + 5):
            return True
    if (my.x > Constants.MY_GOALBOX_RIGHT_X + PBConstants.END_CLEAR_BUFFER
        or my.y > Constants.MY_GOALBOX_TOP_Y + PBConstants.END_CLEAR_BUFFER
        or my.y < Constants.MY_GOALBOX_BOTTOM_Y + PBConstants.END_CLEAR_BUFFER):
        return True
    elif (my.x < Constants.MY_GOALBOX_RIGHT_X
          or my.y < Constants.MY_GOALBOX_TOP_Y
          or my.y > Constants.MY_GOALBOX_BOTTOM_Y) and\
          (ball.locDist >= 150 or ball.dist >= 150):
          return True
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
    if (ball.x < player.brain.my.x and ball.locDist <= 40) or \
            player.brain.myGoalLeftPost.on or\
            player.brain.myGoalRightPost.on or\
            player.brain.myGoalCrossbar.on:
        return True
    #idea: draw ray from me to ball, see if it intersects goal

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
