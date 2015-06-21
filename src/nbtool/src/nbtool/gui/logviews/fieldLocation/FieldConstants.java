package nbtool.gui.logviews.fieldLocation;

public class FieldConstants
{
    public final static float FIELD_WHITE_WIDTH = 900.f;
    public final static float FIELD_WHITE_HEIGHT = 600.f;
    public final static float GREEN_PAD_X = 69.5f;
    public final static float GREEN_PAD_Y = 69.5f;

    public final static float LINE_WIDTH = 5.0f;

    public final static float FIELD_GREEN_WIDTH = FIELD_WHITE_WIDTH + 2.0f * GREEN_PAD_X;
    public final static float FIELD_GREEN_HEIGHT = FIELD_WHITE_HEIGHT + 2.0f * GREEN_PAD_Y;
    public final static float FIELD_WIDTH = FIELD_GREEN_WIDTH;
    public final static float FIELD_HEIGHT = FIELD_GREEN_HEIGHT;
        
    public final static float CENTER_FIELD_X = FIELD_GREEN_WIDTH * .5f;
    public final static float CENTER_FIELD_Y = FIELD_GREEN_HEIGHT * .5f;
    
    public final float FIELD_GREEN_LEFT_SIDELINE_X = 0;
    public final float FIELD_GREEN_RIGHT_SIDELINE_X = FIELD_GREEN_WIDTH;
    public final float FIELD_GREEN_BOTTOM_SIDELINE_Y = 0;
    public final float FIELD_GREEN_TOP_SIDELINE_Y = FIELD_GREEN_HEIGHT;
    
    public final static float FIELD_WHITE_BOTTOM_SIDELINE_Y = GREEN_PAD_Y;
    public final static float FIELD_WHITE_TOP_SIDELINE_Y = (FIELD_WHITE_HEIGHT +
                                                      GREEN_PAD_Y);
    public final static float FIELD_WHITE_LEFT_SIDELINE_X = GREEN_PAD_X;
    public final static float FIELD_WHITE_RIGHT_SIDELINE_X = (FIELD_WHITE_WIDTH +
                                                        GREEN_PAD_X);
    
    public final static float MIDFIELD_X = FIELD_GREEN_WIDTH * .5f;
    public final static float MIDFIELD_Y = FIELD_GREEN_HEIGHT * .5f;
        
    // Other Field object dimensions
    public final float GOAL_POST_CM_HEIGHT = 90.0f; // 80cm to the bottom
                                                        // of the crossbar.
    public final float GOAL_POST_CM_WIDTH = 10.0f;
    public final static float CROSSBAR_CM_WIDTH = 150.f;
    public final float CROSSBAR_CM_HEIGHT = 10.0f;
    public final float GOAL_DEPTH = 50.0f;
    public final float GOAL_POST_RADIUS = GOAL_POST_CM_WIDTH / 2.0f;
    
    //Penalty Marks
    public final static int PENALTY_MARK_WIDTH = 5;
    public final static int PENALTY_MARK_HEIGHT = 15;
    public final static int PENALTY_MARK_DISTANCE_FROM_WHITE_LINE = 130;
    
    //left penalty mark X
    public final static int LEFT_PENALTY_MARK_X = (int)GREEN_PAD_X+PENALTY_MARK_DISTANCE_FROM_WHITE_LINE;
    //right penalty mark x
    public final static int RIGHT_PENALTY_MARK_X = (int)GREEN_PAD_X+(int)FIELD_WHITE_WIDTH-PENALTY_MARK_DISTANCE_FROM_WHITE_LINE;
    
    
    // GOAL CONSTANTS
    public final float LANDMARK_MY_GOAL_BOTTOM_POST_X =
        FIELD_WHITE_LEFT_SIDELINE_X + GOAL_POST_RADIUS;
    public final float LANDMARK_MY_GOAL_TOP_POST_X =
        FIELD_WHITE_LEFT_SIDELINE_X + GOAL_POST_RADIUS;
    public final float LANDMARK_OPP_GOAL_BOTTOM_POST_X =
        FIELD_WHITE_RIGHT_SIDELINE_X - GOAL_POST_RADIUS;
    public final float LANDMARK_OPP_GOAL_TOP_POST_X =
    FIELD_WHITE_RIGHT_SIDELINE_X - GOAL_POST_RADIUS;

    public final static float LANDMARK_MY_GOAL_BOTTOM_POST_Y =
        CENTER_FIELD_Y - CROSSBAR_CM_WIDTH / 2.0f;
    public final float LANDMARK_MY_GOAL_TOP_POST_Y =
        CENTER_FIELD_Y + CROSSBAR_CM_WIDTH / 2.0f;
    public final float LANDMARK_OPP_GOAL_BOTTOM_POST_Y =
        CENTER_FIELD_Y - CROSSBAR_CM_WIDTH / 2.0f;
    public final float LANDMARK_OPP_GOAL_TOP_POST_Y =
        CENTER_FIELD_Y + CROSSBAR_CM_WIDTH / 2.0f;
    
    public final static float CENTER_CIRCLE_RADIUS = 150.0f; // Not scaled

    public final static float GOALBOX_DEPTH = 60.0f;
    public final static float GOALBOX_WIDTH = 220.0f;


    // These are used by the vision system
    // The distance the goalie box extends out past each goal post
    public final float GOALBOX_OVERAGE = (GOALBOX_WIDTH -
                                          (CROSSBAR_CM_WIDTH +
                                           2.0f * GOAL_POST_RADIUS)) / 2.0f;
    // The distance from any goal post to the goalie box corner nearest it
    public final float POST_CORNER = (float)Math.sqrt(GOALBOX_DEPTH * GOALBOX_DEPTH +
                                           GOALBOX_OVERAGE * GOALBOX_OVERAGE);
    
    // Headings to goal from center
    public final float OPP_GOAL_HEADING = 0.0f;
    public final float MY_GOAL_HEADING = 180.0f;

    // my goal box constants relative to (0,0) on my team
    public final float MY_GOALBOX_TOP_Y = MIDFIELD_Y + GOALBOX_WIDTH * .5f;
    public final float MY_GOALBOX_BOTTOM_Y = MIDFIELD_Y - GOALBOX_WIDTH * .5f;
    // bottom as in closest to (0,0)
    public final float MY_GOALBOX_LEFT_X = GREEN_PAD_X;
    public final float MY_GOALBOX_RIGHT_X = GREEN_PAD_X + GOALBOX_DEPTH;
    
    // opp goal box constants relative to (0,0) on my team
    public final float OPP_GOALBOX_BOTTOM_Y = MIDFIELD_Y - GOALBOX_WIDTH * .5f;
    public final float OPP_GOALBOX_TOP_Y = MIDFIELD_Y + GOALBOX_WIDTH * .5f;
    public final float OPP_GOALBOX_LEFT_X =
    FIELD_WHITE_RIGHT_SIDELINE_X - GOALBOX_DEPTH;
    public final float OPP_GOALBOX_RIGHT_X = FIELD_WHITE_RIGHT_SIDELINE_X;

    public final float LINE_CROSS_OFFSET = 130.0f;
        
    public final float LANDMARK_MY_GOAL_CROSS_X = FIELD_WHITE_LEFT_SIDELINE_X +
        LINE_CROSS_OFFSET;
    public final float LANDMARK_MY_GOAL_CROSS_Y = MIDFIELD_Y;
    public final float LANDMARK_OPP_GOAL_CROSS_X = FIELD_WHITE_RIGHT_SIDELINE_X -
        LINE_CROSS_OFFSET;
    public final float LANDMARK_OPP_GOAL_CROSS_Y = MIDFIELD_Y;

    public final float CC_LINE_CROSS_X = MIDFIELD_X;
    public final float CC_LINE_CROSS_Y = MIDFIELD_Y;

    public final float TOP_CC_Y = CENTER_FIELD_Y + CENTER_CIRCLE_RADIUS;
    public final float TOP_CC_X = CENTER_FIELD_X;
    public final float BOTTOM_CC_Y = CENTER_FIELD_Y - CENTER_CIRCLE_RADIUS;
    public final float BOTTOM_CC_X = CENTER_FIELD_X;

    public final float LINE_CROSS_LENGTH = 10.0f; // length of each cross in cm

    // Useful constants for initial localization
    public final float MY_CC_NEAREST_POINT_X = CENTER_FIELD_X -
    CENTER_CIRCLE_RADIUS;
    public final float MY_CROSS_CIRCLE_MIDPOINT_X = LANDMARK_MY_GOAL_CROSS_X +
        ((MY_CC_NEAREST_POINT_X-LANDMARK_MY_GOAL_CROSS_X)/2.0f);
    public final float MY_GOALBOX_MIDPOINT_X = FIELD_WHITE_LEFT_SIDELINE_X +
        (GOALBOX_DEPTH / 2);
    public final float MY_GOALBOX_CROSS_MIDPOINT_X = FIELD_WHITE_LEFT_SIDELINE_X +
        GOALBOX_DEPTH + ((LINE_CROSS_OFFSET - GOALBOX_DEPTH) / 2);
    
    // Constants for heading
    //  Right is towards opponents' goal.
    //  Left is towards own goal.
    public final float HEADING_RIGHT = 0.0f;
    public final float HEADING_UP = 90.0f;
    public final float HEADING_LEFT = 180.0f;
    public final float HEADING_DOWN = -90.0f;

    // kick off positions
    /*
    public final Location ODD_DEFENDER_KICKOFF = new Location(MY_GOALBOX_RIGHT_X + 50,
                                                        MY_GOALBOX_BOTTOM_Y); 
    public final Location EVEN_DEFENDER_KICKOFF = new Location(MY_GOALBOX_RIGHT_X + 100,
                                                    MY_GOALBOX_TOP_Y);
    public final Location ODD_CHASER_KICKOFF = new Location(CENTER_FIELD_X - 45,
                                                OPP_GOALBOX_BOTTOM_Y - 100);
    public final Location EVEN_CHASER_KICKOFF = new Location(CENTER_FIELD_X - 45,
                                                CENTER_FIELD_Y);
    */
}