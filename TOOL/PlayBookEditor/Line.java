package edu.bowdoin.robocup.TOOL.PlayBookEditor;

import edu.bowdoin.robocup.TOOL.Data.Field;
import java.awt.Point;
import java.awt.Color;
import java.awt.Shape;
import java.awt.geom.Line2D;
import java.awt.Rectangle;

/**
 * A line along which robots travel
 *
 * @author Nicholas Dunn
 * @date March 29, 2008
 */
public class Line extends PlayBookObject {

    public Line(String _id, int _x1, int _y1, int _x2, int _y2, Field _field) {
        super(_id, _x1, _y1, _x2, _y2, _field);
    }


    public Line clone() {
        return new Line(id, x1, y1, x2, y2, field);
    }

    public Color getColor() { return Color.BLACK; }

    public Shape getShape() {
        // Convert from field coordinates to image coordinates
        int sy1 = field.fieldToScreenY(y1);
        int sy2 = field.fieldToScreenY(y2);
        return new Line2D.Double(x1,sy1,x2,sy2);
    }

    /**
     * We make a small box around the location they passed in, and then check
     * whether that intersects our line.
     *
     */
    public boolean contains (int x, int y) {
        Rectangle boundingBox = new Rectangle(x - 2,
                                              y - 2,
                                              4, 4);
        return getShape().intersects(boundingBox);
    }

    /**
     * When drawing a horizontal or vertical line, either width or height will
     * be 0; it is still a valid line.
     */
    public static boolean validSize(int width, int height) {
        return width >= MIN_WIDTH || height >= MIN_HEIGHT;
    }
}