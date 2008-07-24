package TOOL.GUI;

import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Stroke;
import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Shape;
import java.awt.event.MouseEvent;
import java.awt.Point;
import java.lang.Math;

import java.awt.geom.Rectangle2D;

public class Rectangle extends RectangularShape  {

   
    public void drawRectangularShape(Graphics2D g2, int x1, int y1,
                                     int w, int h) {
        g2.drawRect(x1, y1, w, h);
    }

    
    public void fillRectangularShape(Graphics2D g2, int x1, int y1,
                                     int w, int h) {
        g2.fillRect(x1, y1, w, h);
    }
    
    public Shape getShape() {
        return new Rectangle2D.Double(upperLeft.getX(),
                                      upperLeft.getY(),
                                      lowerRight.getX() - upperLeft.getX(),
                                      lowerRight.getY() - upperLeft.getY());
    }

}