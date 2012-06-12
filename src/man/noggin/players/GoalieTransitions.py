#
# The transitions for the goalie for the goalie states.
# Covers chase, position and save. Most of the counters
# for transitions are contained in these transitions.
#

import noggin_constants as NogCon
import GoalieConstants as goalCon
import PositionTransitions as PosTran
from math import fabs
from vision import cornerID as IDs

# Visual Goalie

def atGoalArea(player):
    """
    Checks if robot is close enough to the field edge to be at the goal.
    """
    #magic number
    return player.brain.vision.fieldEdge.centerDist < 110.0

def facingForward(player):
    """
    Checks if a robot is facing the cross, which is more or less forward
    if it is in the goal.
    """
    #magic numbers
    return (player.brain.vision.fieldEdge.centerDist > 800.0 or
            (fabs(player.brain.vision.cross.bearing) < 10.0 and
             player.brain.vision.cross.on))

def facingBall(player):
    """
    Checks if the ball is right in front of it.
    """
    #magic numbers
    return (fabs(player.brain.ball.vis.bearing) < 10.0 and
            player.brain.ball.vis.on)

def onLeftSideline(player):
    """
    Looks for a T corner or far goals to determine which sideline it's
    standing on.
    """
    for corner in player.brain.vision.fieldLines.corners:
        if ( (IDs.CENTER_TOP_T in corner.possibilities) or
             (IDs.CENTER_BOTTOM_T in corner.possibilities) ) :
            return True
    return ((player.brain.vision.ygrp.on and
             #magic numbers
             player.brain.vision.ygrp.dist > 400.0) or
            (player.brain.vision.yglp.on and
             player.brain.vision.yglp.dist > 400.0))

def onRightSideline(player):
    """
    Checks that it is definitely not on the left sideline.
    """
    return not onLeftSideline(player)

def shouldPerformSave(player):
    """
    Checks that the ball is moving toward it and close enough to save.
    """
    return (player.brain.ball.loc.relVelX < -50.0 and
            player.brain.ball.vis.on and
            player.brain.ball.loc.dist < 150.0)

def shouldClearBall(player):
    """
    Checks that the ball is more or less in the goal box.
    """
    # ball must be visible
    if not player.brain.ball.vis.on:
        return False

    # if definitely within goal box
    if (player.brain.ball.vis.dist < 70.0):
        return True

    # if to sides of box
    if (player.brain.ball.vis.dist < 120.0 and
        fabs(player.brain.ball.vis.bearing) > 45.0):
        return True

    return False

def ballLostStopChasing(player):
    """
    If the robot does not see the ball while chasing, it is lost.
    """
    if not player.brain.ball.vis.on:
        return False

def ballMovedStopChasing(player):
    """
    If the robot has been chasing for a while and it is far away, it should
    stop chasing.
    """
    return (player.brain.ball.vis.dist > 100.0 and
            player.counter > 200.0)

def reachedTheBall(player):
    """
    The robot has reached the ball after walking to it.
    """
    return player.brain.nav.isAtPosition()

def doneWalking(player):
    """
    HACK for walkTo from nav, which does not always switch to done.
    """
    return player.brain.nav.currentState == 'standing'

def ballIsAtMyFeet(player):
    """
    If the ball is just sitting at the goalie's feet, it should kick fast.
    """
    return (player.brain.ball.loc.dist < 18.0 and
            fabs(player.brain.ball.loc.relX) < 18.0 and
            player.brain.ball.loc.relY < 1.0 and
            player.brain.ball.vis.on)

# ******************
# SAVING TRANSITIONS
# ******************

def shouldSave(player):
    """
    Decision on if goalie should prepare to save
    """
    ball = player.brain.ball

    if(ball.loc.relVelX < goalCon.VEL_HIGH
       and ball.vis.heat <= goalCon.HEAT_LOW):
        player.shouldSaveCounter += 1
        if player.shouldSaveCounter > 1:
            player.shouldSaveCounter = 0
            return True

    else:
        player.shouldSaveCounter = 0
        return False

# NOT USED
# Unsure if it works
def strafeDirForSave(player):
    ball = player.brain.ball
    my = player.brain.my
    timeUntilSave = getTimeUntilSave(player)
    anticipatedY = (ball.loc.relY + ball.loc.relVelY * timeUntilSave)
    if anticipatedY < my.y - goalCon.CENTER_SAVE_THRESH:
        return -1
    elif anticipatedY > my.y + goalCon.CENTER_SAVE_THRESH:
        return 1
    else:
        return 0

def shouldSaveRight(player):
    """
    Decision for saving diving right
    """
    ball= player.brain.ball
    my = player.brain.my

    return(ball.loc.endY < -goalCon.CENTER_SAVE_THRESH
           and ball.loc.endY > -goalCon.DONT_SAVE_LIMIT
           and goalieInBox(player))

def shouldSaveLeft(player):
    """
    Decision for saving diving left
    """
    ball= player.brain.ball
    my = player.brain.my

    return (ball.loc.endY > goalCon.CENTER_SAVE_THRESH
            and ball.loc.endY < goalCon.DONT_SAVE_LIMIT
            and goalieInBox(player))

# Not used
# If you should save but you shouldnt
# dive you will always center save
def shouldSaveCenter(player):
    ball= player.brain.ball

    return False

def shouldHoldSave(player):
    """
    Decision to keep goalie in save.
    If penalty kick then dont ever get
    up.
    """
    return (player.penaltyKicking or
            player.stateTime <= goalCon.TIME_ON_GROUND)

# ********************
# POSITION TRANSITIONS
# ********************

def goalieInBox(player):
    """
    The goalie is in its box
    """
    return player.brain.my.inMyGoalBox()

def goalieIsLost(player):
    """
    Goalie is facing off field
    """
    return PosTran.leavingTheField(player)

# *****************
# CHASE TRANSITIONS
# *****************

def dangerousBall(player):
    """
    The ball is in the box behind the goalie
    """

    return (player.brain.ball.loc.relX < 0 and goalieInBox(player))


