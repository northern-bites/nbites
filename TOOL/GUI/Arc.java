package TOOL.GUI;

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

public class Arc extends AbstractDefinableShape {

    private Point initialPoint;
    private Point endPoint;
    private Point midPoint;
    
    private GeneralPath curve;

    private Point curPoint;

    private boolean finished;

    public void draw(Graphics g) {
        if (initialPoint == null) {
            return;
        }
        saveOldSettings(g);

        Graphics2D g2 = (Graphics2D) g;
        
        
        int x1 = (int) initialPoint.getX();
        int y1 = (int) initialPoint.getY();
        
        g2.setColor(getOutlineColor());
        g2.setStroke(getStroke());

        // We're defining the line, so draw it
        if (endPoint == null) {
            g2.drawLine(x1, y1,
                       (int) curPoint.getX(),
                       (int) curPoint.getY());
        }
        // We've finished drawing the line, show the curve
        else {
            curve.reset();
            curve.moveTo(x1, y1);
            curve.quadTo((float) curPoint.getX(), 
                         (float) curPoint.getY(),
                         (float) endPoint.getX(),
                         (float) endPoint.getY());
            g2.draw(curve);
        }

        restoreOldSettings(g);
    }

    public void reset() {
        super.reset();
        initialPoint = null;
        endPoint = null;
        midPoint = null;
        curve = null;
        curPoint = null;
        finished = false;

    }


    public void mousePressed(MouseEvent e) {
        if (initialPoint == null) {
            initialPoint = e.getPoint();
            curPoint = e.getPoint();
        }
        else {
            curPoint = e.getPoint();
        }
    }

    public void mouseDragged(MouseEvent e) {
        curPoint = e.getPoint();
    }

    public void mouseReleased(MouseEvent e) {
        if (endPoint == null) {
            endPoint = e.getPoint();
            curve = new GeneralPath();
        }
        else {
            midPoint = e.getPoint();
            finished = true;
        }
    }

    public java.awt.Rectangle getRefreshArea() { return null; }


    public Point[] getSignificantPoints() { 
        return new Point[] {initialPoint, endPoint, midPoint};
    }


    public boolean isFinished() { return finished; }

    public Shape getShape() { return curve; }
}