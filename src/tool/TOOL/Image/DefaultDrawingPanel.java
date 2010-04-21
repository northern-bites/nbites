package TOOL.Image;

import javax.swing.*;
import java.awt.event.*;

import java.awt.geom.*;
import java.io.*;
import java.awt.Shape;
import java.awt.Color;
import java.awt.Graphics;


import TOOL.GUI.*;

import TOOL.TOOL;

/**
 * A class that allows the user to draw on the screen using a few different 
 * shapes.  There is no persistence in this class; as soon as a shape is
 * finished being drawn and a new one started, the old one will disappear.
 * Subclasses should include some sort of saving mechanism if they want
 * persistence.
 * Note that the definable objects take care of how they deal with mouse
 * drags etc.
 */
public class DefaultDrawingPanel extends ImagePanel 
    implements MouseListener, MouseMotionListener{

    // Number of pixels wide a cross point is
    public static final int POINT_SIZE = 10;

    // Enumeration holds the different drawing modes we support currently
    protected enum DrawingMode {
        SELECTION_RECT(new SelectionRectangle()), 
            ARC(new Arc()),
            LINE(new Line()), 
            ELLIPSE(new Ellipse()), 
            RECTANGLE(new Rectangle()), 
            POINT(new CrossPoint(POINT_SIZE)),
            QUADRILATERAL(new Quadrilateral()),
            NONE(new EmptyShape());
        DefinableShape shape;
        DrawingMode(DefinableShape shape) {
            this.shape = shape;
        }
        public DefinableShape getShape() {
            return shape;
        }
    };
    protected DrawingMode drawMode;

    protected boolean drawing;
    protected boolean filled;
    protected Color outlineColor, fillColor;
    
    protected DefinableShape curShape;

    public DefaultDrawingPanel() {
        super();
        
        drawing = false;
        addMouseMotionListener(this);

        outlineColor = Color.BLACK;
        filled = false;
        
        // Default to selecting things
        drawMode = DrawingMode.SELECTION_RECT;
        curShape = drawMode.getShape();
    }
    
    /**
     * @return the current shape being drawn on the screen
     */ 
    public DefinableShape getShape() {
        return curShape;
    }

    
    
    public boolean isDrawing() {
        return drawing;
    }

    public boolean isFilled() {
        return filled;
    }
    
    public Color getOutlineColor() {
        return outlineColor;
    }

    public Color getFillColor() {
        return fillColor;
    }

    /**
     * @return true iff the current shape has finished being defined.
     * For instance, for an arc that takes two clicks to be defines, any time
     * before the second mouse release, this method will return false
     */
    public boolean shapeDefined() {
        return curShape.isFinished();
    }

    /**
     * Change the current mode of drawing to drawMode.
     */
    public void setMode(DrawingMode drawMode) {
        this.drawMode = drawMode;
        drawing = false;
        curShape = drawMode.getShape();
    }


    public void setDrawing(boolean choice) {
        drawing = choice;
    }

    public void setFilled(boolean choice) {
        filled = choice;
    }


    public void setOutlineColor(Color n) {
        outlineColor = n;
        curShape.setOutlineColor(outlineColor);
    }
    
    public void setFillColor(Color n) {
        fillColor = n;
    }

    public void setLineWidth(float f) {
        curShape.setLineWidth(f);
    }


    public void paint(Graphics g) {
        super.paint(g);
        
        // for those subclasses that need persistence
        drawExistingShapes(g);

        if (drawing && curShape != null) {
            curShape.draw(g);
        }
    }

    /** 
     * For subclasses to implement; if there are existing shapes already
     * on the screen, draw them before we draw the shape.  Otherwise
     * the currently drawn shape will be drawn at the wrong time if
     * a subclass overrides our paint method.  Intentionally blank
     */
    public void drawExistingShapes(Graphics g) {}
        
    
    public void mouseClicked(MouseEvent e) {
    }

    public void mousePressed(MouseEvent e) {
        if (e.isPopupTrigger()) { return; }

        if (curShape.isFinished()) {
            curShape.reset();
        }

        curShape.mousePressed(e);
        drawing = true;
        repaint();
        
    }
    public void mouseReleased(MouseEvent e) {
        if (e.isPopupTrigger()) { return; }

        curShape.mouseReleased(e);
        
        repaint();
    }

    public void clearCurrentlyDrawnShape() {
        drawing = false;
        curShape.reset();
        repaint();
    }

    public void clearCanvas() { }


    public void mouseEntered(MouseEvent e) {}
    public void mouseExited(MouseEvent e) {}

    public void mouseMoved(MouseEvent e) {}
    public void mouseDragged(MouseEvent e) {
        if (e.isPopupTrigger()) { return; }

        // When shift is held down, the object sets its center to be the
        // initial mouse press
        curShape.setRadiatesFromCenter(e.isShiftDown());
        // When alt is held down, proportions are constrained (e.g. a rectangle
        // is forced to be a square, or an ellipse is forced to be a circle)
        curShape.setConstrainProportions(e.isAltDown());
        curShape.mouseDragged(e);
        repaint();

    }

}
