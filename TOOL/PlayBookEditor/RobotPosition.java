package edu.bowdoin.robocup.TOOL.PlayBookEditor;

import edu.bowdoin.robocup.TOOL.Data.Field;
import java.awt.Point;

/**
 * This class holds the information for a robot position.  These positions can
 * take the form of an (X,Y) coordinate, a line, or more complex systems.
 * This class holds methods for accessing all of these forms of positioning
 * via simple getters and setters.
 */
public class RobotPosition
{
    // Constants
    // Position Types
    public static final int POINT_TYPE = 0;
    public static final int X_LINE_TYPE = 1;
    public static final int Y_LINE_TYPE = 2;
    public static final int BOX_TYPE = 3;

    // Type of position we are
    private int myType;

    // All of our positions are based around lines and offsets
    protected int xMin, xMax;
    protected int yMin, yMax;
    protected int x, y; // Current positions

    // Ball following things
    private boolean ballOffset;
    private int xOffset, yOffset; // Offsets to the ball

    // Constructors
    /**
     * Makes a BOX_TYPE position; a region bounded by two lines
     * @param xRange A range of the x values
     * @param yRange A range of the y values
     */
    public RobotPosition(Range xRange, Range yRange)
    {
	// Set type
	myType = BOX_TYPE;
	ballOffset = true;

	// Set x range
	xMin = xRange.getMin();
	xMax = xRange.getMax();

	// Set y range
	yMin = yRange.getMin();
	yMax = yRange.getMax();

	// Default position to center of the box
	x = (xMin + xMax) / 2;
	y = (yMin + yMax) / 2;
    }

    /**
     * Makes a position of type POINT_TYPE
     *
     * @param posPoint The (x,y) coordinate of the position
     */
    public RobotPosition(Point posPoint)
    {
	this(new Range(posPoint.x, posPoint.x),
	     new Range(posPoint.y, posPoint.y));
	// We set our type
	myType = POINT_TYPE;
    }

    /**
     * Makes an X_LINE_TYPE position; a line with constant y and variable x
     * @param xRange The range of the x values
     * @param yVal The y value to be held constant
     */
    public RobotPosition(Range xRange, int yVal)
    {
	this(xRange, new Range(yVal, yVal));
	// Set type
	myType = X_LINE_TYPE;
    }

    /**
     * Makes a Y_LINE_TYPE position; a line with constant x and variable y
     * @param xVal The x value to be held constant
     * @param yRange The range of the the y values
     */
    public RobotPosition(int xVal, Range yRange)
    {
	this(new Range(xVal, xVal), yRange);
	// Set type
	myType = Y_LINE_TYPE;
    }

    // Setters
    /**
     * @param xOff X offset value for following the ball
     * @param yOff Y offset value for following the ball
     */
    public void setBallOffset(int xOff, int yOff)
    {
	ballOffset = true;
	xOffset = xOff;
	yOffset = yOff;
    }

    public int getXOffset() {
	return xOffset;
    }
    public int getYOffset() {
	return yOffset;
    }

    // Getters
    /**
     * @param ball The current (x,y) of the ball
     * @return The current (x,y) position of the robot
     */
    public Point getCurrentPos(Ball ball)
    {
	if(ballOffset)
	    updateBallBasedPosition(ball);

	return new Point(x,y);
    }

    /**
     * @return The type of position returns either POINT_TYPE, X_LINE_TYPE,
      Y_LINE_TYPE, or BOX_TYPE
     */
    public int getType()
    {
	return myType;
    }

    /**
     * Method to return a PlayBookObject to be drawn in the editor
     *
     * @param _id The id to be associated with the object
     * @param _field The field object where the object will be drawn
     * @return The object associated with this RobotPosition
     */
    public PlayBookObject getPlayBookObject(String _id, Field _field)
    {
	if(myType == BOX_TYPE) {
	    return new Zone(_id, xMin, yMax, xMax, yMin, _field);
	} else if (myType == X_LINE_TYPE ||
		   myType == Y_LINE_TYPE) {
	    return new Line(_id, xMin, yMax, xMax, yMin, _field);
	} else {
	    return new PBEPoint(_id, xMin, yMin, _field);
	}
    }

    // Helper methods
    // Update our position based on the ball location
    private void updateBallBasedPosition(Ball ball)
    {
	x = clipX(ball.getX()+xOffset);
	y = clipY(ball.getY()+yOffset);
    }

    private int clipX(int toClip)
    {
	if(toClip < xMin)
	    return xMin;
	if(toClip > xMax)
	    return xMax;
	return toClip;
    }

    private int clipY(int toClip)
    {
	if(toClip < yMin)
	    return yMin;

	if(toClip > yMax)
	    return yMax;
	return toClip;
    }

}