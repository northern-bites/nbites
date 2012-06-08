#
# The transitions for the goalie for the goalie states.
# Covers chase, position and save. Most of the counters
# for transitions are contained in these transitions.
#

import noggin_constants as NogCon
import GoalieConstants as goalCon
import PositionTransitions as PosTran

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


