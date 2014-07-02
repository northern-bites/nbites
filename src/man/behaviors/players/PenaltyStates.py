"""
@Summer 2013: This entire state is an aid for localization, since we can only
hard reset to one of the two possible post-penalty positions.
"""

import ChaseBallTransitions as transitions
import RoleConstants as roleConstants
from math import copysign, fabs
from objects import RelRobotLocation
from ..util import *

DEBUG_PENALTY_STATES = False
OBJ_SEEN_THRESH = 6

@superState('gameControllerResponder')
def afterPenalty(player):

    if player.firstFrame():

        if DEBUG_PENALTY_STATES:
            print "Entering the 'afterPenalty' state."

        # pan for the ball
        player.brain.tracker.repeatWidePan()
        # reset state specific counters
        player.corner_l_sightings = 0
        player.goal_t_sightings = 0
        player.center_sightings = 0
        player.post_l_sightings = 0
        player.post_r_sightings = 0
        player.goal_right = 0
        player.reset_loc = 0

    vision = player.brain.interface.visionField
    # Do we see a corner?
    for i in range(0, vision.visual_corner_size()):
        corner = vision.visual_corner(i)
        for j in range(0, corner.poss_id_size()):
            poss_id = corner.poss_id(j)
            if (poss_id == corner.corner_id.L_INNER_CORNER or
                poss_id == corner.corner_id.L_OUTER_CORNER):
                # Saw an L-corner!
                if DEBUG_PENALTY_STATES:
                    print "Saw an L-corner!"
                player.corner_l_sightings += copysign(1, corner.visual_detection.bearing)
            if (poss_id == corner.corner_id.BLUE_GOAL_T or
                poss_id == corner.corner_id.YELLOW_GOAL_T):
                # Saw a goal T-corner!
                if DEBUG_PENALTY_STATES:
                    print "Saw a goal T-corner!"
                player.goal_t_sightings += copysign(1, corner.visual_detection.bearing)
            if (poss_id == corner.corner_id.CENTER_CIRCLE or
                poss_id == corner.corner_id.CENTER_T):
                # Saw a center corner (+ or T)
                if DEBUG_PENALTY_STATES:
                    print "Saw a center corner!"
                player.center_sightings += copysign(1, corner.visual_detection.bearing)

    # Do we see a goalpost?
    if vision.goal_post_l.visual_detection.on:
        # Saw a goalpost! (adjust for which goalpost)
        if DEBUG_PENALTY_STATES:
            print "Saw an l-post!"
        if not vision.goal_post_l.visual_detection.bearing == 0:
            player.post_l_sightings += (copysign(1, vision.goal_post_l.visual_detection.bearing) *
                                    copysign(1, 700 - vision.goal_post_l.visual_detection.distance))
    if vision.goal_post_r.visual_detection.on:
        # Saw a goalpost! (adjust for which goalpost)
        if DEBUG_PENALTY_STATES:
            print "Saw an r-post!"
        if not vision.goal_post_r.visual_detection.bearing == 0:
            player.post_r_sightings += (copysign(1, vision.goal_post_r.visual_detection.bearing) *
                                    copysign(1, 700 - vision.goal_post_r.visual_detection.distance))

    # If we've seen any landmark enough, reset localization.
    if fabs(player.corner_l_sightings) > OBJ_SEEN_THRESH:
        if DEBUG_PENALTY_STATES:
            print "Saw enough l-corners!"
        player.reset_loc = copysign(1, player.corner_l_sightings)

    if fabs(player.goal_t_sightings) > OBJ_SEEN_THRESH:
        if DEBUG_PENALTY_STATES:
            print "Saw enough goal t-corners!"
        player.reset_loc = copysign(1, player.goal_t_sightings)

    if fabs(player.center_sightings) > OBJ_SEEN_THRESH:
        if DEBUG_PENALTY_STATES:
            print "Saw enough center corners!"
        player.reset_loc = copysign(1, player.center_sightings) * -1

    if fabs(player.post_l_sightings) > OBJ_SEEN_THRESH:
        if DEBUG_PENALTY_STATES:
            print "Saw enough l-posts!"
        player.reset_loc = copysign(1, player.post_l_sightings)

    if fabs(player.post_r_sightings) > OBJ_SEEN_THRESH:
        if DEBUG_PENALTY_STATES:
            print "Saw enough r-posts!"
        player.reset_loc = copysign(1, player.post_r_sightings)

    # Send the reset loc command.
    if player.reset_loc != 0:
        if DEBUG_PENALTY_STATES:
            print "Sufficient sightings. reset_loc value is: " + str(player.reset_loc)
        player.goal_right += player.reset_loc
        player.corner_l_sightings = 0
        player.goal_t_sightings = 0
        player.center_sightings = 0
        player.post_l_sightings = 0
        player.post_r_sightings = 0
        player.reset_loc = 0

    if fabs(player.goal_right) > 5:
        if DEBUG_PENALTY_STATES:
            print "Consensus reached! Resetting loc. Is the goal to our right? " + str(player.goal_right < 0)
        # Yes, when goal_right is less than 0, our goal is to our right.
        # It seems counter intuitive, but that's how it works. -Josh Z
        player.brain.resetLocalizationFromPenalty(player.goal_right < 0)
        if not roleConstants.isGoalie(player.role):
            return player.goNow('determineRole')
        return player.goLater(player.gameState)

    return player.stay()

@superState('gameControllerResponder')
def determineRole(player):
    if not player.roleSwitching:
        return player.goLater(player.gameState)

    openSpaces = [True, True, True, True]
    for mate in player.brain.teamMembers:
        if not roleConstants.isGoalie(mate.role) and mate.active:
            openSpaces[mate.role - 2] = False

    if openSpaces[3]:
        RoleConstants.setRoleConstants(player, 5)
    if openSpaces[2]:
        RoleConstants.setRoleConstants(player, 4)
    elif openSpaces[1]:
        RoleConstants.setRoleConstants(player, 3)
    elif openSpaces[0]:
        RoleConstants.setRoleConstants(player, 2)

    return player.goLater(player.gameState)
