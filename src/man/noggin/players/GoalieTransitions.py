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
    #magic number
    return player.brain.vision.fieldEdge.centerDist < 100.0

def veryCloseToPost(player):
    #magic numbers
    return ((player.brain.vision.ygrp.on and
             player.brain.vision.ygrp.dist < 70.0) or
            (player.brain.vision.yglp.on and
             player.brain.vision.yglp.dist < 70.0))

def facingForward(player):
    #magic numbers
    return (fabs(player.brain.vision.cross.bearing) < 10.0 and
            player.brain.vision.cross.on)

def onLeftSideline(player):
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
    return not onLeftSideline(player)

def shouldPerformSave(player):
    # Same as Dani's
    return (player.brain.ball.vis.heat <= goalCon.HEAT_LOW and
            player.brain.ball.loc.relVelX < goalCon.VEL_HIGH)

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


