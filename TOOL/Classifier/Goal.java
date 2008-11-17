package edu.bowdoin.robocup.TOOL.Classifier;

import java.awt.Point;
import java.awt.Color;

import edu.bowdoin.robocup.TOOL.GUI.DefinableShape;

/**
 * Holds the information regarding a piece of the goal; can either be
 * the backstop, the left goal post, or the right goal post.  All share
 * the quality of being rectangular and thus subclass the RectangularField \
 * Object class.
 * @author Nicholas Dunn
 */
public class Goal extends QuadrilateralFieldObject{
    
    private static final int BLUE = 0;
    private static final int YELLOW  = 1;

    public static final int NUM_TYPES = 6;
    public static final int BLUE_GOAL_LEFT = 0;
    public static final int BLUE_GOAL_RIGHT = 1;
    public static final int BLUE_GOAL_BACKSTOP = 2;
    public static final int YELLOW_GOAL_LEFT = 3;
    public static final int YELLOW_GOAL_RIGHT = 4;
    public static final int YELLOW_GOAL_BACKSTOP = 5;
    
    public static final String[] IDS = {
        "Blue goal left post",
        "Blue goal right post",
        "Blue goal backstop",
        "Yellow goal left post",
        "Yellow goal right post",
        "Yellow goal backstop"
    };
    public static final String[] BLUE_IDS = {
        IDS[BLUE_GOAL_LEFT],
        IDS[BLUE_GOAL_RIGHT],
        IDS[BLUE_GOAL_BACKSTOP]
    };
    public static final String[] YELLOW_IDS = {
        IDS[YELLOW_GOAL_LEFT],
        IDS[YELLOW_GOAL_RIGHT],
        IDS[YELLOW_GOAL_BACKSTOP]
    };


    /** Default constructor */
    public Goal(int id, int x1, int y1, int x2, int y2, int x3, int y3,
                int x4, int y4) {
        super(id,x1,y1,x2,y2,x3,y3,x4,y4);
    }

    /** Convenience method that uses Points instead of individual ints */
    public Goal(int id, Point upperLeft, Point upperRight, Point lowerLeft,
                  Point lowerRight) {
        super(id, upperLeft, upperRight, lowerLeft, lowerRight);
    }

    public Goal(int id, DefinableShape s, double scale) {
        super(id, s, scale);
    }

    
    public static String[] getLabelArray() { return IDS; }
    
    /** Converts a local blue specific id into a general/global one */
    public static final int blueIDToGlobal(int n) {
        if (n > BLUE_IDS.length || n < 0) {
            System.err.println("Error, received incompatible blue goal ID");
            return -1;
        }
        return n;
    }
    /** Converts a local yellow specific id into a general/global one */
    public static final int yellowIDToGlobal(int n) {
        if (n > YELLOW_IDS.length || n < 0) {
            System.err.println("Error, received incompatible yellow goal ID");
            return -1;
        }
        return n + BLUE_IDS.length;
    }


     public static int getIDFromString(String s) {
        String[] ids = getLabelArray();
        for (int i = 0; i < ids.length; i++) {
            if (ids[i].equals(s)) { return i; }
        }
        return -1;
    }
  
    public Color getColor() { 
        switch (id) {
        case BLUE_GOAL_LEFT:
        case YELLOW_GOAL_LEFT:
            return Color.RED;

        case BLUE_GOAL_RIGHT:
        case YELLOW_GOAL_RIGHT:
            return Color.BLACK;

        case BLUE_GOAL_BACKSTOP:
            return Color.YELLOW;
        case YELLOW_GOAL_BACKSTOP:
            return Color.BLUE;
        default:
            return null;
        }
    }
    public String getLabel() {
        if (id >= 0 && id < IDS.length) {
            return (occluded?"*O*":"") + IDS[id];
        }
        return "Invalid id in center circle";
    }
    
    public String toLog() {
    	return "[ Goal "+(getOccluded() ? 1 : 0)+" ( "+getID()+", "+x1+", "+y1+", "+x2+", "+y2+
    				", "+x3+", "+y3+", "+x4+", "+y4+" ) ]";
    }
}
