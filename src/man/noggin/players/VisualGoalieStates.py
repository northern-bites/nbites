from man.motion.HeadMoves import FIXED_PITCH_LEFT_SIDE_PAN
from vision import certainty
from ..navigator import Navigator as nav

def updatePostObservations(player):
    """
    Updates the underlying C++ data structures.
    """
    if player.brain.vision.ygrp.on and player.brain.vision.ygrp.certainty != certainty.NOT_SURE:
        print "RIGHT: Saw right post."
        player.system.pushRightPostObservation(player.brain.vision.ygrp.dist,
                                               player.brain.vision.ygrp.bearing)
        print "  Pushed " + str(player.brain.vision.ygrp.bearing) + " " + str(player.brain.vision.ygrp.dist)
    if player.brain.vision.yglp.on:
        print "LEFT: Saw left post."
        player.system.pushLeftPostObservation(player.brain.vision.yglp.dist,
                                              player.brain.vision.yglp.bearing)
        print "  Pushed " + str(player.brain.vision.yglp.bearing) + " " + str(player.brain.vision.yglp.dist)

def walkToGoal(player):
    print "========================================"
    if player.firstFrame():
        player.brain.tracker.repeatHeadMove(FIXED_PITCH_LEFT_SIDE_PAN)
        player.brain.nav.goTo(player.system.home,
                              nav.CLOSE_ENOUGH, nav.FAST_SPEED)

    updatePostObservations(player)

    player.system.home.relY = player.system.centerGoalRelY()
    player.system.home.relX = player.system.centerGoalRelX()

    # switch to 1 post when close?
    # if player.system.home.relY < 120:
    #     player.system.home.relY = player.system.leftPostRelY()
    #     player.system.home.relX = player.system.leftPostRelX() - 70.0
    #     player.brain.tracker.lookToAngle(player.system.leftPostBearing())

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

    return player.stay()
    #return Transition.getNextState(player, walkToGoal)

#walkToGoal.transitions = { turnForward: Transition.CountTransition(atGoal) }
