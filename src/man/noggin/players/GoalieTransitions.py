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
import VisualGoalieStates
from objects import RelRobotLocation

# Visual Goalie

def atGoalArea(player):
    """
    Checks if robot is close enough to the field edge to be at the goal.
    """
    #magic number
    return player.brain.vision.fieldEdge.centerDist < 110.0

def ballIsInMyWay(player):
    """
    Checks if robot will run into ball while returning from penalty.
    """
    if not player.brain.ball.vis.on:
        return False

    return (fabs(player.brain.ball.loc.bearing -
                 player.system.centerGoalBearing()) < 20.0 and
            player.brain.ball.loc.dist < 100.0)

def foundACorner(player):
    """
    Loops through corners to find a visible goalbox corner.
    """
    if player.brain.vision.fieldLines.numCorners == 0:
        return False

    # Throw away false positives from the net!
    if player.brain.vision.fieldEdge.centerDist < 110.0:
        return False

    for corner in player.brain.vision.fieldLines.corners:
        if (IDs.YELLOW_GOAL_LEFT_L in corner.possibilities and
            corner.visualOrientation < 0 and
            corner.bearing > 0):
            VisualGoalieStates.centerAtGoalBasedOnCorners.cornerID = IDs.YELLOW_GOAL_LEFT_L
            VisualGoalieStates.centerAtGoalBasedOnCorners.cornerDirection = corner.bearing
            return True
        elif (IDs.YELLOW_GOAL_RIGHT_L in corner.possibilities and
              corner.visualOrientation > 0 and
              corner.bearing < 0):
            VisualGoalieStates.centerAtGoalBasedOnCorners.cornerID = IDs.YELLOW_GOAL_RIGHT_L
            VisualGoalieStates.centerAtGoalBasedOnCorners.cornerDirection = corner.bearing
            return True

    return False

def lostCorner(player):
    """
    Goalie no longer sees the corner it was localizing based on.
    """
    return not foundACorner(player) and player.counter > 60

def noCorner(player):
    """
    Goalie cannot find a corner to localize on. Given more time because
    the pan is very slow.
    """
    return not foundACorner(player) and player.counter > 150

def ballMoreImportant(player):
    """
    Goalie needs to chase, not localize itself.
    """
    if player.brain.ball.vis.on and player.brain.ball.vis.dist < 100.0:
        return True

    if (player.brain.ball.vis.on and player.brain.ball.vis.dist < 150.0 and
        player.aggressive):
        return True

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

def unsure(player):
    return (not onLeftSideline(player) and
            not onRightSideline(player) and
            player.counter > 60)

def onRightSideline(player):
    """
    Looks for the close posts.
    """
    if onLeftSideline(player):
        return False
    return ((player.brain.vision.ygrp.on and
             #magic numbers
             player.brain.vision.ygrp.dist < 400.0) or
            (player.brain.vision.yglp.on and
             player.brain.vision.yglp.dist < 400.0))

def shouldPerformSave(player):
    """
    Checks that the ball is moving toward it and close enough to save.
    """
    if player.penaltyKicking:
        return (player.brain.ball.vis.heat > 5.0 or
                player.brain.ball.loc.relVelX < -50.0)
    return (player.brain.ball.loc.relVelX < -50.0 and
            player.brain.ball.vis.on and
            player.brain.ball.loc.dist > 120.0 and
            player.brain.ball.loc.dist < 200.0 and
            player.brain.ball.loc.relY < 100.0)

def facingSideways(player):
    """
    If the robot is facing a post directly, it's probably turned around.
    """
    if ((player.brain.vision.yglp.on and
         fabs(player.brain.vision.yglp.bearing) < 30.0 and
         player.brain.vision.yglp.bearing != 0.0 and
         player.brain.vision.yglp.dist < 300.0) or
        (player.brain.vision.ygrp.on and
         fabs(player.brain.vision.ygrp.bearing) < 30.0 and
         player.brain.vision.ygrp.bearing != 0.0 and
         player.brain.vision.ygrp.dist < 300.0)):
        if player.brain.vision.yglp.on:
            print "Left post at " + str(player.brain.vision.yglp.bearing)
        if player.brain.vision.yglp.on:
            print "Right post at " + str(player.brain.vision.ygrp.bearing)
        return True
    else:
        return False

def shouldClearBall(player):
    """
    Checks that the ball is more or less in the goal box.
    """
    if player.penaltyKicking:
        return False

    # less than 1.5 minutes left or winning/losing badly
    shouldBeAggressive = (player.brain.comm.gc.timeRemaining() < 90 or
                          (abs(player.brain.comm.gc.teams(0)[1] -
                               player.brain.comm.gc.teams(1)[1]) > 3))
    #print "Should I be aggressive? " + str(shouldBeAggressive)
    #print "Am I aggressive? " + str(player.aggressive)
    #print ""

    if shouldBeAggressive and not player.aggressive:
        print "The goalie is now AGGRESSIVE"
        player.aggressive = True
    elif not shouldBeAggressive and player.aggressive:
        print "The goalie is no longer AGGRESSIVE"
        player.aggressive = False

    # ball must be visible
    if not player.brain.ball.vis.on:
        return False

    # if definitely within goal box
    if (player.brain.ball.vis.dist < 80.0):
        return True

    # farther out but being aggressive
    if (player.brain.ball.vis.dist < 120 and
        player.aggressive):
        return True

    # if to sides of box
    if (player.brain.ball.vis.dist < 120.0 and
        fabs(player.brain.ball.vis.bearing) > 40.0):
        return True

    # to goalie's sides, being aggressive
    if (fabs(player.brain.ball.vis.bearing) > 50.0 and
        player.aggressive):
        return True

    return False

def ballLostStopChasing(player):
    """
    If the robot does not see the ball while chasing, it is lost. Delay
    in case our shoulder pads are just hiding it.
    """
    if not player.brain.ball.vis.on and player.counter > 150:
        return True

def ballMovedStopChasing(player):
    """
    If the robot has been chasing for a while and it is far away, it should
    stop chasing.
    """
    return (player.brain.ball.vis.dist > 100.0 and
            player.counter > 200.0)

def walkedTooFar(player):
    # for the odometry reset delay
    if player.counter < 3:
        return False

    if player.aggressive:
        return False

    return (player.brain.loc.lastOdoX > 90.0 or
            fabs(player.brain.loc.lastOdoY) > 140.0)

def reachedMyDestination(player):
    """
    The robot has reached the ball after walking to it.
    """
    return player.brain.nav.isAtPosition()

def doneWalking(player):
    """
    HACK for walkTo from nav, which does not always switch to done.
    """
    return player.brain.nav.currentState == 'standing'

def successfulKick(player):
    return player.counter > 80

def whiffed(player):
    """
    If the ball is just sitting at the goalie's feet after kicking, it
    should try again.
    """
    return (player.brain.ball.loc.relX < 40.0 and
            fabs(player.brain.ball.loc.relY) < 25.0 and
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


