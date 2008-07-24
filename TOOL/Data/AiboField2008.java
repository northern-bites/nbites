package TOOL.Data;

import java.awt.image.BufferedImage;
import java.awt.Color;
import java.awt.BasicStroke;
import java.awt.Graphics2D;
import java.awt.Dimension;


/**
 * Provides all the constants necessary to construct an image of the new
 * Aibo field, as specified in the 2008 rules.
 * 
 * 
 * @author Nicholas Dunn 
 */
public class AiboField2008 extends Field {
    
    // Each of the four corner arcs are 40 cm in radius
    public static final double CORNER_ARC_RADIUS = 40.0;
    // The arc is 90 degrees of a circle
    public static final double CORNER_ARC_EXTENT = 90.0;

    // The corner arc to the left from goalie's perspective is yellow
    protected static final Color LEFT_CORNER_ARC_COLOR = Color.YELLOW;
    protected static final Color RIGHT_CORNER_ARC_COLOR = Color.BLUE;

    
    public AiboField2008() {}


    public final double getLabScale() {
        return 0.72;
    }
    
    public final double getGreenPadX() { return 20.0; }
    public final double getGreenPadY() { return 30.0; }
    public final double getFieldWhiteWidth() { return 460.0; }
    public final double getFieldWhiteHeight() { return 690.0; }
    public final double getGoalDepth() { return 45.0; }
    public final double getGoalSideLength() { return 25.0; }
    public final double getGoalWidth() { return 90.0; }
    public final double getGoalBoxWidth() { return 130.0; }
    public final double getGoalBoxHeight() { return 45.0; }
    public final double getCCRadius() { return 18.0; }
    public final double getBeaconRadius() { return 5.0; }
    public final double getGoalPostRadius() { return 5.0; }
    public final double getBallRadius() { return 4.0; }
    public final double getLineThickness() { return 5.0; }
    public final double getGoalCrossbarThickness() { return 2.5; }
    public final double getPostToFieldOffset() { return 10.0; }


    /** 
     * Draws all the landmarks on the given graphics context; in our case
     * this includes both beacons (inherited from superclass) and corner
     * arcs (which we must specify ourselves).
     * @param g2 the graphics context on which to draw
     */
    protected void drawLandmarks(Graphics2D g2) {
        // Draw the beacons
        super.drawLandmarks(g2);
        // Draw in the extra arcs
        drawCornerArcs(g2);
    }

    /**
     * Draws the arcs at the four corners of the field, with the yellow arc
     * always being to the goalie's left and the cyan arc being to the goalie's
     * right.
     * @param g2 the graphics context on which to draw
     * @see java.awt.Graphics#drawArc(int, int, int, int, int, int)
     */
    protected void drawCornerArcs(Graphics2D g2) { 
        // Arcs are somewhat tricky.  You define a bounding box for the whole
        // ellipse that would be drawn, NOT a bounding box around the little
        // segment you are drawing.  So if I want a quarter circle, I can't
        // define the bounding box to cover just that segment of the circle, but
        // rather the bounding box must cover the whole circle, and then just
        // draw a piece of the circle.


        // Lower left arc
        g2.setColor(LEFT_CORNER_ARC_COLOR);
        g2.setStroke(new BasicStroke((float)LINE_THICKNESS));

        g2.drawArc(fixXCoord(GREEN_PAD_X + LINE_THICKNESS - CORNER_ARC_RADIUS),
                   fixYCoord(GREEN_PAD_Y + LINE_THICKNESS + CORNER_ARC_RADIUS),
                   (int) (2 * CORNER_ARC_RADIUS), (int)(2 * CORNER_ARC_RADIUS),
                   0, (int) CORNER_ARC_EXTENT); 

        // Upper right
        // it's to the left of the top goal, so it's LEFT_CORNER_ARC_COLOR
        g2.setColor(LEFT_CORNER_ARC_COLOR);
        g2.drawArc(fixXCoord(FIELD_WIDTH - GREEN_PAD_X - LINE_THICKNESS -
                             CORNER_ARC_RADIUS),
                   fixYCoord(FIELD_HEIGHT - GREEN_PAD_Y - LINE_THICKNESS +
                             CORNER_ARC_RADIUS),
                    (int) (2 * CORNER_ARC_RADIUS), (int)(2 * CORNER_ARC_RADIUS),
                   -180, (int) CORNER_ARC_EXTENT);


        // Lower right
        g2.setColor(RIGHT_CORNER_ARC_COLOR);
        g2.drawArc(fixXCoord(FIELD_WIDTH - GREEN_PAD_X - LINE_THICKNESS -
                             CORNER_ARC_RADIUS),
                   fixYCoord(GREEN_PAD_Y + LINE_THICKNESS + CORNER_ARC_RADIUS),
                   (int) (2 * CORNER_ARC_RADIUS), (int)(2 * CORNER_ARC_RADIUS),
                   90, (int) CORNER_ARC_EXTENT);

       
        // Upper left
        // it's to the right of the top goal, so it's RIGHT_CORNER_ARC_COLOR
        g2.drawArc(fixXCoord(GREEN_PAD_X + LINE_THICKNESS - CORNER_ARC_RADIUS),
                   fixYCoord(FIELD_HEIGHT - GREEN_PAD_Y - LINE_THICKNESS +
                             CORNER_ARC_RADIUS),
                   (int) (2 * CORNER_ARC_RADIUS), (int)(2 * CORNER_ARC_RADIUS),
                   -90, (int) CORNER_ARC_EXTENT); 
        
    }
}