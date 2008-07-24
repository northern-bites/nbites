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

public class CrossPoint extends AbstractDefinableShape {
    
    private Point p1;
    private boolean finished;
    private int length;
    private GeneralPath shape;
    
    public CrossPoint(int length) {
        this.length = length;
    }

    public void draw(Graphics g) {
        if (p1 == null) { return; }

        saveOldSettings(g);
        
        Graphics2D g2 = (Graphics2D) g;
        
        g2.setStroke(getStroke());
        g2.setColor(g2.getColor());

        int x = (int) p1.getX();
        int y = (int) p1.getY();

        shape.reset();
        // Horizontal line segment
        shape.moveTo(x-length/2,y);
        shape.lineTo(x+length/2,y);
        // Vertical line segment
        shape.moveTo(x,y-length/2);
        shape.lineTo(x,y+length/2);
        
        g2.draw(shape);

        restoreOldSettings(g);
    }

    public void reset() {
        super.reset();
        p1 = null;
        finished = false;
        shape = null;

    }

    public void mousePressed(MouseEvent e) {
        shape = new GeneralPath();
        p1 = e.getPoint();
    }

    public void mouseDragged(MouseEvent e) {
        p1 = e.getPoint();
    }

    public void mouseReleased(MouseEvent e) {
        p1 = e.getPoint();
        finished = true;
    }

    public Point[] getSignificantPoints() { 
        
        return new Point[] {p1}; 
    }

    public boolean isFinished() {
        return finished;
    }

    public java.awt.Rectangle getRefreshArea() { return null; }

    public Shape getShape() { return shape; }
}