package edu.bowdoin.robocup.TOOL.Classifier;

import java.awt.Point;
import java.awt.Shape;
import edu.bowdoin.robocup.TOOL.GUI.Ellipse;
import edu.bowdoin.robocup.TOOL.GUI.DefinableShape;
import edu.bowdoin.robocup.TOOL.TOOL;

public abstract class CircularObject extends RectangularFieldObject {
    protected int id, x, y, width, height, r;

    public CircularObject(int id, int x, int y, int w, int h) {
        super(id, x, y, w, h);
        /*
        this.id = id;
        this.x = x;
        this.y = y;
        this.width = w;
        this.height = h;*/
    }

    public CircularObject(int id, DefinableShape s, double scale) {
        super(id,0,0,0,0);
        getPointsFromShape(s, scale);
    }
    
    public void getPointsFromShape(DefinableShape s, double scale) {
        if (!(s instanceof Ellipse)) {
            TOOL.CONSOLE.error("Error: Tried to pass a non elliptical " +
                                    "shape into the corner method..");
            return;
        }
        Point[] p = s.getSignificantPoints();
        
        int x1 = (int) (p[0].getX() / scale);
        int y1 = (int) (p[0].getY() / scale);
        int x2 = (int) (p[1].getX() / scale);
        int y2 = (int) (p[1].getY() / scale);
        
        x = Math.min(x1, x2);
        y = Math.min(y1, y2);

        width = Math.abs(x2 - x1);
        height = Math.abs(y2 - y1);
    }
    

    


    public void setID(int x) { id = x; }

        
    public int getID() { return id; }

    public void move(int dx, int dy) {
        x += dx;
        y += dy;
        
    }
    
    public Shape getShape() {
        return new java.awt.geom.Ellipse2D.Double(x, y, 
                                                  width, height);
    }
    
    public void scale(double dx, double dy) {
        
    }

    public abstract String getLabel();

}
