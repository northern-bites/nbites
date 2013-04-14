import noggin_constants as NogginConstants
from objects import Location

APPROACH_BALL_HACK = 2

DEFAULT_KICK_X_OFFSET = 13

#HACK HACK HACK for HackKickInformation
KICK_STRAIGHT_POST_BEARING = 17
KICK_STRAIGHT_BEARING_THRESH = 70
KICK_SIDE_DIST_THRESH = .75*NogginConstants.GOAL_WIDTH

# TODO make these accurate. TODO make ranges specific to kicks
SHORT_RANGE_KICK_DIST = 120
LONG_RANGE_KICK_DIST = 230

SHOOT_FRACTION = 0.145 # how far in along goal width should we shoot?
SHOOT_Y_OFFSET = NogginConstants.GOAL_WIDTH*SHOOT_FRACTION
SIDELINE_KICKOFF_OFFSET = 80.

LEFT_CLEAR_POINT = Location(NogginConstants.OPP_GOALBOX_LEFT_X,
                            NogginConstants.OPP_GOALBOX_TOP_Y)

RIGHT_CLEAR_POINT = Location(NogginConstants.OPP_GOALBOX_LEFT_X,
                             NogginConstants.OPP_GOALBOX_BOTTOM_Y)

SHOOT_LEFT_AIM_POINT = Location(NogginConstants.LANDMARK_OPP_GOAL_LEFT_POST_X,
                                   NogginConstants.LANDMARK_OPP_GOAL_LEFT_POST_Y -
                                   SHOOT_Y_OFFSET)

SHOOT_RIGHT_AIM_POINT = Location(NogginConstants.LANDMARK_OPP_GOAL_LEFT_POST_X,
                                    NogginConstants.LANDMARK_OPP_GOAL_RIGHT_POST_Y +
                                    SHOOT_Y_OFFSET)

SHOOT_CENTER_AIM_POINT = Location(NogginConstants.LANDMARK_OPP_GOAL_LEFT_POST_X,
                                  NogginConstants.CENTER_FIELD_Y)

CENTER_BALL_POINT = Location(NogginConstants.OPP_GOALBOX_LEFT_X,
                             NogginConstants.CENTER_FIELD_Y)

LEFT_KICKOFF_POINT = Location(NogginConstants.CENTER_FIELD_X +
                              NogginConstants.CENTER_CIRCLE_RADIUS,
                              NogginConstants.FIELD_WHITE_TOP_SIDELINE_Y -
                              SIDELINE_KICKOFF_OFFSET)

RIGHT_KICKOFF_POINT = Location(NogginConstants.CENTER_FIELD_X +
                              NogginConstants.CENTER_CIRCLE_RADIUS,
                              NogginConstants.FIELD_WHITE_BOTTOM_SIDELINE_Y +
                              SIDELINE_KICKOFF_OFFSET)

FORWARD_KICKOFF_POINT = Location(NogginConstants.CENTER_FIELD_X +
                                 NogginConstants.CENTER_CIRCLE_RADIUS + 15.,
                                 NogginConstants.CENTER_FIELD_Y)

#************************#
# Constants & Thresholds #
#************************#
BALL_NEAR_LINE_THRESH = 25.

# Used in bestAlignedKickDirection. Right now we just want to get to the ball
# and kick it. Could use these to give preference to a straight kick. TODO
STRAIGHT_KICK_ALIGNMENT_BEARING = 45.
BACK_KICK_ALIGNMENT_BEARING = 135.
