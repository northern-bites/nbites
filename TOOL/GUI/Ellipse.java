package edu.bowdoin.robocup.TOOL.GUI;

import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Stroke;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Shape;
import java.awt.event.MouseEvent;
import java.awt.Point;
import java.lang.Math;

import java.awt.geom.Ellipse2D;

public class Ellipse extends RectangularShape  {

   
    public void drawRectangularShape(Graphics2D g2, int x1, int y1, int w, 
                                     int h) {
        g2.drawOval(x1, y1, w, h);
    }

    public void fillRectangularShape(Graphics2D g2, int x1, int y1, int w, 
                                     int h) {
        g2.fillOval(x1, y1, w, h);
    }
    

    public Shape getShape() {/*
        double x1, y1, x2, y2;
        x1 = Math.min(upperLeft
                             */
        return new Ellipse2D.Double(upperLeft.getX(), upperLeft.getY(),
                                    lowerRight.getX() - upperLeft.getX(),
                                    lowerRight.getY() - upperLeft.getY());
    }
}