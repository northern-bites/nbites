package TOOL.Classifier;

import java.awt.Point;
import java.awt.Color;
import java.awt.Shape;
import java.awt.geom.Line2D;
import java.awt.Rectangle;
import java.awt.Graphics2D;

import TOOL.GUI.DefinableShape;

/**
 * @author Nicholas Dunn1
 * @date February 2, 2008
 */

public class Line extends ClassifierObject implements java.io.Serializable {
    
    // NOTE:  If you change this, you MUST change LineDef.h to correspond!

    
    // COMPLETELY GENERAL ID
    public static final int INDETERMINATE_LINE = 0;

    // FUZZY LINE IDS
    public static final int GOAL_FRONT = 1;
    public static final int GOAL_LEFT = 2;
    public static final int GOAL_RIGHT = 3;
    public static final int GOAL_BACK = 4;
    // Cannot see which side of the field you're on
    public static final int SIDELINE = 5;

    // SPECIFIC LINE IDS
    public static final int MIDLINE = 6;
    public static final int BY_LINE = 7;
    public static final int YB_LINE = 8;
    public static final int BLUE_GOAL_BACK_LINE = 9;
    public static final int BLUE_GOAL_FRONT_LINE = 10;
    public static final int BLUE_GOAL_LEFT_LINE = 11;
    public static final int BLUE_GOAL_RIGHT_LINE = 12;
    public static final int YELLOW_GOAL_BACK_LINE = 13;
    public static final int YELLOW_GOAL_FRONT_LINE = 14;
    public static final int YELLOW_GOAL_LEFT_LINE = 15;
    public static final int YELLOW_GOAL_RIGHT_LINE = 16;

    public static final String[] IDS = {
        "Indeterminate line",
        "Goal line front",
        "Goal line left",
        "Goal line right",
        "Goal back",
        "Side line",
        "Midfield line",
        "Blue/yellow side line",
        "Yellow/blue side line",
        "Blue goal back line",
        "Blue goal front line",
        "Blue goal left line",
        "Blue goal right line",
        "Yellow goal back line",
        "Yellow goal front line",
        "Yellow goal left line",
        "Yellow goal right line"
    };

    public static final String[] generalIDs = {
        IDS[INDETERMINATE_LINE]
    };

    public static final String[] fuzzyIDs = {
        IDS[GOAL_FRONT],
        IDS[GOAL_LEFT],
        IDS[GOAL_RIGHT],
        IDS[SIDELINE]
    };

    public static final String[] clearIDS = {
        IDS[MIDLINE],
        IDS[BY_LINE],
        IDS[YB_LINE],
        IDS[BLUE_GOAL_BACK_LINE],
        IDS[BLUE_GOAL_FRONT_LINE],
        IDS[BLUE_GOAL_LEFT_LINE],
        IDS[BLUE_GOAL_RIGHT_LINE],
        IDS[YELLOW_GOAL_BACK_LINE],
        IDS[YELLOW_GOAL_FRONT_LINE],
        IDS[YELLOW_GOAL_LEFT_LINE],
        IDS[YELLOW_GOAL_RIGHT_LINE]
    };

    public static final String[] descriptions = {
        "General",
        "Fuzzy",
        "Clear"
    };

    
    private int id, x1, y1, x2, y2;
    private Line2D shape;

    public Line(int _id, int _x1, int _y1, int _x2, int _y2) {
        id = _id;
        x1 = _x1;
        y1 = _y1;
        x2 = _x2;
        y2 = _y2;
        shape = new Line2D.Float(x1,y1,x2,y2);
    }

    public Line(int _id, Point first, Point second) {
        this(_id, (int) first.getX(), (int) first.getY(),
             (int) second.getX(), (int) second.getY());
    }
    
    public Line(int id, DefinableShape s, double scale) {
        this.id = id;
        getPointsFromShape(s, scale);
        shape = new Line2D.Float(x1,y1,x2,y2);
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
        if (!(s instanceof TOOL.GUI.Line)) {
            System.err.println("Error: in line");
            TOOL.TOOL.CONSOLE.error("Error: Tried to pass a non Line shape "+
                                    "into the Line method..");
            return;
        }
        Point[] p = s.getSignificantPoints();/*
        int minX = (int) (Math.min(p[0].getX(), p[1].getX()) / scale);
        int maxY = (int) (Math.max(p[0].getY(), p[1].getY()) / scale);
        int minX = (int) (Math.min(p[0].getX(), p[1].getX()) / scale);
        int maxY = (int) (Math.max(p[0].getY(), p[1].getY()) / scale);
                                             */
        x1 = (int) (p[0].getX() / scale);
        y1 = (int) (p[0].getY() / scale);
        x2 = (int) (p[1].getX() / scale);
        y2 = (int) (p[1].getY() / scale);
    }


    public void setColor(Color a) {}
    public Color getColor() { return Color.CYAN.darker();}
    
    public Point[] getHandleLocations() {
        return new Point[] {
            new Point(x1, y1),
            new Point(x2, y2)
        };
    }

    
    public String toString() {
        return "x1:"+x1 + " y1:" + y1 +
            " x2: " + x2 + " y2: " + y2;
    }
    
    public String toLog() {
    	// Slope/distance/point form seems kind of pointless, since that would involve more rounding
    	// and could become progressively less accurate as the file is repeatedly loaded.
    	// if that decision is wrong, please correct it
    	// - hchapman
    	return "[ Line "+(getOccluded() ? 1 : 0)+" ( "+getID()+", "+x1+", "+y1+", "+x2+", "+y2+" ) ]";
    }

    public int getID() { return id; }

    public String getLabel() {
        if (id >= 0 && id < IDS.length) {
            return (occluded?"*O*":"") + IDS[id];
        }
        return "Invalid id in line";
    }


    /** 
     * A line always returns false for containment in java, so instead we
     * check if the line intersects a small region around the (x, y) coordinate.
     * If so we say it is contained.
     */
    public boolean contains(int x, int y) {
        Rectangle boundingBox = new Rectangle(x - MOUSE_OVER_SIZE / 2,
                                              y - MOUSE_OVER_SIZE / 2,
                                              MOUSE_OVER_SIZE,
                                              MOUSE_OVER_SIZE);
        return getShape().intersects(boundingBox);
    }

    public Shape getShape() {
        return shape; 
    }


    public void move(int dx, int dy) {
        x1 += dx; y1 += dy; 
        x2 += dx; y2 += dy;
        shape.setLine(x1,y1,x2,y2);
    }

    public void setID(int x) {
        id = x;
    }
    

    
    public static void drawRaw(Graphics2D canvas, int x1, int y1, int x2, int y2) {
        canvas.drawLine(x1,y1,x2,y2);
    }
    
    

    public void scale(double dx, double dy) {
         
    }
}
