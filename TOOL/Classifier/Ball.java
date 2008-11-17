package edu.bowdoin.robocup.TOOL.Classifier;

import java.awt.Point;
import java.awt.Color;
import java.awt.Shape;
import java.awt.geom.Ellipse2D;
import edu.bowdoin.robocup.TOOL.GUI.DefinableShape;
import edu.bowdoin.robocup.TOOL.GUI.Ellipse;

public class Ball extends CircularObject implements java.io.Serializable {

 public static final int OBSCURED = 0;
    public static final int VISIBLE = 1;
    public static final String IDS[] = {"Visible ball", "Obscured ball"};


    public Ball(int id, int x, int y, int w, int h) {
        super(id, x, y, w, h);
    }

    public Ball(int id, DefinableShape s, double scale) {
        super(id, s, scale);
    }


    public static int getIDFromString(String s) {
        String[] ids = getLabelArray();
        for (int i = 0; i < ids.length; i++) {
            if (ids[i].equals(s)) { return i; }
        }
        return -1;
    }
    

    public static String[] getLabelArray() { return IDS; }

    public void setColor(Color a) {}
    public Color getColor() { return Color.PINK.darker(); }


    public String getLabel() {
        if (id >= 0 && id < IDS.length) { return (occluded?"*O*":"") + IDS[id]; }
        return "Incorrect id for Ball object";
    }

    public String toLog() {
    	return "[ Ball "+(getOccluded() ? 1 : 0)+" ( "+getID()+", "+x+", "+y+", "+width+", "+height+" ) ]";
    }
    
   
}
