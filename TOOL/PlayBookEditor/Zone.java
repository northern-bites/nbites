package TOOL.PlayBookEditor;

import java.awt.Point;
import java.awt.Shape;
import java.awt.Color;
import java.awt.Rectangle;
import java.lang.Math;
import TOOL.Data.Field;
import TOOL.Image.PlayBookEditorViewer;

/**
 * A zone represents an area on the field which we classify.
 *
 * @author Nicholas Dunn
 */
public class Zone extends PlayBookObject {

    /**
     * Constructor for a zone.
     *
     * NOTE: The convention is that upper left refers to upper left in
     * FIELD coordinates, NOT screen coordinates.  Thus the upper left in field
     * coordinates will actually be the lower left in screen coordinates
     *
     *
     * @param id the identifier for the zone; will be used to link regions
     * together which share a common strategy
     * @param x1 upper left corner x
     * @param y1 upper left corner y
     * @param x2 lower right corner x
     * @param y2 lower right corner y
     * @param field the Field object we use for converting from field
     * coordinates into image coordinates in the getShape() method.
     */
    public Zone(String id, int x1, int y1, int x2, int y2, Field field) {
        super(id, x1, y1, x2, y2, field);
    }

    /**
     * Ensures that the zone lies firmly within the bounds of the Field
     * instance passed into it.
     */
    protected void fixBounds() {
        // Too far to the left of the screen
        if (x1 < 0) {
            x2 -= x1;
            x1 = 0;
        }
        // Too far to the right of the screen
        else if (x2 > field.FIELD_WIDTH) {
            x1 -= (x2 - (int)field.FIELD_WIDTH);
            x2 = (int)field.FIELD_WIDTH;
        }
        // Too high
        if (y1 > field.FIELD_HEIGHT) {
            y2 -= (y1 - (int)field.FIELD_HEIGHT);
            y1 = (int)field.FIELD_HEIGHT;
        }
        // Too low
        else if (y2 < 0) {
            y1 -= y2;
            y2 = 0;
        }
    }

    /** 
     * Returns a shape whose coordinates have been translated from
     * the field coordinate system (0,0 is bottom left corner)
     * to image coordinates (0,0 is upper left corner)
     */ 
    public Shape getShape() {
        // Convert from field coordinates to image coordinates
        int sy1 = field.fieldToScreenY(y1);
        int sy2 = field.fieldToScreenY(y2);
        /*                   
                             System.out.println("In field coordinates, creating a new rectangle at: (" + x1 + ", " + y1 + "), (" + x2 + ", " + y2 + ")");*/
        return new Rectangle(Math.min(x1, x2), Math.min(sy1, sy2),
                             Math.abs(x2 - x1), Math.abs(sy2 - sy1));
    }

    public Point getCurLoc(int dir) {
        switch (dir) {
        case N:
            return new Point(0, y1);
        case S:
            return new Point(0, y2);
        case W:
            return new Point(x1, 0);
        case E:
            return new Point(x2, 0);
        case NE:
            return new Point(x2, y1);
        case NW:
            return new Point(x1, y1);
        case SW:
            return new Point(x1, y2);
        case SE:
            return new Point(x2, y2);
        default:
            System.err.println("You should never get this message.");
            return null;
        }
    }

    public boolean contains(int x, int y) {
        return getShape().contains(x, y);
    }

    /** @return a copy of the zone with all instance variables copied */
    public Zone clone() {
        return new Zone(id, x1, y1, x2, y2, field);
    }

    public Color getColor() { return Color.ORANGE; }


    /* Setter methods*/
    public void setID(String i) {id = i;}

}