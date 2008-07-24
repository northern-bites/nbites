package TOOL.Classifier;

import java.awt.Point;
import java.awt.Color;
import java.awt.Shape;
import TOOL.GUI.DefinableShape;
import TOOL.GUI.Ellipse;

public class CenterCircle extends CircularObject {
    
    public static final int ALL_VISIBLE = 1;
    public static final int PARTIALLY_OBSCURED = 0;
    
    public static final String IDS[] = {
        "Obscured center circle",
        "All visible center circle"
    };

    public CenterCircle(int id, DefinableShape s, double scale) {
        super(id, s, scale);
    }

    public CenterCircle(int id, int x1, int y1, int w, int h){
        super(id, x1, y1, w, h);
    }

    public void setColor(Color a) {}
    public Color getColor() { return Color.CYAN; }

   
    public static String[] getLabelArray() { return IDS; }

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
        return "Invalid id in center circle";
    }
    
    public String toLog() {
    	return "[ CenterCircle "+(getOccluded() ? 1 : 0)+" ( "+getID()+", "+x+", "+y+", "+width+", "+height+" ) ]";
    }

    

}
