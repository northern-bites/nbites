package edu.bowdoin.robocup.TOOL.GUI;

import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Stroke;
import java.awt.BasicStroke;

import java.awt.Color;
import java.awt.geom.GeneralPath;
import java.awt.Rectangle;
import java.awt.Shape;
import java.awt.event.MouseEvent;
import java.awt.Point;

public class Quadrilateral extends AbstractDefinableShape {
    
    private Point p1, p2, p3, p4;

    private Point curPoint;
    
    private boolean firstLineDefined, secondLineDefined, finished;
    private GeneralPath outline;

    public void draw(Graphics g) {
        if (p1 == null) {
            return; 
        }
        saveOldSettings(g);

        Graphics2D g2 = (Graphics2D) g;
        
        g2.setStroke(getStroke());
        g2.setColor(g2.getColor());

        // TODO:Make this not ugly as shit


        // The whole quadrilateral is defined
        if (secondLineDefined) {
            outline.reset();
            outline.moveTo((int)p1.getX(),
                           (int)p1.getY());
            outline.lineTo((int)p2.getX(),
                           (int)p2.getY());
            outline.moveTo((int)p3.getX(),
                           (int)p3.getY());
            outline.lineTo((int)p4.getX(),
                           (int)p4.getY());
            g2.draw(outline);
   
        }
        // Just the first line is defined and we're drawing the second
        else if (firstLineDefined) {
            outline.reset();
            outline.moveTo((int)p1.getX(),
                           (int)p1.getY());
            outline.lineTo((int)p2.getX(),
                           (int)p2.getY());
            if (p3 != null) {
                outline.moveTo((int)p3.getX(),
                               (int)p3.getY());
                outline.lineTo((int)curPoint.getX(),
                               (int)curPoint.getY());
            }
            g2.draw(outline);
        }
        // We're drawing the first line
        else {
            g2.drawLine((int)p1.getX(), 
                        (int)p1.getY(), 
                        (int)curPoint.getX(), 
                        (int)curPoint.getY());
        }
        

        restoreOldSettings(g);
    }


    public void reset() {
        p1 = p2 = p3 = p4 = null;
        curPoint = null;
        firstLineDefined = secondLineDefined = finished = false;
        outline = null;
    }

    public void mousePressed(MouseEvent e) {
        if (!firstLineDefined) {
            p1 = e.getPoint();
            curPoint = e.getPoint();
        }
        else if (!secondLineDefined) {
            p3 = e.getPoint();
            curPoint = e.getPoint();
        }
        // We're done.. do nothing
        else {

        }
    }

    public void mouseDragged(MouseEvent e) {
        curPoint = e.getPoint();
    }

    public void mouseReleased(MouseEvent e) {
        if (!firstLineDefined) {
            p2 = e.getPoint();
            
            outline = new GeneralPath();
            firstLineDefined = true;
        }
        else if (!secondLineDefined) {
            p4 = e.getPoint();
            secondLineDefined = true;
            finished = true;
        }
        // We're already done, do nothing
        else {

        }
    }

    public Point[] getSignificantPoints() {
        return new Point[] {p1, p2, p3, p4};
    }

    public boolean isFinished() { return finished; }

    public java.awt.Rectangle getRefreshArea() { return null; }

    public Shape getShape() { return outline; }
}