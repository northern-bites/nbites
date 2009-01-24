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
/**
 * A class that does nothing; allows us to ignore mouse drags etc when we want
 * while still having an AbstractDefinableShape
 */
public class EmptyShape extends AbstractDefinableShape {

    public void draw(Graphics g) {}

    public void mousePressed(MouseEvent e) {}

    public void mouseDragged(MouseEvent e) {}

    public void mouseReleased(MouseEvent e) {}

    public java.awt.Rectangle getRefreshArea() { return null; }

    public boolean isFinished() { return true; }

    public Point[] getSignificantPoints() { return null; }

    public Shape getShape() { return null; }
    


}