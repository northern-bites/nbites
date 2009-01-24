package TOOL.Classifier;


import TOOL.Misc.Pair;
import TOOL.GUI.Movable;
import TOOL.GUI.ID_Changeable;
import TOOL.GUI.Resizable;
import TOOL.GUI.Handle;
import TOOL.GUI.DefinableShape;

import java.awt.Color;
import java.awt.Point;
import java.awt.Shape;
import java.awt.BasicStroke;
import java.awt.geom.GeneralPath;
import java.awt.Stroke;
import java.awt.Point;
import java.awt.Graphics2D;
import java.awt.Composite;
import java.awt.AlphaComposite;
import java.awt.Rectangle;
import java.awt.Font;
import java.awt.FontMetrics;
import java.util.Comparator;

import javax.swing.*;

import java.util.HashMap;
import java.awt.geom.Rectangle2D;

public abstract class ClassifierObject implements Movable, ID_Changeable,
                                                  Resizable
{

    public static final int MOUSE_OVER_SIZE = 2;
    
    public static final float OPAQUE = 1.0F;
    public static final float GHOST_TRANSPARENCY = 0.5F;

    public static final float UNSELECTED_LINE_WIDTH = 1;
    public static final float SELECTED_LINE_WIDTH = 2.0F;

    public static final Comparator<Point> Y_Order = 
        new Comparator<Point>() {
        public int compare(Point a, Point b) {
            return (int)(a.getY() - b.getY());
        }
    };
    
    public static final Comparator<Point> X_Order = 
        new Comparator<Point>() {
        public int compare(Point a, Point b) {
            return (int)(a.getX() - b.getX());
        }
    };
    

    // Ball, Goal, Beacon, Line, Center circle, Corner, CornerArc
    public static final int NUM_OBJECTS = 7;
  

    //    protected static String[] IDS;
    protected static HashMap<String, Integer> stringToID;

    // What color to draw it on the screen
    protected Color color;
    protected int id;
    protected boolean occluded;
    

    

    public ClassifierObject() {
        stringToID = setupHashMap();
    }
    
    // Extracts the (x,y) location of various points of the classifier object
    // based on the shape passed into it.  Note that we convert from screen
    // coordinates to image coordinates by use of scale
    public abstract void getPointsFromShape(DefinableShape p, double scale);

    
    protected static HashMap<String, Integer> setupHashMap() {
        HashMap<String, Integer> map = new HashMap<String, Integer>();
        String[] labels = getLabelArray();
        for (int i = 0; i < labels.length; i++) {
            map.put(labels[i], i);
        }
        return map;
    }

    public static int getIDFromString(String s) {
        
        return 0;
        
    }
    
    /*
    public static int getIDFromString(String s) {
        Integer g = stringToID.get(s);
        if (g == null) {
            return -1;
        }
        else {
            return g.intValue();
        }
        }*/


    // What color should be drawn on classifier screen
    public abstract Color getColor();
    
    public abstract void setColor(Color c);
    
    public abstract String getLabel();

    public abstract void move(int dx, int dy);

    public abstract void scale(double dx, double dy);
        
    public abstract Shape getShape();

    public abstract Point[] getHandleLocations();

    

    /** 
     * By default, just check whether the shape representation of the object
     * contains the point.  Subclasses may wish to override this method,
     * particularly lines (since the Java Line implementation always returns
     * false for containment)
     */
    public boolean contains(int x, int y) {
        return getShape().contains(x, y);
    }

    /** 
     * By default, just check whether the shape representation of the object
     * contains the point.  Subclasses may wish to override this method,
     * particularly lines (since the Java Line implementation always returns
     * false for containment) 
     */
    public boolean contains(Point p) {
        return contains((int) p.getX(), (int) p.getY());
    }

    /**
     * @return true if and only if the shape of the classified object intersects
     * the rectangular region specified by r.
     * Used to determine when an object is selected
     */
    public boolean intersects(Rectangle2D r) {
        return getShape().intersects(r);
    }

    /**
     * @return true if and only if the shape of the classified object intersects
     * the rectangular region specified by specified by the arguments.
     * Used to determine when an object is selected
     */
    public boolean intersects(double x, double y, double w, double h) {
        return getShape().intersects(x,y,w,h);
    }


    public void setOccluded(boolean x) {
    	occluded = x;
    }
    
    public boolean getOccluded() {
    	return occluded;
    }
    
    public int getID() {
        return id;
    }

 
    public void setID(int x) {
        id = x;
    }

    public void drawUnselected(Graphics2D canvas) {
        draw(canvas, UNSELECTED_LINE_WIDTH, OPAQUE);
    }

    public void drawSelected(Graphics2D canvas) {
        draw(canvas, SELECTED_LINE_WIDTH, OPAQUE);
    }

    public void drawGhosted(Graphics2D canvas) {
        draw(canvas, UNSELECTED_LINE_WIDTH, GHOST_TRANSPARENCY);
    }

    /**
     * Draws a representation of the ClassifierObject to the graphics
     * context provided.  In this default implementation, merely draws
     * the shape provided by the getShape() method in the color provided by
     * the getColor() method.
     *
     * Subclasses wishing for a different onscreen representation should
     * override this method.
     * @param canvas where to draw the image
     * @param lineWidth the width of the line, in pixels
     * @param alpha a number between 0.0F (completely transparent) and 1.0F
     *        (completely opaque)
     */
    public void draw(Graphics2D canvas, float lineWidth, float alpha) {
        // Save the previous stroke and color to restore
        Stroke oldStroke = canvas.getStroke();
        Color oldColor = canvas.getColor();
        Composite oldOpacity = canvas.getComposite();
        
        
        // Draw the shape in specified width
        canvas.setColor(getColor());
        canvas.setStroke(new BasicStroke(lineWidth));
        canvas.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER,
                                                       alpha));
        canvas.draw(getShape());
        
        // Draw handles
        for (Point p : getHandleLocations()) {
             Handle h = new Handle(p);
            canvas.draw(h.getShape());
        }

       


        // Restore old values
        canvas.setColor(oldColor);
        canvas.setStroke(oldStroke);
        canvas.setComposite(oldOpacity);
    }

    
    public static String[] getLabelArray() { 
        return new String[] {"You should not use this method unless you are a subclass."}; 
    }

    public void resize() {}

    
    /*
    private static ClassifierObject createQuadrilateralObject(ClassifierModel.ClassifierMode mode, int id, DefinableShape p, double scale) {
        switch (mode) {
        default:
            System.err.println("In createQuadrilateralObject(): " + mode + 
                               " is not acceptable");
            return null;
        }
    }

    private static ClassifierObject createRectangularObject(ClassifierModel.ClassifierMode mode, int id, DefinableShape p, double scale) {
        
        Point[] points;

        
        switch (mode) {
        case BLUE_YELLOW_BEACON:
        case YELLOW_BLUE_BEACON:
            
            

        case YELLOW_GOAL:
        case BLUE_GOAL:

            
        default:
            
            System.err.println("In createRectangularObject(): " + mode + 
                               " is not acceptable");
            return null;

        }
    }

    private static ClassifierObject createCorner(ClassifierModel.ClassifierMode mode, int id, DefinableShape p, double scale) {
        switch (mode) {
        default:
            System.err.println("In createRectangularObject(): " + mode + 
                               " is not acceptable");
            return null;
        }
    }


    private static ClassifierObject createLine(ClassifierModel.ClassifierMode mode, int id, DefinableShape p, double scale) {
        switch (mode) {
        default:
            System.err.println("In createRectangularObject(): " + mode + 
                               " is not acceptable");
            return null;
        }

    }
    
    private static ClassifierObject createArc(ClassifierModel.ClassifierMode mode, int id, DefinableShape p, double scale) {
        switch (mode) {
            if (!(s instanceof Arc)) {
                TOOL.TOOL.CONSOLE.error("Error: Tried to pass a non point shape "+
                                    "into the corner method..");
                return null;
            }

            points = s.getSignificantPoints();
        
            int x1 = (int) (p[0].getX() / scale);
            int y1 = (int) (p[0].getY() / scale);
            int x2 = (int) (p[1].getX() / scale);
            int y2 = (int) (p[1].getY() / scale);
            int radiusX = (int) (p[2].getX() / scale);
            int radiusY = (int) (p[2].getY() / scale);
            
            return new CornerArc(id, x1, y1, x2, y2, radiusX, radiusY)
                
        default:
            System.err.println("In createRectangularObject(): " + mode + 
                               " is not acceptable");
            return null;
        }
    }
    
    
    public static ClassifierObject createObject(ClassifierModel.ClassifierMode mode,
                                                int id,
                                                DefinableShape p, 
                                                double scale) {
        
        switch (mode) {
        case BLUE_YELLOW_BEACON:
        case YELLOW_BLUE_BEACON:
        case YELLOW_GOAL:
        case BLUE_GOAL:
            return createQuadrilateralObject(mode, id, p, scale);

        case T_CORNER:
        case INNER_L_CORNER:
        case OUTER_L_CORNER:
            return createCorner(mode, id, p, scale);
                        

        case BALL:
        case CENTER_CIRCLE:
            return createRectangularObject(mode, id, p, scale);
        
        case BLUE_CORNER_ARC:
        case YELLOW_CORNER_ARC:
            return createArc(mode, id, p, scale);

        case LINE:
            return createLine(mode, id, p, scale);
        default:
            System.err.println("Error: Unrecognized mode in createObject()");
            return null;
        }

        }*/
    


    public static ClassifierObject createObject(Classifier.ClassMode mode,
                                                int globalID,Pair<Point,Point>
                                                line1, Pair<Point,Point> line2){

        
        int x1,x2,x3,x4;
        int y1,y2,y3,y4;
        x1 = (int)line1.getFirst().getX();  y1 = (int)line1.getFirst().getY();
        x2 = (int)line1.getSecond().getX(); y2 = (int)line1.getFirst().getY();
        x3 = (int)line2.getFirst().getX();  y3 = (int)line2.getFirst().getY();
        x4 = (int)line2.getSecond().getX(); y4 = (int)line2.getFirst().getY();

        // There are three valid ways in which the user can define the 
        // rectangle:
        // Drawing the parallel lines along the sides
        // Drawing the parallel lines along the top
        // Drawing the diagonals (that is, a line from the top left corner
        // to bottom right corner, and from top right to bottom left).  
        

        // sort to determine the top and bottom pairs, then sort to determine
        // left and right
        Point[] points = new Point[] {line1.getFirst(),
                                     line1.getSecond(),
                                     line2.getFirst(),
                                     line2.getSecond()};
        java.util.Arrays.sort(points, Y_Order);
        
        Point[] top = new Point[]{points[0], points[1]};
        Point[] bottom = new Point[]{points[2],points[3]};
        java.util.Arrays.sort(top, X_Order);
        java.util.Arrays.sort(bottom, X_Order);
        
        switch (mode) {
                
        case BEACON:
            return new Beacon(globalID, top[0], top[1], bottom[0], bottom[1]);
        case BLUE_GOAL:
        case YELLOW_GOAL:
            return new Goal(globalID, top[0], top[1], bottom[0], bottom[1]);
        
        default:
            System.err.println("Bad argument in addRectangularObject(); " +
                               "enum was not recognized");
            return null;
        }
    }

    

    public static ClassifierObject createObject(Classifier.ClassMode mode,
                                                int globalID,
                                                int x1, int y1, 
                                                int x2, int y2,
                                                int x3, int y3) {
           
        switch (mode) {
        case BALL:
            return new Ball(globalID, Math.min(x1, x2), // upper left x
                           Math.min(y1, y2), // upper left y coord
                           Math.abs(x2-x1),  // width
                           Math.abs(y2-y1)); // height


           
        case CENTER_CIRCLE:
            return new CenterCircle(globalID, Math.min(x1, x2),
                                   Math.min(y1, y2), 
                                   Math.abs(x2-x1),  // width
                                   Math.abs(y2-y1)); // height
        case LINE:
            return new Line(globalID, x1, y1, x2, y2);
                
        case BLUE_CORNER_ARC:
        case YELLOW_CORNER_ARC:
            return new CornerArc(globalID, x1, y1, x2, y2, x3, y3);
            

        
        case L_CORNER:
            return new Corner(globalID, Corner.L, x2, y2);
        case T_CORNER:
            return new Corner(globalID, Corner.T, x2, y2);
        default:
            System.err.println("Bad argument in createObject(); enum was "+
                               "not recognized");
        }
        return null;

    }

    public String toLog() {
    	return "[ UNDEFINED object info ( "+getID()+" ) ]";
    }

}
