#
# The transitions for the goalie for the goalie states.
# Covers chase, position and save.
#
from .. import NogginConstants as NogCon
import GoalieConstants as goalCon

#SAVING TRANSITIONS

def shouldSave(player):
    ball = player.brain.ball

    # Not sure if we want velocity or accel here
    # and do we want ball.heat?
    if (ball.relVelX < -2 and ball.heat == 0):
        #player.shouldSaveCounter += 1
        #if player.shouldSaveCounter > 1:
            #player.shouldSaveCounter = 0
        return True

    return False

# not used right now
#should move goalie but with dive right now shouldnt need
def strafeDirForSave(player):
    ball = player.brain.ball
    my = player.brain.my
    timeUntilSave = getTimeUntilSave(player)
    anticipatedY = (ball.relY + ball.relVelY * timeUntilSave)
    if anticipatedY < my.y - goalCon.CENTER_SAVE_THRESH:
        return -1
    elif anticipatedY > my.y + goalCon.CENTER_SAVE_THRESH:
        return 1
    else:
        return 0

def shouldSaveRight(player):
    ball= player.brain.ball

    return(ball.endY < -goalCon.CENTER_SAVE_THRESH
           and ball.endY > -goalCon.DONT_SAVE_LIMIT
           and goalieInBox(player))

def shouldSaveLeft(player):
    ball= player.brain.ball

    return (ball.endY > goalCon.CENTER_SAVE_THRESH
            and ball.endY < goalCon.DONT_SAVE_LIMIT
            and goalieInBox(player))


# Not used
def shouldSaveCenter(player):
    ball= player.brain.ball

    return False

def shouldHoldSave(player):
    return player.stateTime <= goalCon.TIME_ON_GROUND

#POSITION TRANSITIONS

# player is inside the box with a small buffer
#localization not good enough for this?
def goalieInBox(player):
    my = player.brain.my

    return (my.x < NogginConstants.MY_GOALBOX_RIGHT_X + 10 and
            NogginConstants.MY_GOALBOX_TOP_Y + 10 > my.y and
            my.y > NogginConstants.MY_GOALBOX_BOTTOM_Y - 10)

#return (my.inMyGoalBox)

#CHASE TRANSITIONS

def dangerousBall(player):
    ball = player.brain.ball

    # in box and behind me and close to me
    # if inBox(player):
    return (ball.relX < 0 and goalieInBox(player))
        #and ball.dist < 30)


