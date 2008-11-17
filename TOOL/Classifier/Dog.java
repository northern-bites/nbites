package edu.bowdoin.robocup.TOOL.Classifier;

import edu.bowdoin.robocup.TOOL.GUI.DefinableShape;
import java.awt.Color;


public class Dog extends RectangularFieldObject {
    
    
    public static final String[] IDS = {"Red Dog", "Blue Dog", "Naked Dog"};
    public static final int RED = 0;
    public static final int BLUE = 1;
    public static final int NAKED = 2;


    public Dog(int id, int x1, int y1, int w, int h) {
        super(id, x1, y1, w, h);
    }

    public Dog(int id, DefinableShape s, double scale) {
        super(id, s, scale);
    }
    
    public static String[] getLabelArray() { return IDS; }

    public static int getIDFromString(String s) {
        String[] ids = getLabelArray();
        for (int i = 0; i < ids.length; i++) {
            if (ids[i].equals(s)) { return i; }
        }
        return -1;
    }

    public String getLabel() {
        return IDS[id];
    }

    public Color getColor() {
        switch (id) {
        case RED:
            return Color.RED;
        case BLUE:
            return Color.BLUE;
        case NAKED:
            return Color.WHITE;
        default:
            return Color.BLACK;
        }
    }
    
    
}