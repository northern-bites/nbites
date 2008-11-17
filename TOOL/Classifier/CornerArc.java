package edu.bowdoin.robocup.TOOL.Classifier;

import java.awt.Point;
import java.awt.Color;
import java.awt.Shape;
import java.awt.geom.GeneralPath;

import edu.bowdoin.robocup.TOOL.GUI.DefinableShape;
import edu.bowdoin.robocup.TOOL.GUI.Arc;
import edu.bowdoin.robocup.TOOL.TOOL;

public class CornerArc extends ClassifierObject {

    // General ID
    public static final int BLUE = 0;
    public static final int YELLOW = 1;

    // Specific ID
    // blue arcs always on right side of goal
    public static final int BLUE_GOAL_BLUE_ARC = 2;
    public static final int BLUE_GOAL_YELLOW_ARC = 3;

    // yellow always on left
    public static final int YELLOW_GOAL_BLUE_ARC = 4;
    public static final int YELLOW_GOAL_YELLOW_ARC = 5;

    public static final String IDS[] = {
        "Blue arc",
        "Yellow arc",
        "Blue goal blue arc",
        "Blue goal yellow arc",
        "Yellow goal blue arc",
        "Yellow goal yellow arc"
    };

    public static final String BLUE_IDS[] = {
        IDS[BLUE],
        IDS[BLUE_GOAL_BLUE_ARC],
        IDS[YELLOW_GOAL_BLUE_ARC]
    };
    
    public static final String YELLOW_IDS[] = {
        IDS[YELLOW],
        IDS[YELLOW_GOAL_YELLOW_ARC],
        IDS[BLUE_GOAL_YELLOW_ARC]
    };

    public static final int NUM_BLUE_IDS = BLUE_IDS.length;
    public static final int NUM_YELLOW_IDS = YELLOW_IDS.length;
    
    public static String[] getLabelArray() { return IDS; }
    
    public static int getIDFromString(String s) {
        String[] ids = getLabelArray();
        for (int i = 0; i < ids.length; i++) {
            if (ids[i].equals(s)) { return i; }
        }
        return -1;
    }
    
    /*
    public static int stringToID(String s){
        
    }*/

    public static int blueIDToGlobal(int n) {
        if (n > NUM_BLUE_IDS || n < 0) {
            System.err.println("Error, received incompatible Blue arc ID");
            return -1;
        }
        // TODO - insert conversion code here that doesn't use string comps
        for (int i = 0; i < IDS.length; i++) {
            if (BLUE_IDS[n].equals(IDS[i])) {
                return i;
            }
        }
        System.err.println("Error, received incompatible Blue arc ID");
        return -1;
    }

    public static int yellowIDToGlobal(int n) {
        if (n > NUM_YELLOW_IDS || n < 0) {
            System.err.println("Error, received incompatible Yellow arc ID");
            return -1;
        }
        // TODO - insert conversion code here that doesn't use string comps
        for (int i = 0; i < IDS.length; i++) {
            if (YELLOW_IDS[n].equals(IDS[i])) {
                return i;
            }
        }
        System.err.println("Error, received incompatible Yellow arc ID");
        return -1;
        
    }


    private int x1, y1, x2, y2, radiusX, radiusY, id;

    public CornerArc(int id, int x1, int y1, int x2, 
                     int y2, int radiusX, int radiusY) {
        this.id = id;
        this.x1 = x1;
        this.y1 = y1;
        this.x2 = x2;
        this.y2 = y2;
        this.radiusX = radiusX;
        this.radiusY = radiusY;
    }

    public CornerArc(int id, DefinableShape s, double scale) {
        this.id = id;
        getPointsFromShape(s, scale);
    }

    public void getPointsFromShape(DefinableShape s, double scale) {
        if (!(s instanceof Arc)) {
            TOOL.CONSOLE.error("Error: Tried to pass a non point shape "+
                                    "into the arc method..");
            return;
        }

        Point p[] = s.getSignificantPoints();
        
        x1 = (int) (p[0].getX() / scale);
        y1 = (int) (p[0].getY() / scale);
        x2 = (int) (p[1].getX() / scale);
        y2 = (int) (p[1].getY() / scale);
        radiusX = (int) (p[2].getX() / scale);
        radiusY = (int) (p[2].getY() / scale);
    }



    public void setColor(Color a) {}
    public Color getColor() {  
        // Even ones are blue
        if (id%2 == 0) { return Color.BLUE; }
        else { return Color.YELLOW; }
    }

    public Point[] getHandleLocations() {
        return new Point[] {
            new Point(x1, y1),
            new Point(x2, y2),
            new Point(radiusX, radiusY)
        };
    }
    public int getID() { return id; }

    public Shape getShape() {
        GeneralPath a = new GeneralPath();
        a.moveTo(x1,y1);
        // draw a curve through (radiusX, radiusY) reaching the point (x2,y2)
        a.quadTo(radiusX, radiusY, x2, y2);
        return a;
    }
    public String getLabel() {
        if (id >= 0 && id < IDS.length) {
            return (occluded?"*O*":"") + IDS[id];
        }
        return "Invalid id in corner arc";
    }
    
    public String toLog() {
    	return "[ CornerArc "+(getOccluded() ? 1 : 0)+" ( "+getID()+", "+x1+", "+y1+", "+x2+", "+y2+
    			", "+radiusX+", "+radiusY+" ) ]";
    }


    public void move(int dx, int dy) {
        x1 += dx; y1 += dy; 
        x2 += dx; y2 += dy;
        radiusX += dx; radiusY += dy;
    }

    public void scale(double dx, double dy) {
         
    }

    public void setID(int x) {
        id = x;
    }

}
