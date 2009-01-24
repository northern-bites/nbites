package TOOL.Classifier;

import java.awt.Color;
import java.awt.Point;

import TOOL.GUI.DefinableShape;
/** @author Nicholas Dunn
 * Don't forget, we read from TOP to BOTTOM.
 */

public class Beacon extends QuadrilateralFieldObject {

    public static final int NUM_TYPES = 3;
    // Not enough of post is seen to classify
    public static final int INDETERMINATE = 0;
    public static final int BY = 1;
    public static final int YB = 2;

    public static final String[] IDS = {
        "Indeterminate", "Blue Yellow", "Yellow Blue"
    };
    
    /**
     * Constructs a new Beacon object.  
     * order is as follows:  upper left, upper right, lower left, lower right
     */
    public Beacon(int id, int x1, int y1, int x2, int y2, int x3, int y3,
                int x4, int y4) {
        super(id,x1,y1,x2,y2,x3,y3,x4,y4);
    }

    /** Convenience method that uses Points instead of individual ints */
    public Beacon(int id, Point upperLeft, Point upperRight, Point lowerLeft,
                  Point lowerRight) {
        super(id, upperLeft, upperRight, lowerLeft, lowerRight);
    }

    
    public Beacon(int id, DefinableShape s, double scale) {
        super(id, s, scale);
    }

    public static String[] getLabelArray() { return IDS; }

    public Color getColor() { 
        switch(id) {
        case BY:
            return Color.GREEN.darker().darker();
        case YB:
            return Color.RED;
        case INDETERMINATE:
            return Color.BLACK;
        default:
            return null;
        }
        
    }


    public static int getIDFromString(String s) {
        String[] ids = getLabelArray();
        for (int i = 0; i < ids.length; i++) {
            if (ids[i].equals(s)) { return i; }
        }
        return -1;
    }
    public String getLabel() {
        if (id >= 0 && id < IDS.length) {
            return (occluded?"*O*":"") + IDS[id];
        }
        return "Invalid id in beacon";
    }

    public String toLog() {
    	return "[ Beacon "+(getOccluded() ? 1 : 0)+" ( "+getID()+", "+x1+", "+y1+", "+x2+", "+y2+
    				", "+x3+", "+y3+", "+x4+", "+y4+" ) ]";
    }

}
