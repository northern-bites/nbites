package edu.bowdoin.robocup.TOOL.Classifier;

public class FieldObject implements java.io.Serializable {
    private int x1, y1, x2, y2, id;      

    public FieldObject(int id, int x1, int y1, int x2, int y2) {
        this.id = id;
        this.x1 = x1;
        this.y1 = y1;
        this.x2 = x2;
        this.y2 = y2;
        
    }

    public String toString() {
        return "(" + x1 + ", " + y1 + "), ("+
            x2 + ", " + y2 + ") " + id;

    }

}
