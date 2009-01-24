package TOOL.Classifier;

import java.awt.Point;
import java.awt.Color;
import java.awt.Shape;
import java.awt.geom.GeneralPath;
import java.awt.Rectangle;
import TOOL.GUI.CrossPoint;
import TOOL.GUI.DefinableShape;
import TOOL.TOOL;


/**
 * @author Nicholas Dunn
 * @date 02/02/08
 * Note: if you modify constants, you MUST modify linedef.h to reflect change.
 */
public class Corner extends ClassifierObject implements java.io.Serializable {
    // Type can either be L or T
    private int type, id, x, y;

    public static final int L = 0;
    public static final int T = 1;
    
    private static final int L_INNER_CORNER = 0;
    private static final int L_OUTER_CORNER = 1;
    private static final int T_CORNER = 2;

    // Will not use for our purposes
    private static final int CENTER_CIRCLE = 3;

    // FUZZY/CLEAR CORNER IDS
    private static final int BLUE_GOAL_T = 4;
    private static final int YELLOW_GOAL_T = 5;
    private static final int BLUE_GOAL_RIGHT_L_OR_YELLOW_GOAL_LEFT_L = 6;
    private static final int BLUE_GOAL_LEFT_L_OR_YELLOW_GOAL_RIGHT_L = 7;
    private static final int CORNER_INNER_L = 8;
    private static final int GOAL_BOX_INNER_L = 9;
    private static final int CORNER_OR_GOAL_INNER_L = 10;
    private static final int BLUE_GOAL_OUTER_L = 11;
    private static final int YELLOW_GOAL_OUTER_L = 12;
    private static final int CENTER_T = 13;

    // SPECIFIC CORNER IDS
    private static final int BLUE_CORNER_LEFT_L = 14;
    private static final int BLUE_CORNER_RIGHT_L = 15;
    private static final int BLUE_GOAL_LEFT_T = 16;
    private static final int BLUE_GOAL_RIGHT_T = 17;
    private static final int BLUE_GOAL_LEFT_L = 18;
    private static final int BLUE_GOAL_RIGHT_L = 19;
    private static final int CENTER_BY_T = 20;
    private static final int CENTER_YB_T = 21;
    private static final int YELLOW_CORNER_LEFT_L = 22;
    private static final int YELLOW_CORNER_RIGHT_L = 23;
    private static final int YELLOW_GOAL_LEFT_T = 24;
    private static final int YELLOW_GOAL_RIGHT_T = 25;
    private static final int YELLOW_GOAL_LEFT_L = 26;
    private static final int YELLOW_GOAL_RIGHT_L = 27;


    public static final String[] IDS = {
        "L inner corner",
        "L outer corner",
        "T corner",
        "Center circle - DO NOT PICK THIS ONE",
        "Blue goal T",
        "Yellow goal T",
        "Blue goal right L or Yellow goal left L",
        "Blue goal left L or Yellow goal right L",
        "Corner inner L",
        "Goal box inner L",
        "Corner or goal inner L",
        "Blue goal outer L",
        "Yellow goal outer L",
        "Center T",
        "Blue corner left L",
        "Blue corner right L",
        "Blue goal left T",
        "Blue goal right T",
        "Blue goal left L",
        "Blue goal right L",
        "Center BY T",
        "Center YB T",
        "Yellow corner left L",
        "Yellow corner right L",
        "Yellow goal left T",
        "Yellow goal right T",
        "Yellow goal left L",
        "Yellow goal right L"
    };
    
    
    public static final String[] L_CORNER_IDS = {
        IDS[L_INNER_CORNER],
        IDS[L_OUTER_CORNER],
        IDS[BLUE_GOAL_RIGHT_L_OR_YELLOW_GOAL_LEFT_L],
        IDS[BLUE_GOAL_LEFT_L_OR_YELLOW_GOAL_RIGHT_L],
        IDS[CORNER_INNER_L],
        IDS[GOAL_BOX_INNER_L],
        IDS[CORNER_OR_GOAL_INNER_L],
        IDS[BLUE_GOAL_OUTER_L],
        IDS[YELLOW_GOAL_OUTER_L],
        IDS[BLUE_CORNER_LEFT_L],
        IDS[BLUE_CORNER_RIGHT_L],
        IDS[BLUE_GOAL_LEFT_L],
        IDS[BLUE_GOAL_RIGHT_L],
        IDS[YELLOW_CORNER_LEFT_L],
        IDS[YELLOW_CORNER_RIGHT_L],
        IDS[YELLOW_GOAL_LEFT_L],
        IDS[YELLOW_GOAL_RIGHT_L]
    };

    public static final String[] T_CORNER_IDS = {
        IDS[T_CORNER],
        IDS[BLUE_GOAL_T],
        IDS[YELLOW_GOAL_T],
        IDS[CENTER_T],
        IDS[BLUE_GOAL_LEFT_T],
        IDS[BLUE_GOAL_RIGHT_T],
        IDS[CENTER_BY_T],
        IDS[CENTER_YB_T],
        IDS[YELLOW_GOAL_LEFT_T],
        IDS[YELLOW_GOAL_RIGHT_T],
    };

    public static final int NUM_T_CORNERS = T_CORNER_IDS.length;
    public static final int NUM_L_CORNERS = L_CORNER_IDS.length;
    
    
    public static final int LINE_LENGTH = 5;

       
    public Corner(int id, DefinableShape s, double scale) {
        this.id = id;
        getPointsFromShape(s, scale);
    }

    public Corner(int id, int type, int x, int y) {
        this.type = type;
        this.x = x;
        this.y = y;
        this.id = id;
    }
    
    
    public static String[] getLabelArray() { return IDS; }
    
    public static int getIDFromString(String s) {
        String[] ids = getLabelArray();
        for (int i = 0; i < ids.length; i++) {
            if (ids[i].equals(s)) { return i; }
        }
        return -1;
    }

    public void getPointsFromShape(DefinableShape s, double scale) {
        if (!(s instanceof CrossPoint)) {
            TOOL.CONSOLE.error("Error: Tried to pass a non point shape "+
                                    "into the corner method..");
            return;
        }
        Point p = s.getSignificantPoints()[0];
        x = (int) (p.getX() / scale);
        y = (int) (p.getY() / scale);

    }

    public static int tIDToGlobal(int n) {
        if (n > NUM_T_CORNERS || n < 0) {
            System.err.println("Error, received incompatible T Corner ID");
            return -1;
        }
        
        
        // TODO - insert conversion code here that doesn't use string comps
        for (int i = 0; i < IDS.length; i++) {
            if (T_CORNER_IDS[n].equals(IDS[i])) {
                return i;
            }
        }
        System.err.println("Error, received incompatible T Corner ID");
        return -1;
        

    }

    public static int lIDToGlobal (int n) {
        if (n > NUM_L_CORNERS || n < 0) {
            System.err.println("Error, received incompatible L Corner ID");
            return -1;
        }
        
         // TODO - insert conversion code here that doesn't use string comps
        for (int i = 0; i < IDS.length; i++) {
            if (L_CORNER_IDS[n].equals(IDS[i])) {
                return i;
            }
        }
        System.err.println("Error, received incompatible T Corner ID");
        return -1;
    }

    
    public void setColor(Color a) {}

    public Color getColor() { return Color.ORANGE.darker();}

    public String toString() {
        return "(" + x + "," + y + ")" + IDS[id];
    }

    public Point[] getHandleLocations() {
        return new Point[] {
            new Point(x, y)
        };
    }
    public int getID() { return id; }

    public Shape getShape() {
        // a point is just a cross in our system/
        GeneralPath point = new GeneralPath();
        // Horizontal line segment
        point.moveTo(x-LINE_LENGTH/2,y);
        point.lineTo(x+LINE_LENGTH/2,y);
        // Vertical line segment
        point.moveTo(x,y-LINE_LENGTH/2);
        point.lineTo(x,y+LINE_LENGTH/2);
        return point;
    }


    public String getLabel() {
        if (id >= 0 && id < IDS.length) {
            return (occluded?"*O*":"") + IDS[id];
        }
        return "Invalid id in corner";
    }
    
    public String toLog() {
    	return "[ Corner "+(getOccluded() ? 1 : 0)+" ( "+getID()+", "+x+", "+y+" ) ]";
    }

    public void move(int dx, int dy) {
        x += dx; y += dy;
    }

    public void setID(int x) {
        id = x;
    }

    public void scale(double dx, double dy) {
        
    }

    public boolean contains(int x, int y) {
        Rectangle boundingBox = new Rectangle(x - MOUSE_OVER_SIZE / 2,
                                              y - MOUSE_OVER_SIZE / 2,
                                              MOUSE_OVER_SIZE,
                                              MOUSE_OVER_SIZE);
        return getShape().intersects(boundingBox);
    }
}
