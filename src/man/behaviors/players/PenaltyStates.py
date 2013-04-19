import ChaseBallTransitions as transitions
from math import copysign, fabs

OBJ_SEEN_THRESH = 6

# @Summer 2012: This entire state appears to be a hack for localization.
# Consider removing entirely.
def afterPenalty(player):

    if player.firstFrame():
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
                player.corner_l_sightings += copysign(1, corner.visual_detection.bearing)
            if (poss_id == corner.corner_id.BLUE_GOAL_T or
                poss_id == corner.corner_id.YELLOW_GOAL_T):
                # Saw a goal T-corner!
                player.goal_t_sightings += copysign(1, corner.visual_detection.bearing)
            if (poss_id == corner.corner_id.CENTER_CIRCLE or
                poss_id == corner.corner_id.CENTER_T):
                # Saw a center corner (+ or T)
                player.center_sightings += copysign(1, corner.visual_detection.bearing)

    # Do we see a goalpost?
    if vision.goal_post_l.visual_detection.on:
        # Saw a goalpost! (adjust for which goalpost)
        if not vision.goal_post_l.visual_detection.bearing == 0:
            player.post_l_sightings += (copysign(1, vision.goal_post_l.visual_detection.bearing) *
                                    copysign(1, 700 - vision.goal_post_l.visual_detection.distance))
    if vision.goal_post_r.visual_detection.on:
        # Saw a goalpost! (adjust for which goalpost)
        if not vision.goal_post_r.visual_detection.bearing == 0:
            player.post_r_sightings += (copysign(1, vision.goal_post_r.visual_detection.bearing) *
                                    copysign(1, 700 - vision.goal_post_r.visual_detection.distance))

    # If we've seen any landmark enough, reset localization.
    if fabs(player.corner_l_sightings) > OBJ_SEEN_THRESH:
        player.reset_loc = copysign(1, player.corner_l_sightings)
    if fabs(player.goal_t_sightings) > OBJ_SEEN_THRESH:
        player.reset_loc = copysign(1, player.goal_t_sightings)
    if fabs(player.center_sightings) > OBJ_SEEN_THRESH:
        player.reset_loc = copysign(1, player.center_sightings) * -1
    if fabs(player.post_l_sightings) > OBJ_SEEN_THRESH:
        player.reset_loc = copysign(1, player.post_l_sightings)
    if fabs(player.post_r_sightings) > OBJ_SEEN_THRESH:
        player.reset_loc = copysign(1, player.post_r_sightings)

    # Send the reset loc command.
    if player.reset_loc != 0:
        player.goal_right += player.reset_loc
        player.corner_l_sightings = 0
        player.goal_t_sightings = 0
        player.center_sightings = 0
        player.post_l_sightings = 0
        player.post_r_sightings = 0
        player.reset_loc = 0

    if fabs(player.goal_right) > 5:
        player.brain.resetLocalizationFromPenalty(player.goal_right < 0)
        return player.goNow(player.gameState)


    return player.stay()

def penaltyRelocalize(player):
    """
    Note: This is the old code that I'm using as a back-up in case we can't
    see any goal posts. It may be possible to make this smarter. -Wils
    """
    if player.firstFrame():
        player.setWalk(1, 0, 0)

    if player.brain.ball.vis.frames_on >= OBJ_SEEN_THRESH:
        player.brain.tracker.trackBall()
        return player.goLater(player.gameState)

    if player.brain.my.locScore != NogginConstants.locScore.BAD_LOC:
        player.shouldRelocalizeCounter += 1

        if player.shouldRelocalizeCounter > 30:
            player.shouldRelocalizeCounter = 0
            return player.goLater(player.gameState)

    else:
        player.shouldRelocalizeCounter = 0

    if not player.brain.motion.head_is_active:
        player.brain.tracker.repeatWidePan()

    return player.stay()
