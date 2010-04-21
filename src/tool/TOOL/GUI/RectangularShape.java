package TOOL.GUI;

import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Stroke;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Shape;
import java.awt.event.MouseEvent;
import java.awt.Point;
import java.lang.Math;

public abstract class RectangularShape extends TwoPointDefinable {

    public void drawShape(Graphics2D g2, int x1, int y1, int x2, int y2) {
        int minX = Math.min(x1, x2);
        int maxX = Math.max(x1, x2);
        int minY = Math.min(y1, y2);
        int maxY = Math.max(y1, y2);
        int width = maxX - minX;
        int height = maxY - minY;
        drawRectangularShape(g2, minX, minY, width, height);
    }

    public void fillShape(Graphics2D g2, int x1, int y1, int x2, int y2) {
        int minX = Math.min(x1, x2);
        int maxX = Math.max(x1, x2);
        int minY = Math.min(y1, y2);
        int maxY = Math.max(y1, y2);
        int width = maxX - minX;
        int height = maxY - minY;
        fillRectangularShape(g2, minX, minY, width, height);
    }
    
    public abstract void drawRectangularShape(Graphics2D g2, int x1, int y1,
                                              int w, int h);

    
    public abstract void fillRectangularShape(Graphics2D g2, int x1, int y1,
                                              int w, int h);


    public abstract Shape getShape() ;
}