package edu.bowdoin.robocup.TOOL.Data;

import java.awt.image.BufferedImage;
import java.awt.Color;
import java.awt.BasicStroke;
import java.awt.Graphics2D;
import java.awt.Dimension;


/**
 * Provides all the constants necessary to construct an image of the new
 * Nao field, as specified in the 2008 rules.
 *
 * Dimensions taken from
 * http://www.tzi.de/4legged/pub/Website/Downloads/NaoRules2008.pdf
 *
 * @author Nicholas Dunn
 */
public class NaoField2008 extends Field {

    public NaoField2008() {}

    // 486 / 600 = .81
    public final double getLabScale() { return .81; }

    public final double getGreenPadX() { return 20.0; }
    public final double getGreenPadY() { return 40.0; }
    public final double getFieldWhiteWidth() { return 400.0; }
    public final double getFieldWhiteHeight() { return 600.0; }
    // Side length = 40, goal post width = 10
    public final double getGoalDepth() { return 50.0; }
    public final double getGoalSideLength() { return 40.0; }

    // Note: These goal width and goal box width values look wrong, as they do
    // not correspond with the pictures we have been provided of the field.
    // However, they are based off of the dimensions of the goal provided in
    // the NaoRules2008

    // The width between the two posts
    public final double getGoalWidth() { return 140.0; }
    public final double getGoalBoxWidth() { return 200.0; }
    public final double getGoalBoxHeight() { return 60.0; }
    public final double getCCRadius() { return 65.0; }
    // There are no beacons
    public final double getBeaconRadius() { return 0.0; }
    public final double getGoalPostRadius() { return 5.0; }
    public final double getBallRadius() { return 4.0; }
    public final double getLineThickness() { return 5.0; }
    public final double getGoalCrossbarThickness() { return 2.5; }
    // Used for beacons, again we have none
    public final double getPostToFieldOffset() { return 0.0; }



    protected static final Color GOAL_SIDE_COLOR = Color.WHITE;
    protected static final Color NET_COLOR = Color.WHITE;

    protected static final double NET_DRAW_WIDTH = 1.0;
    // Size of the holes of the net, in cm (I think we have 2 inch netting)
    protected static final double MESH_SIZE = 2 * 2.54;


    /** Since we have no beacons to draw, just exit */
    protected void drawLandmarks(Graphics2D g2) {

    }

    /**
     * Draws the top goal on the given graphics context.
     * @param g2 the graphics context on which to draw
     */
    protected void drawTopGoal(Graphics2D g2) {
        drawNet(g2, NET_COLOR, NET_DRAW_WIDTH,
                LANDMARK_TOP_GOAL_RIGHT_POST_X,
                LANDMARK_TOP_GOAL_LEFT_POST_Y + GOAL_SIDE_LENGTH,
                LANDMARK_TOP_GOAL_LEFT_POST_X,
                LANDMARK_TOP_GOAL_RIGHT_POST_Y);

        // draw left side of goal
	drawLine(g2, GOAL_SIDE_COLOR, DEFAULT_THICKNESS,
                 LANDMARK_TOP_GOAL_LEFT_POST_X,
                 LANDMARK_TOP_GOAL_LEFT_POST_Y,
                 LANDMARK_TOP_GOAL_LEFT_POST_X,
                 LANDMARK_TOP_GOAL_LEFT_POST_Y+GOAL_SIDE_LENGTH);
	// draw right side of goal
	drawLine(g2, GOAL_SIDE_COLOR, DEFAULT_THICKNESS,
                 LANDMARK_TOP_GOAL_RIGHT_POST_X,
                 LANDMARK_TOP_GOAL_RIGHT_POST_Y,
                 LANDMARK_TOP_GOAL_RIGHT_POST_X,
                 LANDMARK_TOP_GOAL_LEFT_POST_Y+GOAL_SIDE_LENGTH);
        // draw top goal left post
	fillOval(g2, TOP_GOAL_COLOR, DEFAULT_THICKNESS,
                 LANDMARK_TOP_GOAL_LEFT_POST_X,
                 LANDMARK_TOP_GOAL_LEFT_POST_Y,
                 GOAL_POST_RADIUS, GOAL_POST_RADIUS);
	// draw top goal right post
	fillOval(g2, TOP_GOAL_COLOR, DEFAULT_THICKNESS,
                 LANDMARK_TOP_GOAL_RIGHT_POST_X,
                 LANDMARK_TOP_GOAL_RIGHT_POST_Y,
                 GOAL_POST_RADIUS, GOAL_POST_RADIUS);

        // draw crossbar
	drawLine(g2, TOP_GOAL_COLOR, GOAL_CROSSBAR_THICKNESS,
                 LANDMARK_TOP_GOAL_LEFT_POST_X,
                 LANDMARK_TOP_GOAL_LEFT_POST_Y,
                 LANDMARK_TOP_GOAL_RIGHT_POST_X,
                 LANDMARK_TOP_GOAL_RIGHT_POST_Y);



    }

     /**
     * Draws the bottom goal on the given graphics context.
     * @param g2 the graphics context on which to draw
     */
    protected void drawBottomGoal(Graphics2D g2) {
        // Draw the net
        drawNet(g2, NET_COLOR, NET_DRAW_WIDTH,
                LANDMARK_BOTTOM_GOAL_LEFT_POST_X,
                LANDMARK_BOTTOM_GOAL_LEFT_POST_Y,
                LANDMARK_BOTTOM_GOAL_RIGHT_POST_X,
                LANDMARK_BOTTOM_GOAL_RIGHT_POST_Y - GOAL_SIDE_LENGTH);

        // draw left side of goal
	drawLine(g2, GOAL_SIDE_COLOR, DEFAULT_THICKNESS,
                 LANDMARK_BOTTOM_GOAL_LEFT_POST_X,
                 LANDMARK_BOTTOM_GOAL_LEFT_POST_Y,
                 LANDMARK_BOTTOM_GOAL_LEFT_POST_X,
                 LANDMARK_BOTTOM_GOAL_LEFT_POST_Y-GOAL_SIDE_LENGTH);
	// draw right side of goal
	drawLine(g2, GOAL_SIDE_COLOR, DEFAULT_THICKNESS,
                 LANDMARK_BOTTOM_GOAL_RIGHT_POST_X,
                 LANDMARK_BOTTOM_GOAL_RIGHT_POST_Y,
                 LANDMARK_BOTTOM_GOAL_RIGHT_POST_X,
                 LANDMARK_BOTTOM_GOAL_RIGHT_POST_Y-GOAL_SIDE_LENGTH);

        // draw bottom goal left post
	fillOval(g2, BOTTOM_GOAL_COLOR, DEFAULT_THICKNESS,
                 LANDMARK_BOTTOM_GOAL_LEFT_POST_X,
                 LANDMARK_BOTTOM_GOAL_LEFT_POST_Y,
                 GOAL_POST_RADIUS, GOAL_POST_RADIUS);
	// draw bottom goal right post
	fillOval(g2, BOTTOM_GOAL_COLOR, DEFAULT_THICKNESS,
                 LANDMARK_BOTTOM_GOAL_RIGHT_POST_X,
                 LANDMARK_BOTTOM_GOAL_RIGHT_POST_Y,
                 GOAL_POST_RADIUS, GOAL_POST_RADIUS);

	// draw crossbar
	drawLine(g2, BOTTOM_GOAL_COLOR, GOAL_CROSSBAR_THICKNESS,
                 LANDMARK_BOTTOM_GOAL_LEFT_POST_X,
                 LANDMARK_BOTTOM_GOAL_LEFT_POST_Y,
                 LANDMARK_BOTTOM_GOAL_RIGHT_POST_X,
                 LANDMARK_BOTTOM_GOAL_RIGHT_POST_Y);


    }




    protected void drawNet(Graphics2D g2, Color col,double strokeWidth,
                           double x1, double y1, double x2, double y2) {
        g2.setColor(col);
        g2.setStroke(new BasicStroke((float) strokeWidth));

        int numHorRects = (int) ((x2 - x1) / MESH_SIZE);
        int numVertRects = (int) ((y1 - y2) / MESH_SIZE);

        for (int i = 0; i < numHorRects; i++) {
            for (int j = 0; j < numVertRects; j++) {
                g2.drawRect((int) (x1 + (i * MESH_SIZE)),
                            (int) (fixYCoord(y1) + (j * MESH_SIZE)),
                            (int) MESH_SIZE, (int) MESH_SIZE);
            }
        }


        }


}