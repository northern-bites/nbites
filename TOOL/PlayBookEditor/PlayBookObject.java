package edu.bowdoin.robocup.TOOL.PlayBookEditor;

import edu.bowdoin.robocup.TOOL.Data.Field;
import java.awt.Point;
import java.awt.Color;
import java.awt.Shape;

public abstract class PlayBookObject {

    protected static final int MIN_WIDTH = 10;
    protected static final int MIN_HEIGHT = MIN_WIDTH;

    // Used in resizing the objects
    public static final int N = java.awt.Cursor.N_RESIZE_CURSOR;
    public static final int NE = java.awt.Cursor.NE_RESIZE_CURSOR;
    public static final int E = java.awt.Cursor.E_RESIZE_CURSOR;
    public static final int SE = java.awt.Cursor.SE_RESIZE_CURSOR;
    public static final int S = java.awt.Cursor.S_RESIZE_CURSOR;
    public static final int SW = java.awt.Cursor.SW_RESIZE_CURSOR;
    public static final int W = java.awt.Cursor.W_RESIZE_CURSOR;
    public static final int NW = java.awt.Cursor.NW_RESIZE_CURSOR;

    // Upper left x,y.  Lower right x, y.  Define the bounding box of object
    protected int x1, y1, x2, y2;
    protected String id;
    protected Field field;


    public PlayBookObject(String _id, int _x1, int _y1, int _x2, int _y2,
                          Field _field) {
        id = _id;
        x1 = _x1;
        y1 = _y1;
        x2 = _x2;
        y2 = _y2;
        field = _field;
    }

    /**
     * Shifts the bounds of the zone by dx in x direction and dy in y
     * direction
     * @param dx amount in cm to move zone to the right
     *        (negative indicates left)
     * @param dy amount in cm to move zone up
     *        (negative indicates down)
     *
     */
    public void move(int dx, int dy) {
        x1 += dx;
        y1 += dy;
        x2 += dx;
        y2 += dy;
        //fixBounds();
    }

    /**
     * This method fixes the swapping of upper left/lower right that might
     * occur when resizing the rectangle
     */

    public void fixCoords() {
          if (x2 < x1) {
            int temp = x2;
            x2 = x1;
            x1 = temp;
        }
        if (y1 < y2) {
            int temp = y2;
            y2 = y1;
            y1 = temp;
        }
    }



     /**
     * Ensures that the zone lies firmly within the bounds of the Field
     * instance passed into it.
     */
    protected void fixBounds() {
        // Too far to the left of the screen
        if (x1 < 0) {
            x2 -= x1;
            x1 = 0;
        }
        // Too far to the right of the screen
        else if (x2 > field.FIELD_WIDTH) {
            x1 -= (x2 - (int)field.FIELD_WIDTH);
            x2 = (int)field.FIELD_WIDTH;
        }
        // Too high
        if (y1 > field.FIELD_HEIGHT) {
            y2 -= (y1 - (int)field.FIELD_HEIGHT);
            y1 = (int)field.FIELD_HEIGHT;
        }
        // Too low
        else if (y2 < 0) {
            y1 -= y2;
            y2 = 0;
        }
    }

    public int clipX(double x) {
        if (x < 0) { return 0; }
        else if (x > field.FIELD_WIDTH) { return (int) field.FIELD_WIDTH; }
        else { return (int) x; }
    }

    public int clipY(double y) {
        if (y < 0) { return 0; }
        else if (y > field.FIELD_HEIGHT) { return (int) field.FIELD_HEIGHT; }
        else { return (int) y; }
    }

    /* Getter methods */
    /**
     * Method generates a RobotPosition from the PlayBookObjectInstance
     */
    public RobotPosition generateRobotPosition()
    {
	if (x1 == x2) {
	    if (y1 == y2)
		return new RobotPosition(new Point(x1, y1));
	    return new RobotPosition(x1, new Range(y1, y2));
	} else if (y1 == y2) {
	    return new RobotPosition(new Range(x1, x2), y1);
	}
	return new RobotPosition(new Range(x1, x2), new Range(y1, y2));
    }

    public int getX1() { return x1; }
    public int getX2() { return x2; }
    public int getY1() { return y1; }
    public int getY2() { return y2; }
    public String getID() { return id; }

    public int getLeftX() { return x1; }
    public int getRightX() { return x2; }
    public int getTopY() { return y1; }
    public int getBottomY() { return y2; }

    public int getWidth() { return x2 - x1; }
    public int getHeight() { return y1 - y2; }

    public Point getUpperLeft() {
        return new Point(x1, y1);
    }
    public Point getUpperRight() {
        return new Point(x2, y1);
    }
    public Point getLowerLeft() {
        return new Point(x1, y2);
    }
    public Point getLowerRight() {
        return new Point(x2, y2);
    }

    public void resize(Point endPt,
                       int dir) {
        int newX = (int) endPt.getX();
        int newY = (int) endPt.getY();

        switch (dir) {
        case N:
            y1 = newY;
            break;
        case S:
            y2 = newY;
            break;
        case W:
            x1 = newX;
            break;
        case E:
            x2 = newX;
            break;
        case NE:
            y1 = newY;
            x2 = newX;
            break;
        case NW:
            y1 = newY;
            x1 = newX;
            break;
        case SW:
            y2 = newY;
            x1 = newX;
            break;
        case SE:
            y2 = newY;
            x2 = newX;
            break;
        }
    }

    public static boolean validSize(int width, int height) {
        return width >= MIN_WIDTH && height >= MIN_HEIGHT;
    }

    public Point getCurLoc(int dir) {
        switch (dir) {
        case N:
            return new Point(0, y1);
        case S:
            return new Point(0, y2);
        case W:
            return new Point(x1, 0);
        case E:
            return new Point(x2, 0);
        case NE:
            return new Point(x2, y1);
        case NW:
            return new Point(x1, y1);
        case SW:
            return new Point(x1, y2);
        case SE:
            return new Point(x2, y2);
        default:
            System.err.println("You should never get this message.");
            return null;
        }
    }

    public void setID(String i) { id = i; }

    /*****************************************
     * Abstract methods that playbook objects must implement
     *****************************************/

    public String toString() {
        return "ID: " + id +
            " (" + x1 + ", " + y1 + ")" +
            " (" + x2 + ", " + y2 + ")";
    }


    public abstract Color getColor();

    public abstract Shape getShape();

    public abstract PlayBookObject clone();

    public abstract boolean contains(int x, int y);

    
}