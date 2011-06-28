from .. import NogginConstants
from ..typeDefs.Location import Location

DEBUG_KICKS = False

ALIGN_FOR_KICK_BEARING_THRESH = 60

DEFAULT_KICK_X_OFFSET = 13

# clearBall constants
CLEAR_CENTER_FIELD_STRAIGHT_ANGLE = 60

# Kick objectives
NUM_OBJECTIVES = 6
(OBJECTIVE_CLEAR,
 OBJECTIVE_CENTER,
 OBJECTIVE_SHOOT,
 OBJECTIVE_KICKOFF,
 OBJECTIVE_ORBIT,
 OBJECTIVE_UNCLEAR) = range(NUM_OBJECTIVES)

SHOOT_FRACTION = 0.125 # how far in along goal width should we shoot?
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

SHOOT_BALL_SIDE_KICK_ANGLE = 45
SHOOT_BALL_FAR_LOC_ALIGN_ANGLE = 15
SHOOT_BALL_FAR_SIDE_KICK_ANGLE = 45

KICK_STRAIGHT_POST_BEARING = 10
KICK_STRAIGHT_BEARING_THRESH = 70
KICK_SIDE_DIST_THRESH = NogginConstants.GOAL_WIDTH

#Constants to help decide back kick
CLEAR_POST_DIST_DIFF = .2*NogginConstants.GOAL_WIDTH

BALL_NEAR_LINE_THRESH = 25.
