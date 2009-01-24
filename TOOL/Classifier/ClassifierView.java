package TOOL.Classifier;

import TOOL.Image.ImagePanel;

import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Cursor;
import java.awt.Point;
import java.awt.Color;
import java.awt.AlphaComposite;
import java.awt.Composite;
import java.awt.BasicStroke;
import java.awt.Font;
import java.awt.FontMetrics;


import java.util.Observer;
import java.util.Observable;

import java.awt.geom.AffineTransform;

import TOOL.Image.DefaultDrawingPanel;

import TOOL.GUI.Rectangle;
import TOOL.GUI.Ellipse;
import TOOL.GUI.Line;
import TOOL.GUI.Arc;
import TOOL.GUI.Quadrilateral;
import TOOL.GUI.Squiggle;
import TOOL.GUI.CrossPoint;
import TOOL.GUI.DefinableShape;

import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;


import javax.swing.InputMap;
import javax.swing.ActionMap;
import java.awt.event.InputEvent;
import javax.swing.KeyStroke;
import javax.swing.AbstractAction;
import java.awt.event.ActionEvent;
import java.awt.event.KeyEvent;
import javax.swing.JComponent;


/**
 * @author Nicholas Dunn
 * @date 5/7/08
 *
 * An implementation of the ModelViewController design methodology for our
 * ClassifierModule.
 *
 * This class merely displays the underlying data of the ClassifierModel.
 * @see DefaultDrawingPanel
 * 
 */

public class ClassifierView extends DefaultDrawingPanel implements Observer,
                                                                   MouseMotionListener{

    ClassifierModel model;

    protected static final Cursor HAND_CURSOR = Cursor.
        getPredefinedCursor(Cursor.HAND_CURSOR);
    protected static final Cursor MOVE_CURSOR = Cursor.
        getPredefinedCursor(Cursor.MOVE_CURSOR);
    protected static final Cursor DEFAULT_CURSOR = Cursor.
        getDefaultCursor();
    protected static final Cursor CROSSHAIR_CURSOR = Cursor.
        getPredefinedCursor(Cursor.CROSSHAIR_CURSOR);


    protected static final AlphaComposite NORMAL_ALPHA = 
        AlphaComposite.getInstance(AlphaComposite.SRC_OVER, .4F);
    protected static final AlphaComposite SELECTED_ALPHA = 
        AlphaComposite.getInstance(AlphaComposite.SRC_OVER, .7F);
    protected static final AlphaComposite OPAQUE =
        AlphaComposite.getInstance(AlphaComposite.SRC_OVER, 1.0F);
    protected static final AlphaComposite HANDLE_ALPHA = OPAQUE;
    
    protected boolean moving = false;

    protected static final Font CLASS_FONT = 
        new Font("SansSerif", Font.PLAIN, 8);
    

    
    /**
     * @param model the underlying model which we display 
     */
    public ClassifierView(ClassifierModel model) {
        this.model = model;
        model.addObserver(this); // Connect the View to the model
    }

        
    /**
     * Fulfills the Observer interface.  Called whenever our model changes and
     * we must refresh.
     */ 
    public void update(Observable o, Object arg) {

        fixMode(model.getMode());
        
        if (model.getCurrentImage() != null) {
            updateImage(model.getCurrentImage());
        }
        repaint();
    }

    public void drawExistingShapes(Graphics g) {
        Graphics2D g2d = (Graphics2D) g;
        
        AffineTransform oldTransform = g2d.getTransform();

        g2d.scale(xScale, yScale);

        if (model.hasClassifiedObjects()) {
            for (ClassifierObject e : model.getVisibleObjects()) {
                e.drawUnselected(g2d);
            }
        }
        
        if (model.hasSelectedObjects()) {
            for (ClassifierObject e : model.getSelected()) {
                e.drawSelected(g2d);
            }
            
            // We do the drawing of the labels in a separate loop just so that
            // they are not obscured behind classified shapes
            FontMetrics fontInfo = g2d.getFontMetrics(CLASS_FONT);
            
            for (ClassifierObject e : model.getSelected()) {
                drawClassifierObjectInfo(g2d, e, fontInfo);
            }
        }
        // Get rid of the scaling
        g2d.setTransform(oldTransform);

    }

    protected void drawClassifierObjectInfo(Graphics2D g2d,
                                            ClassifierObject e,
                                            FontMetrics fontInfo) {
        Color oldColor = g2d.getColor();
        Composite oldOpacity = g2d.getComposite();
        


        String textToDraw = e.getLabel();
        // Calculate the bounding box around the text to draw
        java.awt.geom.Rectangle2D stringBounds = 
            fontInfo.getStringBounds(textToDraw, g2d);

        // Calculate the bounding box around the shape already drawn
        java.awt.Rectangle shapeBounds = e.getShape().getBounds();

        // We attempt to draw our string roughly in the middle of the object,
        int stringX = (int) (shapeBounds.getX() + shapeBounds.getWidth()/2 -
                             stringBounds.getWidth()/2);
        int stringY = (int) (shapeBounds.getY() + shapeBounds.getHeight()/2 +                              stringBounds.getHeight()/2);
        
        g2d.setFont(CLASS_FONT);

        g2d.setColor(Color.WHITE);
        g2d.setComposite(NORMAL_ALPHA);


        // Draw a background rectangle in white, over which we draw the
        // string
        g2d.fillRect(stringX, 
                     stringY - (int) stringBounds.getHeight() + fontInfo.getDescent(), 
                     (int) stringBounds.getWidth(),   
                     (int) stringBounds.getHeight());
        Color stringColor = Color.BLACK;

        // Draw the string
        g2d.setColor(stringColor);
        g2d.setComposite(OPAQUE);
        g2d.drawString(textToDraw, stringX, stringY); 
        
        // Restore old settings
        g2d.setComposite(oldOpacity);
        g2d.setColor(oldColor);
    }


    public void calculateScaling() {
        super.calculateScaling();
        super.setLineWidth((float) xScale);
    }
    

    // Based on the classifier mode we're in, change the type of shape we're
    // drawing as well as the color (in some cases)
    public void fixMode(ClassifierModel.ClassifierMode mode) {
        switch (mode) {
        case LINE:
            super.setMode(DrawingMode.LINE);
            break;
        case INNER_L_CORNER:
            super.setMode(DrawingMode.POINT);
            break;
        case OUTER_L_CORNER:
            super.setMode(DrawingMode.POINT);
            break;
        case T_CORNER:
            super.setMode(DrawingMode.POINT);
            break;
        case BLUE_GOAL:
            super.setMode(DrawingMode.QUADRILATERAL);
            break;
        case YELLOW_GOAL:
            super.setMode(DrawingMode.QUADRILATERAL);
            break;
        case BLUE_YELLOW_BEACON:
        case YELLOW_BLUE_BEACON:
            super.setMode(DrawingMode.QUADRILATERAL);
            break;
        case BALL:
        case CENTER_CIRCLE:
            super.setMode(DrawingMode.ELLIPSE);
            break;
        case SELECTION:
            super.setMode(DrawingMode.SELECTION_RECT);
            break;
        case YELLOW_CORNER_ARC:
            super.setMode(DrawingMode.ARC);
            setOutlineColor(Color.YELLOW);
            break;
        case BLUE_CORNER_ARC:
            super.setMode(DrawingMode.ARC);
            setOutlineColor(Color.BLUE);
            break;
        case MOVING:
            super.setMode(DrawingMode.NONE);
            break;
        case DOG:
            super.setMode(DrawingMode.RECTANGLE);
            break;
        }
        super.setLineWidth((float) xScale);
    }
    

    public void mousePressed(MouseEvent e) {
        super.mousePressed(e); 
    }

    public void mouseReleased(MouseEvent e) {
        super.mouseReleased(e);
    }
        
    public void mouseDragged(MouseEvent e) {
        super.mouseDragged(e);
    }

    public void mouseMoved(MouseEvent e) {}
    
    /**
     * Converts screen coordinates to image coordinates by dividing by the
     * x and y scales.  Note that unlike the getImageX and getImageY methods
     * of ImagePanel,this does not give -1 if outside of the image.
     */
    public Point getImageCoordinate(Point screenCoordinate) {
        return new Point((int) (screenCoordinate.getX() / xScale),
                         (int) (screenCoordinate.getY() / yScale));
    }


    
    

}
