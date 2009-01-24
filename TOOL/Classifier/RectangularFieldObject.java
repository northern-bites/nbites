package TOOL.Classifier;

import java.awt.Point;
import java.awt.Color;
import java.awt.Shape;
import java.awt.geom.GeneralPath;
import java.awt.Rectangle;

import TOOL.GUI.DefinableShape;
import TOOL.GUI.RectangularShape;
import TOOL.TOOL;

public abstract class RectangularFieldObject extends QuadrilateralFieldObject  {

    
    protected int width, height;

    /**
     * @param id identifier
     * @param x1 x coord of upper left corner
     * @param y1 y coord of upper left corner
     * @param w  width
     * @param h height
     */
    public RectangularFieldObject(int id, int x1, int y1, int w, int h) {
        super(id, 
              // Upper left
              x1, y1,
              // Upper right
              x1 + w, y1,
              // Lower left
              x1, y1 + h,
              // lower right
              x1 + w, y1 + h);
        width = w;
        height = h;
        
    }

    public RectangularFieldObject(int id, DefinableShape s, double scale) {
        super(id,0,0,0,0,0,0,0,0);

        getPointsFromShape(s, scale);
    }

   
    public void getPointsFromShape(DefinableShape s, double scale) {
        if (!(s instanceof RectangularShape)) {
            TOOL.CONSOLE.error("Error: Tried to pass a non quadrilateral "+
                                    "shape into the rectangular method..");
            return;
        }

        Point[] p = s.getSignificantPoints();

        int _x1 = (int) (p[0].getX() / scale);
        int _y1 = (int) (p[0].getY() / scale);
        int _x2 = (int) (p[1].getX() / scale);
        int _y2 = (int) (p[1].getY() / scale);
        
        this.x1 = Math.min(_x1, _x2);
        this.y1 = Math.min(_y1, _y2);

        this.width = Math.abs(_x2 - _x1);
        this.height = Math.abs(_y2 - _y1);

    }

    public Point[] getHandleLocations() {
        return new Point[] {
            // Upper left
            new Point(x1,y1),
            // upper right
            new Point(x1+width,y1),
            // lower right
            new Point(x1+width,y1+height),
            // lower left
            new Point(x1,y1+height),

            // Top
            new Point(x1+width/2, y1)
        };
    }

    public Shape getShape() {
        return new java.awt.geom.Rectangle2D.Double(x1, y1, width, height);
    }

}
