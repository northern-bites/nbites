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

public abstract class TwoPointDefinable extends AbstractDefinableShape {
    
    protected Point initialPoint;
    protected Point curPoint;
    protected Point upperLeft, lowerRight;
    protected boolean finished;

    public void draw(Graphics g) {
        if (initialPoint == null) { return; }

        saveOldSettings(g);

        Graphics2D g2 = (Graphics2D) g;
        
        int x1 = (int) initialPoint.getX();
        int y1 = (int) initialPoint.getY();

        int x2 = (int) curPoint.getX();
        int y2 = (int) curPoint.getY();
        
        if (proportionsConstrained()) {
            if (Math.abs(x2 - x1) > Math.abs(y2 - y1)) {
                y2 = ((y2 > y1) ?
                      y1 + Math.abs(x2 - x1) : // if y2 is below y1, y2 = ...
                      y1 - Math.abs(x2 - x1)); // else y2 is above y1..
            }
            // Other way around, make x bigger to compensate
            else {
                x2 = ((x2 > x1) ? 
                      x1 + Math.abs(y2-y1) : // x2 is to the right of x1..
                      x1 - Math.abs(y2-y1)); // x2 is to the left
            }

        }
        if (radiatesFromCenter()) {
            // x1 = x1 - (x2 - x1) = x1 - x2 + x1 = 2 * x1 - x2
            x1 = 2 * x1 - x2;
            y1 = 2 * y1 - y2;
        }

        g2.setStroke(getStroke());
        
        if (isFilled()) {
            g2.setColor(getFillColor());
            fillShape(g2, x1, y1, x2, y2);
        }

        g2.setColor(getOutlineColor());
        drawShape(g2, x1, y1, x2, y2);
        restoreOldSettings(g);

        upperLeft.setLocation(Math.min(x1, x2),
                              Math.min(y1, y2));
        lowerRight.setLocation(Math.max(x1, x2),
                               Math.max(y1, y2));
        /*
        upperLeft.setLocation(x1, y1);
        lowerRight.setLocation(x2, y2);*/
    }


    public void reset() {
        super.reset();
        finished = false;
        initialPoint = curPoint = null;
    }

    public abstract void drawShape(Graphics2D g2, int x1, int y1, int x2, int y2);

    public abstract void fillShape(Graphics2D g2, int x1, int y1, int x2, int y2);
    

     /**
     * Upon a mouse press, save the point of the press as both the
     * initial point and the current point
     */
    public void mousePressed(MouseEvent e) {
        initialPoint = e.getPoint();
        curPoint = e.getPoint();

        upperLeft = new Point();
        lowerRight = new Point();
        /*
        lastSize = new java.awt.Rectangle((int)initialPoint.getX(), 
                                          (int)initialPoint.getY(),
                                          1, 1);
                                          curSize = lastSize;*/
    }

    public void mouseDragged(MouseEvent e) {
        curPoint = e.getPoint();
    }

    public void mouseReleased(MouseEvent e) {
        finished = true;
    }

    public java.awt.Rectangle getRefreshArea() { return null; }
    
    // Return the upper left and lower right point
    public Point[] getSignificantPoints() { 
        return new Point[] {upperLeft, lowerRight};
    }

    public boolean isFinished() { return finished; }

    public abstract Shape getShape();
}