package edu.bowdoin.robocup.TOOL.GUI;

import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Stroke;
import java.awt.BasicStroke;

import java.awt.geom.Line2D;

import java.awt.Color;
import java.awt.Rectangle;
import java.awt.Shape;
import java.awt.event.MouseEvent;
import java.awt.Point;

public class Line extends TwoPointDefinable {
    
    public void drawShape(Graphics2D g2, int x1, int y1, int x2, int y2) {
        g2.drawLine(x1, y1, x2, y2);
    }

    // Filling a line makes no sense 
    public void fillShape(Graphics2D g2, int x1, int y1, int x2, int y2) {
        
    }
    
    public Shape getShape() {
        return new Line2D.Float((int) upperLeft.getX(), (int) upperLeft.getY(),
                                (int) lowerRight.getX(), 
                                (int) lowerRight.getY());
    }

    public Point[] getSignificantPoints() { 
        return new Point[] {initialPoint, curPoint};
    }


}