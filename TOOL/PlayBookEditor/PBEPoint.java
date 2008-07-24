package TOOL.PlayBookEditor;

import TOOL.Data.Field;
import java.awt.Point;
import java.awt.Color;
import java.awt.Shape;
import java.awt.geom.Ellipse2D;
import java.awt.Rectangle;

/**
 * A line along which robots travel
 *
 * @author Nicholas Dunn
 * @date March 29, 2008
 */
public class PBEPoint extends PlayBookObject {

    private int RADIUS = 12;

    public PBEPoint(String _id, int _x, int _y, Field _field) {
        super(_id, _x, _y, _x, _y, _field);
    }

    public PBEPoint clone() {
        return new PBEPoint(id, x1, y1, field);
    }

    public Color getColor() { return Color.BLACK; }

    public Shape getShape() {
        // Convert from field coordinates to image coordinates
        int sx = field.fieldToScreenX(x1);
        int sy = field.fieldToScreenY(y1);
        return new Ellipse2D.Float(sx-RADIUS/2, sy-RADIUS/2, RADIUS, RADIUS);
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