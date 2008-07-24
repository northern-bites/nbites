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


public class Squiggle extends AbstractDefinableShape {

    private GeneralPath path;

    private boolean finished;

    public void draw(Graphics g) {
        if (path == null) { return; }
        saveOldSettings(g);

        Graphics2D g2 = (Graphics2D) g;
        
        g2.setStroke(getStroke());
        g2.setColor(getOutlineColor());

        g2.draw(path);
        if (isFilled()) {
            g2.setColor(getFillColor());
            g2.fill(path);
        }
        restoreOldSettings(g);
    }


    public void mousePressed(MouseEvent e) {
        if (path == null) {
            path = new GeneralPath();
            path.moveTo((int) e.getX(), (int) e.getY());
        }
        
        
    }

    public void mouseReleased(MouseEvent e) {
        finished = true;
        //path.closePath();
    }

    public void mouseDragged(MouseEvent e) {
        if (!finished) {
            // Add the point to the path
            path.lineTo((int) e.getX(), (int) e.getY());
        }
    }


    public Point[] getSignificantPoints() {
        return null;
    }

    public boolean isFinished() { return finished; }

    public java.awt.Rectangle getRefreshArea() { return null; }

    public Shape getShape() { return path; }

}