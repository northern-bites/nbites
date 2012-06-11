from man.motion.HeadMoves import (FIXED_PITCH_LEFT_SIDE_PAN,
                                  FIXED_PITCH_RIGHT_SIDE_PAN)
from vision import certainty
from ..navigator import Navigator as nav
from ..util import Transition
import goalie
from GoalieConstants import RIGHT, LEFT

DEBUG_OBSERVATIONS = False
DEBUG_APPROACH = True
DEBUG_POSITION = False

def updatePostObservations(player):
    """
    Updates the underlying C++ data structures.
    """
    if (player.brain.vision.ygrp.on and
        player.brain.vision.ygrp.certainty != certainty.NOT_SURE):
        player.system.pushRightPostObservation(player.brain.vision.ygrp.dist,
                                               player.brain.vision.ygrp.bearing)
        if DEBUG_OBSERVATIONS:
            print "RIGHT: Saw right post."
            print "  Pushed " + str(player.brain.vision.ygrp.bearing) + " " + str(player.brain.vision.ygrp.dist)

    if player.brain.vision.yglp.on:
        player.system.pushLeftPostObservation(player.brain.vision.yglp.dist,
                                              player.brain.vision.yglp.bearing)
        if DEBUG_OBSERVATIONS:
            print "LEFT: Saw left post."
            print "  Pushed " + str(player.brain.vision.yglp.bearing) + " " + str(player.brain.vision.yglp.dist)

def walkToGoal(player):
    """
    Has the goalie walk in the general direction of the goal.
    """
    if player.firstFrame():
        if player.side == RIGHT:
            player.brain.tracker.repeatHeadMove(FIXED_PITCH_LEFT_SIDE_PAN)
            player.system.resetPosts(goalie.RIGHT_SIDE_RP_DISTANCE,
                                     goalie.RIGHT_SIDE_RP_ANGLE,
                                     goalie.RIGHT_SIDE_LP_DISTANCE,
                                     goalie.RIGHT_SIDE_LP_ANGLE)
        else:
            player.brain.tracker.repeatHeadMove(FIXED_PITCH_RIGHT_SIDE_PAN)
            player.system.resetPosts(goalie.LEFT_SIDE_RP_DISTANCE,
                                     goalie.LEFT_SIDE_RP_ANGLE,
                                     goalie.LEFT_SIDE_LP_DISTANCE,
                                     goalie.LEFT_SIDE_LP_ANGLE)

        player.system.home.relH = 0

        player.brain.nav.goTo(player.system.home,
                              nav.CLOSE_ENOUGH, nav.FAST_SPEED)

    if DEBUG_APPROACH:
        print "========================================"

    if player.system.centerGoalDistance() > 200.0:
        updatePostObservations(player)

    player.system.home.relY = player.system.centerGoalRelY()
    player.system.home.relX = player.system.centerGoalRelX()

    if DEBUG_APPROACH:
        print "BEARINGS " + str(player.system.centerGoalBearing())
        print "  LEFT " + str(player.system.leftPostBearing())
        print "  RIGHT " + str(player.system.rightPostBearing())

        print "DISTANCES " + str(player.system.centerGoalDistance())
        print "  LEFT " + str(player.system.leftPostDistance())
        print "  RIGHT " + str(player.system.rightPostDistance())

        print "TO GET TO"
        print "  LEFT " + str(player.system.leftPostRelX()) + " " + str(player.system.leftPostRelY())
        print "  RIGHT " + str(player.system.rightPostRelX()) + " " + str(player.system.rightPostRelY())

        print "Going to " + str(player.system.home.relX) + " " + str(player.system.home.relY)

        print "FIELDEDGE DISTANCE " + str(player.brain.vision.fieldEdge.centerDist)

    return Transition.getNextState(player, walkToGoal)

def spinAtGoal(player):
    if player.firstFrame():
        player.system.home.relX = 0
        player.system.home.relY = 0
        # Decide which way to rotate based on the way we came from
        if player.side == RIGHT:
            player.system.home.relH = -90
        else:
            player.system.home.relH = 90
        player.brain.nav.goTo(player.system.home,
                              nav.CLOSE_ENOUGH, nav.CAREFUL_SPEED)

    #if player.brain.vision.cross.on:
        #player.system.pushCrossObservation(player.brain.vision.cross.bearing)

    player.brain.tracker.lookToAngle(0.0)

    return Transition.getNextState(player, spinAtGoal)

def decideSide(player):
    player.side = LEFT
    return player.goNow('walkToGoal')

def standStill(player):
    if player.firstFrame():
        player.brain.nav.stop()

    return player.stay()
