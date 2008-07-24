package TOOL.Classifier;

import java.awt.Point;
import java.awt.Color;
import java.awt.Shape;
import java.awt.geom.GeneralPath;
import java.awt.Rectangle;

import java.util.Arrays;

import TOOL.GUI.DefinableShape;
/**
 * Superclass to all field objects whose shape is generally rectangular;
 * this includes beacons and goals.  Specifically, this class is used to
 * hold the data associated with rectangular shapes that are at tilted or
 * otherwise skewed from a normal right rectangle.
 * In order to represent such a skewed shape, we hold 4 sets of points, one
 * for each corner in the box.  The order is as follows:
 * upper left, upper right, lower left, lower right.
 *
 * Any class subclassing this one merely needs to define its ID strings etc.
 * @author Nicholas Dunn
 *
 */
public abstract class QuadrilateralFieldObject extends ClassifierObject {
    
    protected int x1, y1, x2, y2, x3, y3, x4, y4, id; 
    

    public QuadrilateralFieldObject(DefinableShape s, double scale) {
        getPointsFromShape(s, scale);

    }

    /**
     * @param id The global identifier for the goal
     * @param x1 The x coord of the upper left corner
     * @param y1 The y coord of the upper left corner
     * @param x2 The x coord of the upper right corner
     * @param y2 The y coord of the upper right corner
     * @param x3 The x coord of the lower left corner
     * @param y3 The y coord of the lower left corner
     * @param x4 The x coord of the lower right corner
     * @param y4 The y coord of the lower right corner
     */
    public QuadrilateralFieldObject(int id, 
                                  int x1, int y1, int x2, int y2, 
                                  int x3, int y3, int x4, int y4) {
        this.id = id;
        this.x1 = x1;
        this.y1 = y1;
        this.x2 = x2;
        this.y2 = y2;
        this.x3 = x3;
        this.y3 = y3;
        this.x4 = x4;
        this.y4 = y4;
    }

    /** Convenience constructor that strips the ints out of the Points and
     * then calls the default constructor */
    public QuadrilateralFieldObject(int id, Point upperLeft,
                                    Point upperRight, Point lowerLeft,
                                    Point lowerRight) {
        this(id, (int) upperLeft.getX(), (int) upperLeft.getY(),
             (int) upperRight.getX(), (int) upperRight.getY(),
             (int) lowerLeft.getX(), (int) lowerLeft.getY(),
             (int) lowerRight.getX(), (int) lowerRight.getY());
    }

    
    public QuadrilateralFieldObject(int id, DefinableShape s, double scale) {
        this.id = id;
        getPointsFromShape(s, scale);
    }

    
    public static String[] getLabelArray(){ return null; }

    public void getPointsFromShape(DefinableShape s, double scale) {
        if (!(s instanceof TOOL.GUI.Quadrilateral)) {
            TOOL.TOOL.CONSOLE.error("Error: Tried to pass a non quadrilateral "+
                                    "shape into the corner method..");
            return;
        }
        
        Point[] points = s.getSignificantPoints();
        Arrays.sort(points, ClassifierObject.Y_Order);
        
        Point[] top = new Point[] {points[0], points[1]};
        Point[] bottom = new Point[] {points[2], points[3]};
        
        Arrays.sort(top, ClassifierObject.X_Order);
        Arrays.sort(bottom, ClassifierObject.X_Order);
        
        // Upper left
        x1 = (int) (top[0].getX() / scale);
        y1 = (int) (top[0].getY() / scale);
        // Upper right
        x2 = (int) (top[1].getX() / scale);
        y2 = (int) (top[1].getY() / scale);
        // Lower left
        x3 = (int) (bottom[0].getX() / scale);
        y3 = (int) (bottom[0].getY() / scale);
        // Lower right
        x4 = (int) (bottom[1].getX() / scale);
        y4 = (int) (bottom[1].getY() / scale);
    }

    
    
    public void setColor(Color a){}
    public Color getColor(){return null;}

    
    public Point[] getHandleLocations() { return new Point[] {
            new Point(x1, y1),
            new Point(x2 ,y2),
            new Point(x3, y3),
            new Point(x4, y4)
        };
    }

    public int getID() { return id; }

    public Shape getShape() {
        GeneralPath a = new GeneralPath();
        // Start in upper left corner, proceed in a clockwise manner
        a.moveTo(x1, y1);
        // upper right
        a.lineTo(x2, y2);
        // lower right
        a.lineTo(x4, y4);
        // lower left
        a.lineTo(x3, y3);
        // back to upper left to complete polygon
        a.lineTo(x1, y1);
        return a;
    }



    public String getLabel() {
        return "Subclass of Quadrilatera Field Object should implement " + 
            "this method, not me";
    }

    public void move(int dx, int dy) {
        x1 += dx; y1 += dy;
        x2 += dx; y2 += dy;
        x3 += dx; y3 += dy;
        x4 += dx; y4 += dy;
    }

    public void setID(int x) {
        id = x;
    }

    public void scale(double dx, double dy) {}


    /**
     * Check if a small box centered around (x,y) intersects the shape 
     * of our region.
     */
    public boolean contains(int x, int y) {
        Rectangle boundingBox = new Rectangle(x - MOUSE_OVER_SIZE / 2,
                                              y - MOUSE_OVER_SIZE / 2,
                                              MOUSE_OVER_SIZE,
                                              MOUSE_OVER_SIZE);
        return getShape().intersects(boundingBox);
    }

}  
