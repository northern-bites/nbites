package TOOL.GUI;

import java.awt.Graphics;
import java.awt.Stroke;
import java.awt.Color;
import java.awt.Rectangle;
import java.awt.Shape;
import java.awt.event.MouseEvent;
import java.awt.Point;

public interface DefinableShape {

    public void draw(Graphics g);

    public void mousePressed(MouseEvent e);

    public void mouseDragged(MouseEvent e);

    public void mouseReleased(MouseEvent e);

    // Resets the object to its state before any drawing was done.
    public void reset();

    
    ////////////////////////////////////////////////////////////
    // Setters
    ////////////////////////////////////////////////////////////
    
    public void setConstrainProportions(boolean b);

    public void setRadiatesFromCenter(boolean b);

    public void setOutlineColor(Color c);

    public void setFillColor(Color c);

    public void setFilled(boolean b);

    public void setStroke(Stroke s);

    /**
     * Sets the stroke of the object to the default BasicStroke with width w.
     * Subclasses can choose to override this default BasicStroke.
     */
    public void setLineWidth(float w);
    ////////////////////////////////////////////////////////////
    // Getters
    ////////////////////////////////////////////////////////////
    
    public Color getFillColor();

    public Color getOutlineColor();

    public Rectangle getRefreshArea();
        
    public boolean proportionsConstrained();

    public boolean radiatesFromCenter();
    
    public boolean isFinished();

    public boolean isFilled();

    public Stroke getStroke();

    public Shape getShape();

    public Point[] getSignificantPoints();

    public double getWidth();
    public double getHeight();
    public double getSize();
}