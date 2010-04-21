package TOOL.GUI;

import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Stroke;
import java.awt.BasicStroke;

import java.awt.Color;
import java.awt.Rectangle;
import java.awt.Shape;
import java.awt.event.MouseEvent;
import java.awt.Point;



public abstract class AbstractDefinableShape implements DefinableShape {

    private boolean proportionsConstrained;
    private boolean radiatingFromCenter;
    private boolean filled;
    private Stroke stroke = DEFAULT_STROKE;
    private Color outlineColor = DEFAULT_OUTLINE_COLOR;
    private Color fillColor = DEFAULT_FILL_COLOR;
        
    private static final Stroke DEFAULT_STROKE = new BasicStroke(1);
    private static final Color DEFAULT_OUTLINE_COLOR = Color.BLACK;
    private static final Color DEFAULT_FILL_COLOR = Color.RED;


    private Stroke oldGraphicsStroke;
    private Color oldGraphicsColor;

    public abstract void draw(Graphics g);

    public abstract void mousePressed(MouseEvent e);

    public abstract void mouseDragged(MouseEvent e);

    public abstract void mouseReleased(MouseEvent e);
    
    public void reset() {
        /*setStroke(DEFAULT_STROKE);
        setOutlineColor(DEFAULT_OUTLINE_COLOR);
        setFillColor(DEFAULT_FILL_COLOR);
        setFilled(false);*/
    }

    
    ////////////////////////////////////////////////////////////
    // Setters
    ////////////////////////////////////////////////////////////
    
    public void setConstrainProportions(boolean b) {
        proportionsConstrained = b;
    }

    public void setRadiatesFromCenter(boolean b) {
        radiatingFromCenter = b;
    }

    public void setOutlineColor(Color c) {
        outlineColor = c;
    }

    public void setFillColor(Color c) {
        fillColor = c;
    }

    public void setFilled(boolean b) {
        filled = b;
    }

    public void setStroke(Stroke s) {
        stroke = s;
    }

    public void setLineWidth(float w) {
        setStroke(new BasicStroke(w));
    }

    ////////////////////////////////////////////////////////////
    // Getters
    ////////////////////////////////////////////////////////////
    
    public abstract java.awt.Rectangle getRefreshArea();
        
    public boolean proportionsConstrained() {
        return proportionsConstrained;
    }

    public boolean radiatesFromCenter() {
        return radiatingFromCenter;
    }
    
    public abstract boolean isFinished();

    public boolean isFilled() {
        return filled;
    }

    public Color getFillColor() {
        return fillColor;
    }

    public Color getOutlineColor() {
        return outlineColor;
    }

    public abstract Shape getShape();

    public Stroke getStroke() { 
        return stroke;
    }
    
    public abstract Point[] getSignificantPoints();

    public double getWidth() {
        return getShape().getBounds().getWidth();
    }

    public double getHeight() {
        return getShape().getBounds().getHeight();
    }


    // By default return the length of the diagonal of the rectangle enclosing
    // the shape; subclasses can overide if they wish
    public double getSize() {
        java.awt.Rectangle rect = getShape().getBounds();
        double width = rect.getWidth();
        double height = rect.getHeight();
        double diagonal = Math.sqrt(width * width + height * height);
        return diagonal;
    }

    ////////////////////////////////////////////////////////////
    // Misc helper functions
    ////////////////////////////////////////////////////////////

    public void saveOldSettings(Graphics g) {
        Graphics2D g2 = (Graphics2D) g;
        oldGraphicsStroke = g2.getStroke();
        oldGraphicsColor = g2.getColor();
    }

    
    public void restoreOldSettings(Graphics g) {
        Graphics2D g2 = (Graphics2D) g;
        g2.setStroke(oldGraphicsStroke);
        g2.setColor(oldGraphicsColor);
    }

    


}
