package TOOL.Data;

import java.awt.*;
import java.awt.image.BufferedImage;

/**
 * This class holds all the constants for a field and provides drawing methods
 * both to create an image of this field as well as to draw on top of
 * that field later on in a meaningful way (e.g. to draw circles around where
 * a robot thinks it is).
 *
 * Note that, as this is an abstract class, you cannot instantiate it: You must
 * subclass this field and fill in the appropriate getter methods that provide
 * this class with the information about the dimensions of the field.  Then you
 * may create an instance of that subclass, inheriting all the code that will
 * be in common with any field (namely the drawing methods and constant names).
 *
 * Note that one can override all of the drawing methods in a subclass to draw
 * the field in any way they wish.  This current setup should not need much
 * modifying for either the Nao or Aibo fields, however, with the exception of
 * the drawLandmarks method.
 *
 * @author Nicholas Dunn (Modified, did not create from scratch)
 * @date   March 17, 2008
 *
 */
public abstract class Field
{
    public double REAL_FIELD_SCALE = 1.0;

    public final double SCALE = REAL_FIELD_SCALE;
    public final double GREEN_PAD_X = getGreenPadX() * SCALE;
    public final double GREEN_PAD_Y = getGreenPadY() * SCALE;
    public final double FIELD_WHITE_WIDTH = getFieldWhiteWidth() * SCALE;
    public final double FIELD_WHITE_HEIGHT = getFieldWhiteHeight() * SCALE;
    public final double FIELD_GREEN_WIDTH = (FIELD_WHITE_WIDTH + 2*GREEN_PAD_X);
    public final double FIELD_GREEN_HEIGHT = (FIELD_WHITE_HEIGHT +
                                              2*GREEN_PAD_Y);
    public final double FIELD_WIDTH = FIELD_GREEN_WIDTH;
    public final double FIELD_HEIGHT = FIELD_GREEN_HEIGHT;
    public final double CENTER_FIELD_X = FIELD_GREEN_WIDTH / 2.0;
    public final double CENTER_FIELD_Y = FIELD_GREEN_HEIGHT / 2.0;

    public final double FIELD_WHITE_BOTTOM_SIDELINE_Y = GREEN_PAD_Y;
    public final double FIELD_WHITE_TOP_SIDELINE_Y =
        FIELD_WHITE_HEIGHT + GREEN_PAD_Y;
    public final double FIELD_WHITE_LEFT_SIDELINE_X = GREEN_PAD_X;
    public final double FIELD_WHITE_RIGHT_SIDELINE_X =
        FIELD_WHITE_WIDTH + GREEN_PAD_X;

    public final double GOAL_DEPTH = getGoalDepth();
    public final double GOAL_SIDE_LENGTH = getGoalSideLength();
    public final double GOAL_WIDTH = getGoalWidth();
    public final double GOAL_BOX_WIDTH = getGoalBoxWidth();
    public final double GOAL_BOX_DEPTH = getGoalBoxDepth();
    public final double CENTER_CIRCLE_RADIUS = getCCRadius();
    public final double GOAL_POST_RADIUS = getGoalPostRadius();
    public final double BALL_RADIUS = getBallRadius();
    public final double LINE_THICKNESS = getLineThickness();
    public final double DEFAULT_THICKNESS = 1.0;
    public final double GOAL_CROSSBAR_THICKNESS =
        getGoalCrossbarThickness();

    // computed field dimensions used by internal procedures and the RoboCup
    // Rules. There is no need to set or to modify these values
    public final double LEFT_SIDELINE_X = GREEN_PAD_X;
    public final double RIGHT_SIDELINE_X =
        GREEN_PAD_X + FIELD_WHITE_WIDTH;
    public final double BOTTOM_SIDELINE_Y = GREEN_PAD_Y;
    public final double TOP_SIDELINE_Y =
        GREEN_PAD_Y + FIELD_WHITE_HEIGHT;
    public final double MIDFIELD_X = FIELD_WIDTH / 2.;
    public final double MIDFIELD_Y = FIELD_HEIGHT / 2.;

    // my left post is left of goalie defending my goal facing the opponent
    public final double LANDMARK_RIGHT_GOAL_TOP_POST_X =
        RIGHT_SIDELINE_X + GOAL_POST_RADIUS;
    public final double LANDMARK_RIGHT_GOAL_BOTTOM_POST_X =
        RIGHT_SIDELINE_X + GOAL_POST_RADIUS;
    public final double LANDMARK_LEFT_GOAL_TOP_POST_X =
        LEFT_SIDELINE_X - GOAL_POST_RADIUS;
    public final double LANDMARK_LEFT_GOAL_BOTTOM_POST_X =
        LEFT_SIDELINE_X - GOAL_POST_RADIUS;

    // measure to the center of the posts, 5 cm off the line
    public final double LANDMARK_RIGHT_GOAL_TOP_POST_Y =
        MIDFIELD_Y + GOAL_WIDTH / 2.0;
    public final double LANDMARK_RIGHT_GOAL_BOTTOM_POST_Y =
        MIDFIELD_Y - GOAL_WIDTH / 2.0;
    public final double LANDMARK_LEFT_GOAL_TOP_POST_Y =
        MIDFIELD_Y + GOAL_WIDTH / 2.0;
    public final double LANDMARK_LEFT_GOAL_BOTTOM_POST_Y =
        MIDFIELD_Y - GOAL_WIDTH / 2.0;

    public final double GOAL_X_LEFT = MIDFIELD_X - (GOAL_WIDTH / 2.);
    public final double GOAL_X_CENTER = MIDFIELD_X;
    public final double GOAL_BOX_TOP_Y = MIDFIELD_Y + (GOAL_BOX_WIDTH / 2.);
    public final double GOAL_BOX_BOTTOM_Y = MIDFIELD_Y - (GOAL_BOX_WIDTH / 2.);
    public final double RIGHT_GOAL_Y = BOTTOM_SIDELINE_Y;
    public final double LEFT_GOAL_Y = TOP_SIDELINE_Y;

    public final double DRAW_STROKE = 1.;
    public final Color FIELD_COLOR = new Color(0,200,0);//Color.GREEN;
    public final Color LINES_COLOR = Color.WHITE;
    public final Color LEFT_GOAL_COLOR = getLeftGoalColor();
    public final Color RIGHT_GOAL_COLOR = getRightGoalColor();

    public final Color REAL_DOG_POSITION_COLOR = Color.RED;

    protected static final Color GOAL_SIDE_COLOR = Color.WHITE;
    protected static final Color NET_COLOR = Color.WHITE;

    protected static final double NET_DRAW_WIDTH = 1.0;
    // Size of the holes of the net, in cm (I think we have 2 inch netting)
    protected static final double MESH_SIZE = 2 * 2.54;

    // Particle Junk
    public final int PARTICLE_LENGTH = 10;
    public final float PARTICLE_STROKE = 4.0f;

    /***********************************************************************
     * Member variable getters.
     * Note that these are abstract, meaning that any subclass must fill in
     * bodies for them (in essence, filling in the dimensions of the field,
     * from which all other variables can be computed)
     *
     **********************************************************************/
    public abstract double getGreenPadX();
    public abstract double getGreenPadY();
    public abstract double getFieldWhiteWidth();
    public abstract double getFieldWhiteHeight();
    public abstract double getGoalDepth();
    public abstract double getGoalSideLength();
    public abstract double getGoalWidth();
    public abstract double getGoalBoxWidth();
    public abstract double getGoalBoxDepth();
    public abstract double getCCRadius();
    public abstract double getGoalPostRadius();
    public abstract double getLineThickness();
    public abstract double getGoalCrossbarThickness();

    // Things that should not change between Nao and Aibo fields
    public double getBallRadius() { return 4.0; }
    public Color getLeftGoalColor() { return Color.BLUE; }
    public Color getRightGoalColor() { return Color.YELLOW; }

    /**
     * Creates a BufferedImage representation of the field through the use
     * of helper drawing methods.
     * @return the image representation of the field
     */
    public BufferedImage getFieldImage()
    {
        BufferedImage field = new BufferedImage((int) FIELD_WIDTH,
                                                (int) FIELD_HEIGHT,
                                                BufferedImage.TYPE_INT_ARGB);
        Graphics2D g2 = field.createGraphics();
        drawField(g2);
        drawFieldLines(g2);
        drawGoals(g2);
        return field;
    }


    // This is precisely the same conversion; note that the two are inverses.
    // FIELD_HEIGHT - (FIELD_HEIGHT - Y) = Y.
    public int fieldToScreenY(int y) { return (int)(FIELD_HEIGHT - y);}
    public float fieldToScreenY(float y) { return (float)FIELD_HEIGHT - y;}
    public int screenToFieldY(int y) { return fieldToScreenY(y); }
    public float screenToFieldY(float y) { return fieldToScreenY(y); }
    // Our coordinate system doesn't change the x values; just return the scaled
    // x
    public int fieldToScreenX(int x) { return (x);}//(int) (SCALE * x); }
    public float fieldToScreenX(float x) { return (x);}//(int) (SCALE * x); }
    public int screenToFieldX(int x) { return fieldToScreenX(x); }
    public float screenToFieldX(float x) { return fieldToScreenX(x); }

    public int fixYCoord(double y) { return screenToFieldY((int)y); }
    public int fixXCoord(double x) { return screenToFieldX((int)x); }
    public int fixWidth(double w) { return (int)w;}//(int) (SCALE * w); }
    public int fixHeight(double h) { return (int)h;}//(int) (SCALE * h); }


    public double getFieldWidth() {return FIELD_WIDTH;}
    public double getFieldHeight() {return FIELD_HEIGHT;}
    public double getFieldLowX() {return LEFT_SIDELINE_X;}
    public double getFieldHighX() {return RIGHT_SIDELINE_X;}
    public double getFieldLowY() {return BOTTOM_SIDELINE_Y;}
    public double getFieldHighY() {return TOP_SIDELINE_Y;}

    public Dimension getPreferredSize() {
        return new Dimension((int) FIELD_WIDTH, (int) FIELD_HEIGHT);
    }

    /**********************************************************************
     * Drawing methods.
     * Please note that these methods CHANGE THE GRAPHICS CONTEXT THEY ARE
     * GIVEN.  Thus you must ensure that you set the color and stroke of
     * the Graphics2D object yourself after calling these methods as the
     * previous values will be overwritten.
     **********************************************************************/

    /**
     * Draws the green part of the field.
     * @param g2 the graphics context on which to draw
     */
    protected void drawField(Graphics2D g2) {
        g2.setColor(FIELD_COLOR);
        g2.fillRect(0,0, (int) FIELD_WIDTH, (int) FIELD_HEIGHT);
    }

    /**
     * Draws the field lines on top of the green of the field.
     * @param g2 the graphics context on which to draw
     */
    protected void drawFieldLines(Graphics2D g2) {
        // Sidelines
        drawRect(g2, LINES_COLOR, LINE_THICKNESS,
                 LEFT_SIDELINE_X, GREEN_PAD_Y,
                 FIELD_WHITE_WIDTH, FIELD_WHITE_HEIGHT);

        // Midline
        drawLine(g2, LINES_COLOR, LINE_THICKNESS,
                 MIDFIELD_X,TOP_SIDELINE_Y, MIDFIELD_X, BOTTOM_SIDELINE_Y);

        // Center circle
        drawOval(g2, LINES_COLOR, LINE_THICKNESS, MIDFIELD_X,
                 MIDFIELD_Y, CENTER_CIRCLE_RADIUS, CENTER_CIRCLE_RADIUS);

        // Left goal box
        drawRect(g2, LINES_COLOR, LINE_THICKNESS,
                 LEFT_SIDELINE_X, GOAL_BOX_BOTTOM_Y,
                 GOAL_BOX_DEPTH, GOAL_BOX_WIDTH);

        // Right goal box
        drawRect(g2, LINES_COLOR, LINE_THICKNESS,
                 RIGHT_SIDELINE_X - GOAL_BOX_DEPTH, GOAL_BOX_BOTTOM_Y,
                 GOAL_BOX_DEPTH, GOAL_BOX_WIDTH);

        // Left side cross
        drawLine(g2, LINES_COLOR, LINE_THICKNESS,
                 LEFT_SIDELINE_X + 180.0f - 5.0f,
                 CENTER_FIELD_Y,
                 LEFT_SIDELINE_X + 180.0f + 5.0f,
                 CENTER_FIELD_Y);
        drawLine(g2, LINES_COLOR, LINE_THICKNESS,
                 LEFT_SIDELINE_X + 180.0f,
                 CENTER_FIELD_Y + 5.0f,
                 LEFT_SIDELINE_X + 180.0f,
                 CENTER_FIELD_Y - 5.0f);
        // Right side cross
        drawLine(g2, LINES_COLOR, LINE_THICKNESS,
                 RIGHT_SIDELINE_X - 180.0f - 5.0f,
                 CENTER_FIELD_Y,
                 RIGHT_SIDELINE_X - 180.0f + 5.0f,
                 CENTER_FIELD_Y);
        drawLine(g2, LINES_COLOR, LINE_THICKNESS,
                 RIGHT_SIDELINE_X - 180.0f,
                 CENTER_FIELD_Y + 5.0f,
                 RIGHT_SIDELINE_X - 180.0f,
                 CENTER_FIELD_Y - 5.0f);
    }

    /**
     * Helper method which delegates the drawing of the two goals to two
     * other methods, drawTopGoal and drawBottomGoal
     * @param g2 the graphics context on which to draw
     */
    protected void drawGoals(Graphics2D g2) {
        drawLeftGoal(g2);
        drawRightGoal(g2);
    }

    /**
     * Draws the top goal on the given graphics context.
     * @param g2 the graphics context on which to draw
     */
    protected void drawLeftGoal(Graphics2D g2) {
        drawNet(g2, NET_COLOR, NET_DRAW_WIDTH,
                LANDMARK_LEFT_GOAL_TOP_POST_X - GOAL_SIDE_LENGTH,
                LANDMARK_LEFT_GOAL_TOP_POST_Y,
                LANDMARK_LEFT_GOAL_BOTTOM_POST_X,
                LANDMARK_LEFT_GOAL_BOTTOM_POST_Y);

        // draw top side of goal
        drawLine(g2, GOAL_SIDE_COLOR, DEFAULT_THICKNESS,
                 LANDMARK_LEFT_GOAL_TOP_POST_X,
                 LANDMARK_LEFT_GOAL_TOP_POST_Y,
                 LANDMARK_LEFT_GOAL_TOP_POST_X - GOAL_SIDE_LENGTH,
                 LANDMARK_LEFT_GOAL_TOP_POST_Y);
        // draw bottom side of goal
        drawLine(g2, GOAL_SIDE_COLOR, DEFAULT_THICKNESS,
                 LANDMARK_LEFT_GOAL_BOTTOM_POST_X,
                 LANDMARK_LEFT_GOAL_BOTTOM_POST_Y,
                 LANDMARK_LEFT_GOAL_BOTTOM_POST_X - GOAL_SIDE_LENGTH,
                 LANDMARK_LEFT_GOAL_BOTTOM_POST_Y);
        // draw left goal top post
        fillOval(g2, LEFT_GOAL_COLOR, DEFAULT_THICKNESS,
                 LANDMARK_LEFT_GOAL_TOP_POST_X,
                 LANDMARK_LEFT_GOAL_TOP_POST_Y,
                 GOAL_POST_RADIUS, GOAL_POST_RADIUS);
        // draw left goal bottom post
        fillOval(g2, LEFT_GOAL_COLOR, DEFAULT_THICKNESS,
                 LANDMARK_LEFT_GOAL_BOTTOM_POST_X,
                 LANDMARK_LEFT_GOAL_BOTTOM_POST_Y,
                 GOAL_POST_RADIUS, GOAL_POST_RADIUS);

        // draw crossbar
        drawLine(g2, LEFT_GOAL_COLOR, GOAL_CROSSBAR_THICKNESS,
                 LANDMARK_LEFT_GOAL_TOP_POST_X,
                 LANDMARK_LEFT_GOAL_TOP_POST_Y,
                 LANDMARK_LEFT_GOAL_BOTTOM_POST_X,
                 LANDMARK_LEFT_GOAL_BOTTOM_POST_Y);
    }

     /**
     * Draws the bottom goal on the given graphics context.
     * @param g2 the graphics context on which to draw
     */
    protected void drawRightGoal(Graphics2D g2) {
        // Draw the net
        drawNet(g2, NET_COLOR, NET_DRAW_WIDTH,
                LANDMARK_RIGHT_GOAL_TOP_POST_X,
                LANDMARK_RIGHT_GOAL_TOP_POST_Y,
                LANDMARK_RIGHT_GOAL_BOTTOM_POST_X + GOAL_SIDE_LENGTH,
                LANDMARK_RIGHT_GOAL_BOTTOM_POST_Y);

        // draw top side of goal
        drawLine(g2, GOAL_SIDE_COLOR, DEFAULT_THICKNESS,
                 LANDMARK_RIGHT_GOAL_TOP_POST_X,
                 LANDMARK_RIGHT_GOAL_TOP_POST_Y,
                 LANDMARK_RIGHT_GOAL_TOP_POST_X + GOAL_SIDE_LENGTH,
                 LANDMARK_RIGHT_GOAL_TOP_POST_Y);
        // draw bottom side of goal
        drawLine(g2, GOAL_SIDE_COLOR, DEFAULT_THICKNESS,
                 LANDMARK_RIGHT_GOAL_BOTTOM_POST_X,
                 LANDMARK_RIGHT_GOAL_BOTTOM_POST_Y,
                 LANDMARK_RIGHT_GOAL_BOTTOM_POST_X + GOAL_SIDE_LENGTH,
                 LANDMARK_RIGHT_GOAL_BOTTOM_POST_Y);

        // draw left goal top post
        fillOval(g2, RIGHT_GOAL_COLOR, DEFAULT_THICKNESS,
                 LANDMARK_RIGHT_GOAL_TOP_POST_X,
                 LANDMARK_RIGHT_GOAL_TOP_POST_Y,
                 GOAL_POST_RADIUS, GOAL_POST_RADIUS);
        // draw right goal bottom post
        fillOval(g2, RIGHT_GOAL_COLOR, DEFAULT_THICKNESS,
                 LANDMARK_RIGHT_GOAL_BOTTOM_POST_X,
                 LANDMARK_RIGHT_GOAL_BOTTOM_POST_Y,
                 GOAL_POST_RADIUS, GOAL_POST_RADIUS);

        // draw crossbar
        drawLine(g2, RIGHT_GOAL_COLOR, GOAL_CROSSBAR_THICKNESS,
                 LANDMARK_RIGHT_GOAL_TOP_POST_X,
                 LANDMARK_RIGHT_GOAL_TOP_POST_Y,
                 LANDMARK_RIGHT_GOAL_BOTTOM_POST_X,
                 LANDMARK_RIGHT_GOAL_BOTTOM_POST_Y);
    }




    protected void drawNet(Graphics2D g2, Color col,double strokeWidth,
                           double x1, double y1, double x2, double y2) {
        g2.setColor(col);
        g2.setStroke(new BasicStroke((float) strokeWidth));

        int numHorRects = (int) ((x2 - x1) / MESH_SIZE);
        int numVertRects = (int) ((y1 - y2) / MESH_SIZE);

        for (int i = 0; i <= numHorRects; i++) {
            for (int j = 0; j <= numVertRects; j++) {
                g2.drawRect((int) (x1 + (i * MESH_SIZE)),
                            (int) (fixYCoord(y1) + (j * MESH_SIZE)),
                            (int) MESH_SIZE, (int) MESH_SIZE);
            }
        }
    }


    /**
     * Draws a rectangle whose lower left corner is located at field
     * coordinates (left_x, bottom_y).  Transforms the coordinates into image
     * coordinates before rendering.
     * @param drawing_on the graphics context on which this method draws and
     * subsequently changes
     * @param in_color the Color to draw the rectangle frame
     * @param stroke the width of the line to draw, in pixels
     * @param left_x the x coordinate of bottom left of the rectangle - in
     * FIELD coordinates
     * @param bottom_y the y coord of the bottom left of the rectangle -
     * in FIELD coordinates
     * @param width the width of the rectangle, in cm (pixels)
     * @param height the height of the rectangle, in cm (pixels)
     */
    public void drawRect(Graphics2D drawing_on, Color in_color,
                         double stroke, double left_x,
                         double bottom_y, double width,
                         double height) {
        drawing_on.setColor(in_color);
        drawing_on.setStroke(new BasicStroke((float)stroke));
        drawing_on.drawRect(fixXCoord(left_x), fixYCoord(bottom_y + height),
                            fixWidth(width), fixHeight(height));
    }


    /**
     * Draws a filled rectangle whose lower left corner is at field coordinates
     * (left_x, bottom_y).
     * @see drawRect(Graphics2D, Color, double, double, double, double, double)
     * drawRect
     */
    public void fillRect(Graphics2D drawing_on, Color in_color,
                         double stroke, double left_x,
                         double bottom_y, double width,
                         double height) {
        drawing_on.setColor(in_color);
        drawing_on.setStroke(new BasicStroke((float)stroke));
        drawing_on.fillRect(fixXCoord(left_x),
                            fixYCoord(bottom_y + height),
                            fixWidth(width),
                            fixHeight(height));
    }

    /**
     * Draws a framed oval whose center is at (center_x, center_y) in field
     * coordinates.
     *
     * @param drawing_on the graphics context on which the method draws
     * @param in_color the color of the outline of the oval
     * @param stroke the width of the line drawing the oval, in pixels
     * @param center_x the x coordinate of the center of the oval in
     * field coordinates
     * @param center_y the y coordinate of the center of the oval in
     * field coordiantes
     * @param x_radius 1/2 the "width" of the oval
     * @param y_radius 1/2 the "height" of the oval
     */
    public void drawOval(Graphics2D drawing_on, Color in_color,
                         double stroke,
                         double center_x, double center_y,
                         double x_radius, double y_radius) {
        drawing_on.setColor(in_color);
        drawing_on.setStroke(new BasicStroke((float)stroke));
        drawing_on.drawOval(fixXCoord(center_x - x_radius),
                            fixYCoord(center_y + y_radius),
                            fixWidth(2. * x_radius),
                            fixHeight(2. * y_radius));
    }

    /**
     * Draws a filled oval whose center is at (center_x, center_y) in field
     * coordiantes.
     * @param drawing_on the graphics context on which the method draws
     * @param in_color the color of the outline of the oval
     * @param stroke the width of the line drawing the oval, in pixels
     * @param center_x the x coordinate of the center of the oval in
     * field coordinates
     * @param center_y the y coordinate of the center of the oval in
     * field coordiantes
     * @param x_radius 1/2 the "width" of the oval
     * @param y_radius 1/2 the "height" of the oval
     */
    public void fillOval(Graphics2D drawing_on, Color in_color,
                         double stroke,
                         double center_x, double center_y,
                         double x_radius, double y_radius) {
        drawing_on.setColor(in_color);
        drawing_on.setStroke(new BasicStroke((float)stroke));
        drawing_on.fillOval(fixXCoord(center_x - x_radius),
                            fixYCoord(center_y + y_radius),
                            fixWidth(2. * x_radius),
                            fixHeight(2. * y_radius));
    }

    /**
     * Draws a line between (x_1, y_1) and (x_2, y_2) in field coordinates.
     *
     * @param drawing_on the graphics context on which this method draws
     * @param in_color the color of the line
     * @param x_1 the x coordinate of one endpoint of the line, in field
     *        coordinates
     * @param y_1 the corresponding y coordinate of x_1, in field coordinates
     * @param x_2 the x coordinate of the second endpoint of the line, in
     *        field coordinates
     * @param y_2 the corresponding y coordinate of x_2, in field coordinates
     */
    public void drawLine(Graphics2D drawing_on, Color in_color,
                         double stroke,
                         double x_1, double y_1,
                         double x_2, double y_2) {
        drawing_on.setColor(in_color);
        drawing_on.setStroke(new BasicStroke((float)stroke));
        drawing_on.drawLine(fixXCoord(x_2), fixYCoord(y_2),
                            fixXCoord(x_1), fixYCoord(y_1));
    }

    /**
     * Draws a line between (x_1, y_1) and (x_2, y_2) in field coordinates.
     *
     * @param drawing_on the graphics context on which this method draws
     * @param in_color the color of the line
     * @param strok A strok object of the type to be painted on the field
     * @param x_1 the x coordinate of one endpoint of the line, in field
     *        coordinates
     * @param y_1 the corresponding y coordinate of x_1, in field coordinates
     * @param x_2 the x coordinate of the second endpoint of the line, in
     *        field coordinates
     * @param y_2 the corresponding y coordinate of x_2, in field coordinates
     */
    public void drawLine(Graphics2D drawing_on, Color in_color,
                         BasicStroke stroke,
                         double x_1, double y_1,
                         double x_2, double y_2) {
        drawing_on.setColor(in_color);
        drawing_on.setStroke(stroke);
        drawing_on.drawLine(fixXCoord(x_2), fixYCoord(y_2),
                            fixXCoord(x_1), fixYCoord(y_1));
    }


    public int yCor(int len, double dir) {return (int)(len * Math.cos(dir));}
    public int xCor(int len, double dir) {return (int)(len * Math.sin(dir));}

    /**
     * Draws a number at (left_x, bottom_y) in field coordinates.
     *
     * @param drawing_on the graphics context on which to draw the number
     * @param in_color the color in which the number will be drawn
     * @param left_x the x coord (field coordinates) where number will be
     *        drawn
     * @param bottom_y the y coord (field coordinates) where number will be
     *        drawn
     * @param number the number to draw
     */
    public void drawNumber(Graphics2D drawing_on, Color in_color,
                           double left_x, double bottom_y, int number) {
        drawing_on.setColor(in_color);
        drawing_on.drawString(""+number, fixXCoord(left_x),
                              fixYCoord(bottom_y));
    }
}