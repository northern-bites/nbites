package TOOL.Image;


import javax.swing.*;
import java.awt.*;
import java.awt.Point;
import java.awt.geom.GeneralPath;
import java.awt.event.*;
import java.lang.Math;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.Stack;

import java.awt.image.BufferedImage;
import java.awt.geom.*;
import TOOL.Classifier.*;
import TOOL.Misc.Pair;
import TOOL.Vision.Vision;

import java.io.*;

import java.awt.geom.AffineTransform;




/**
 * @author Nicholas Dunn
 * @date   February 14, 2008
 * 
 * Displays TOOLImages and allows user to draw shapes on them to correspond
 * with field objects.
 *
 * Used in conjunction with the Classifier class in order to categorize exactly
 * what appears in the image and where.  
 *
 * Cool features currently in here:
 * Hold down meta to radiate outward from the center (e.g. you click in the 
 * center of the ball and drag outward in order to match the width of the 
 * ball)
 * Hold down shift to constrain the aspect ratio to 1:1 (e.g. to make a perfect
 * circle or square)
 *
 * To fix for next check in:
 *
 * To come later:
 * Be able to select things you have laid down and reposition them, as well as
 * be able to change their dimensions.
 * 
 */

public class ClassifierDrawingPanel extends ImagePanel implements
                                                           MouseListener,
                                                           MouseMotionListener{
    
    protected Classifier classifier;
    
    // the anchor is the xy coord where user initially presses the mouse.
    // endP2 is always the place where user releases the mouse or where the 
    // cursor is during a drag.  endPt1 is usually the anchor, but sometimes
    // it is a different point (if we're radiating out from the center)
    protected Point anchor, endPt1, endPt2;
    // A point lying on the arc's periphery (used to calculate how to draw
    // the curve)
    protected Point arcRadius;
    protected boolean arcLineDrawn;

    // When we need to draw two lines in order to specify a shape (e.g.
    // a slanted retangle
    protected Pair<Point, Point> line1, line2;
    

    protected int imageWidth, imageHeight;

    protected Color curColor;
    protected boolean drawing;

    protected Classifier.ClassMode classifierMode;

    protected ClassifierObject curSelected;

    public enum DrawingMode {ARC, LINE, ELLIPSE, RECT, POINT};
    // Which mode we're drawing at the moment; changes how system reacts to
    // mouse movements
    protected DrawingMode drawMode;
    
    protected Stack<ClassifierObject> undoStack;
    protected Stack<ClassifierObject> redoStack;

    public static final int LINE_LENGTH = 5;

    // Limit the size of a rectangle
    public static final int MIN_RECT_WIDTH = 5;
    public static final int MIN_RECT_HEIGHT = 5;

    // How thick to draw each line or curve
    protected int lineWeight;
    protected int THICK_LINE_WEIGHT = 3;

    // Since we draw rectangles in two passes, keep track of
    // what has been drawn so far.
    protected GeneralPath curRectangle;

    protected Shape curShape;


    protected int xTranslate, yTranslate;
    


    public static final int DEFAULT_LINE_WIDTH = 1;

    public ClassifierDrawingPanel(Classifier c) {
        super();
        classifier = c;
        drawing = false;
        
        Classifier.ClassMode startMode = Classifier.ClassMode.LINE;
        drawMode = startMode.getShape();
        curColor = startMode.getColor();
        

        addMouseMotionListener(this);

        undoStack = new Stack<ClassifierObject>();
        redoStack = new Stack<ClassifierObject>();
                        
        lineWeight = (int) (DEFAULT_LINE_WIDTH * xImageScale);
        anchor = new Point();
        endPt1 = new Point();
        endPt2 = new Point();
        arcRadius = null;

        line1 = new Pair<Point, Point>(null, null);
        line2 = new Pair<Point, Point>(null, null);

        curRectangle = null;
        curShape = null;
        curSelected = null;

    }

    
    // Catch the resized window, update the line width to match.
    // also update our transformation
    protected void calculateScaling() {
        super.calculateScaling();
        lineWeight = (int) (DEFAULT_LINE_WIDTH * xImageScale);
    }
    
    /**
     * Whenever we load a new image, this method is called.  Used to calculate
     * new heights and widths, as well as clear any old drawing.
     */
    public void setImage(BufferedImage image) {
        super.setImage(image);
        imageWidth = image.getWidth();
        imageHeight = image.getHeight();
        clearCanvas();
    }


    /**
     * Erases the last drawn object from the screen; adds it to the redo stack
     */
    public void undo() {
        if (undoStack.empty()) {
            classifier.setText("Undo stack is empty; cannot undo.");
            return;
        }
        ClassifierObject s = undoStack.pop();
        redoStack.push(s);
        repaint();
    }

    /**
     * Puts the last object that was removed from the screen back on it.
     */
    public void redo() {
        if (redoStack.empty()) {
            classifier.setText("Redo stack is empty; cannot redo.");
            return;
        }
        ClassifierObject s= redoStack.pop();
        undoStack.push(s);
        repaint();
    }

    public void drawClassifiedShapes(ClassifierObject[] shapes) {
        if (shapes == null) { 
            System.err.println("Error, cannot "+
                               "draw a null array in drawClassifiedShapes");
        }
        for (int i = 0; i < shapes.length; i++) {
            if (shapes[i] != null) { 
                undoStack.push(shapes[i]);
            }
        }
    }


    /** Whenever the classifier panel changes its mode, this method should
     * be called so that we are drawing the correct shape and color */
    public void setMode(Classifier.ClassMode classMode) {
        // Since specifying an arc is a two step process, we want to ensure
        // that we STOP classifying it if we switch in the middle
        if (drawMode == DrawingMode.ARC) {
            arcLineDrawn = false;
            arcRadius = null;
            repaint();
        }
        // Same with rectangles
        else if (drawMode == DrawingMode.RECT) {
            line1.setFirst(null);
            
            //curRectangle = null;
            repaint();
        }
        drawMode = classMode.getShape();
        curColor = classMode.getColor();
        curShape = null;
    }
    
    public void setDrawing(boolean choice) {
        drawing = choice;
    }
  

    
    /**
     * Calculates the portion of the screen that must be redrawn during
     * a mouse drag.
     *
     * @param old1 the previous value of endPt1 before entering drag
     * @param old2 the previous value of endPt2 before entering drag
     * @param new1 current value of endPt1
     * @param new2 current value of endPt2
     *
     * Note that we can pass any four points that correspond to what is being
     * drawn and this method will determine the absolute biggest rectangle
     * that needs to be redrawn.  For instance, when we are drawing an arc
     * we care about where the user is stretching the curve, so one of these
     * points should be arcRadius.
     */
    protected int[] calculateRefreshArea(Point old1, Point old2,
                                      Point new1, Point new2) {
        int[] coords = new int[4];
        
        int minX, minY, maxX, maxY;
        minX = min((int) old1.getX(), (int) old2.getX(),
                   (int) new1.getX(), (int) new2.getX());
        minX = clip(minX, 0, (int) (xImageScale*imageWidth));

        
        minY = min((int) old1.getY(), (int) old2.getY(),
                   (int) new1.getY(), (int) new2.getY());
        minY = clip(minY, 0, (int) (yImageScale*imageHeight));

        
        maxX = max((int) old1.getX(), (int) old2.getX(),
                   (int) new1.getX(), (int) new2.getX());
        maxX = clip(maxX, 0, (int) (xImageScale*imageWidth));


        maxY = max((int) old1.getY(), (int) old2.getY(),
                   (int) new1.getY(), (int) new2.getY());
        maxY = clip(maxY, 0, (int) (yImageScale*imageHeight));

        int padding = (drawMode == DrawingMode.POINT) ? 
            (int) (LINE_LENGTH * xImageScale) : lineWeight;
        
        // Pad the line weight values to ensure we refresh enough
        coords[0] = minX - 2*padding;
        coords[1] = minY - 2*padding; 
        coords[2] = Math.abs(maxX-minX) + 4*padding; // width
        coords[3] = Math.abs(maxY-minY) + 4*padding; // height
        return coords;
    }

    




    /**
     * Draws the shape currently being specified, as well as those shapes
     * already drawn on the screen.
     */
    public void paint(Graphics g) {
        int x1,y1,x2,y2;
        x1 = (int) endPt1.getX(); y1 = (int) endPt1.getY();
        x2 = (int) endPt2.getX(); y2 = (int) endPt2.getY();

        super.paint(g);
        Graphics2D g2 = (Graphics2D) g;
        
        // Draw all the shapes previously specified
        g2.scale(xImageScale, yImageScale);
        Font curFont = new Font("SansSerif", Font.PLAIN, 8);
        g2.setFont(curFont);
        
        Iterator i = undoStack.iterator();
        while (i.hasNext()) {
            ClassifierObject s = (ClassifierObject) i.next();
            if (s!= curSelected) {
                Shape curShape = s.getShape();
                g2.setColor(s.getColor());
                g2.draw(curShape);
            }
        }
        
        

        // We have a currently selected object that we are going to draw more 
        // prominently, along with its text representation
        if (curSelected != null) {
            g2.setTransform(new AffineTransform());
            g2.translate(xTranslate, yTranslate);
            g2.scale(xImageScale, yImageScale);

            // thick line
            g2.setStroke(new BasicStroke(THICK_LINE_WEIGHT));
            g2.setColor(curSelected.getColor());
            g2.draw(curSelected.getShape());
           
           
            String textToDraw = curSelected.getLabel();
            FontMetrics fontInfo = g2.getFontMetrics(curFont);
            Rectangle2D stringBounds = fontInfo.getStringBounds(textToDraw, g2);
            Rectangle shapeBounds = curSelected.getShape().getBounds();
            
            int stringX, stringY;
            // We attempt to draw our string roughly in the middle of the object,
            // clipping the values to ensure that the string is fully drawn on 
            // the screen.
           stringX = clip((int)(shapeBounds.getX() + shapeBounds.getWidth()/2 -
                                stringBounds.getWidth()/2), 0, 
                          (int)(imageWidth - stringBounds.getWidth()));
           stringY = clip((int)(shapeBounds.getY() + shapeBounds.getHeight()/2 - 
                                stringBounds.getHeight()/2), 
                          (int)stringBounds.getHeight(),
                          (int)(imageHeight - stringBounds.getHeight()));

           // Certain shapes are drawn black; as such draw the text white
           // in these cases
           Color stringColor = (curSelected.getColor().equals(Color.BLACK)) ?
               Color.WHITE : Color.BLACK;
           g2.setColor(stringColor);
           g2.drawString(textToDraw, stringX, stringY); 
        }
        
        // Draw the line segment(s) describing the rectangle being drawn
        if (curShape != null) {
            g2.setStroke(new BasicStroke());
            g2.setColor(curColor);
            g2.draw(curShape);
        }
        // Return to identity matrix; removes old scale
       g2.setTransform(new AffineTransform());
       
       // All the code that follows pertains to mouse drag drawing code
       if (!drawing) { return; }
       
        // Make the lines the correct weight/width
        g2.setStroke(new BasicStroke(lineWeight));
        // We have a valid object, make sure our color is correct
        g2.setColor(curColor);
        
        switch (drawMode) {
        case ELLIPSE:
            g2.drawOval(Math.min(x1, x2), Math.min(y1, y2),
                        Math.abs(x2-x1),Math.abs(y2-y1));
            break;
        case LINE:
        case RECT:
            g2.drawLine(x1,y1,x2,y2);
            break;
        case POINT:
            // Horizontal segment
            g2.drawLine(x2-(int)(xImageScale*LINE_LENGTH/2),y2,
                        x2+(int)(xImageScale*LINE_LENGTH/2),y2);
            // Vertical segment
            g2.drawLine(x2,y2-(int)(yImageScale*LINE_LENGTH/2),
                        x2,y2+(int)(yImageScale*LINE_LENGTH/2));
            break;
        case ARC:


            // For the first part, just draw the line they're defining
            if (!arcLineDrawn || arcRadius == null) {
                g2.drawLine(x1,y1,x2,y2);
            }
            else {
                // Draw the curve they're now defining
                int rx, ry;
                rx = (int) arcRadius.getX();
                ry = (int) arcRadius.getY();

                GeneralPath b = new GeneralPath();
                b.moveTo(x1,y1);
                b.quadTo(rx,ry,x2,y2);
                g2.draw(b);
            }
            break;
        }
        g2.dispose();
                 
    }


    public void mouseClicked(MouseEvent e) {
        int x2 = (int) e.getX();
        int y2 = (int) e.getY();
        /*if (e.getButton() == POPUP_MENU_BUTTON ||
                e.getButton() == POPUP_MENU_CTL_BUTTON &&
                e.isControlDown())
                popupMenu.show(this, e.getX(), e.getY());*/
    }

    /** Draws the specified shape.  Rather than refreshing the entire canvas,
     * calculates just the area that has been affected and redraws that area
     * of the screen.
     */
    public void mouseDragged(MouseEvent e) {
        if (rightClick(e)) { return; }

        // Keep track of the old information so we can determine what portion
        // of the screen needs to be redrawn
        Point oldPt1, oldPt2;
        oldPt1 = (Point) endPt1.clone(); oldPt2 = (Point) endPt2.clone();
        
        int x1 = (int) anchor.getX();
        int y1 = (int) anchor.getY();
        int x2 = (int) e.getX();
        int y2 = (int) e.getY();
        

        // Constrain proportions
        if (e.isShiftDown()) {
            // X change is bigger than y; make Y change bigger to compensate
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
            // By this point, our endPt2 has been moved so as to constrain our
            // aspect ratio to be 1:1
            endPt2.setLocation(x2,y2);
        }
        
        // No proportion restraint
        else {
            // End point 2 is now where the mouse is
            endPt2.setLocation(e.getPoint());
        }


        // Radiate outward from original point - changes endPt1, doesn't affect
        // endPt2.
        if (e.isMetaDown() || e.isControlDown()) {
            // anchor.x - (x2 - anchor.x) = 2*anchor.x - x2
            endPt1.setLocation(2*anchor.getX() - x2,
                               2*anchor.getY() - y2);
        }

        // Normal mode - endPt1 is simply the anchor point.
        else {
            endPt1.setLocation(anchor);
        }

        // We've already drawn the line, now we're stretching it into a curve
        if (drawMode == DrawingMode.ARC && arcLineDrawn) {
            if (arcRadius == null) {
                arcRadius = new Point();
            }
            // we need our endPt2 and endPt1 to remain fixed; only our
            // arcRadius point changes after this
            arcRadius.setLocation(endPt2);
            endPt2.setLocation(oldPt2);
            endPt1.setLocation(oldPt1);
            
        }
        // Where we should repaint
        int[] area;

        // Take the point on the curve into consideration as well
        if (drawMode == DrawingMode.ARC && arcRadius != null) {
            area = calculateRefreshArea(oldPt1, oldPt2, arcRadius,
                                          endPt2);
        }
        // Disregard any of the arcRadius stuff; no curves
        else {
            area = calculateRefreshArea(oldPt1, oldPt2, endPt1,
                                        endPt2);
        }

        if (curSelected != null) {
            xTranslate = x2 -(int)anchor.getX();
            yTranslate = y2 -(int)anchor.getY();
            repaint();
        }
        else {
            // x, y, width, height
            repaint(area[0], area[1], area[2], area[3]);
        }
    }


   

    
    
    

 
    public void clearCanvas() {
        undoStack.clear();
        repaint();
    }


    public void mouseEntered(MouseEvent e) {}
    public void mouseExited(MouseEvent e) {}

    /**
     * Determine if the user is mousing over an object currently
     * drawn on the screen. If so, then we update a global variable
     * called curSelected and repaint the screen to reflect the 
     * new information (we draw a string identifying the object and
     * give it added prominence on the screen)
     */
    public void mouseMoved(MouseEvent e) {
        int x, y;
        // Convert screen coordinates into image coordinates
        x = (int)(e.getX() / xImageScale);
        y = (int)(e.getY() / yImageScale);
        // Create a small box around the cursor in order to check if
        // we're mousing over the shape or not
        Rectangle boundingBox = new Rectangle(x - 1, y - 1, 3, 3);

        // Check all of the objects on the screen
        Iterator i = undoStack.iterator();
        while (i.hasNext()) {
            
            ClassifierObject j = (ClassifierObject) i.next();
            
            // Note: we cannot use "shape.contains(point)" because
            // lines always return false for this method
            if (j.getShape().intersects(boundingBox)) {
                curSelected = j;
                repaint();
                return;
            } 
        }
        
        curSelected = null;
        repaint();
    }

    public boolean rightClick(MouseEvent e) {
        return e.getButton() == POPUP_MENU_BUTTON ||
            e.getButton() == POPUP_MENU_CTL_BUTTON &&
            e.isControlDown();
    }

    public void mousePressed(MouseEvent e) {
        // Right click doesn't start the process
        if (rightClick(e)) { return; }

        if (curSelected == null) {
            drawing = true;
        }
        // Reset our translation values
        else {
            xTranslate = 0; yTranslate = 0;
        }
        anchor = e.getPoint();
        // As soon as we start defining a shape when in
        // arc mode, we want to remove the straight line
        // segment if it exists
        if (drawMode == DrawingMode.ARC) { curShape = null; }
    }


    /**
     * Determines whether user is completely finished drawing the shape (certain
     * shapes require two presses to fully define, such as an arc or a 
     * rectangular object).
     *
     * If shape is completely finished, then a temporary copy of the shape
     * is saved to curShape so that our paint method can draw it correctly.
     * 
     * We then notify Classifier that we have a new object.
     *
     * We then remove the temporary curShape variable so that we do not have two
     * separate copies of the object drawn on the screen - if Classifier 
     * determines that the user meant to do what he did, then it adds a more
     * permanent version of the shape to the undoStack, but along with
     * extra information.
     *
     */
    public void mouseReleased(MouseEvent e) {
        // Popup a menu to allow them to reclassify what's already
        // been put down before
        if (curSelected != null && rightClick(e)) { 
            classifier.changeID(curSelected, (int)e.getX(), (int)e.getY());
            return; 
        }
        else if (rightClick(e)) { return; }


        // We have a selected object; if the user has moved it,
        // call the corresponding method to update the object.
        if (curSelected != null) { 
            curSelected.move((int)(xTranslate/xImageScale),
                             (int)(yTranslate/yImageScale));
            // reset x and y translate amounts
            xTranslate = 0;
            yTranslate = 0;
            repaint();
            return;
        }

        // We have no currently selected object; we're done drawing
        // at least part of if not all of a shape, depending on which
        // DrawingMode we are in.

        drawing = false;

        // For convenience, do the conversion just once and then use the 
        // values throughout the method
        int x1, y1, x2, y2, x3, y3;
        x1 = getImageX((int) endPt1.getX());
        y1 = getImageY((int) endPt1.getY());
        x2 = getImageX((int) endPt2.getX());
        y2 = getImageY((int) endPt2.getY());
        x3 = y3 = -1;

        // Out of the image frame
        if (x2 == -1 || y2 == -1) { return; }
        
        // Width in image terms, not screen.
        int width, height;
        width = Math.abs(x2-x1);
        height = Math.abs(y2-y1);
        /*
        if (drawMode != DrawingMode.POINT && (width <= MIN_RECT_WIDTH ||
                                           height <= MIN_RECT_HEIGHT)) {
            return;
            }*/
        

        // Go through all the different modes and change which shape or
        // part of the shape is drawn. 
        switch (drawMode) {
        case ELLIPSE:
            Ellipse2D.Float ellipse = new Ellipse2D.Float(Math.min(x1,x2),
                                                          Math.min(y1,y2),
                                                          Math.abs(x2-x1),
                                                          Math.abs(y2-y1));
            curShape = ellipse;
            break;
        case LINE:
            Line2D.Float line = new Line2D.Float(x1,y1,x2,y2);
            curShape = line;
            break;
        case POINT:
            // a point is just a cross in our system
            GeneralPath point = new GeneralPath();
            // Horizontal line segment
            point.moveTo(x2-LINE_LENGTH/2,y2);
            point.lineTo(x2+LINE_LENGTH/2,y2);
            // Vertical line segment
            point.moveTo(x2,y2-LINE_LENGTH/2);
            point.lineTo(x2,y2+LINE_LENGTH/2);
            curShape = point;
            break;
            
        // Drawing an arc consists of two steps: first dragging out
        // a straight line indicating its end points, then dragging
        // the line into a curve.
        // If we're done the first step, then we just add a straight
        // line segment as curShape; otherwise we add the curve.
        case ARC:
            // Draw the straight line segment
            if (!arcLineDrawn) {
                arcLineDrawn = true;
                Line2D.Float arcLine = new Line2D.Float(x1,y1,x2,y2);
                curShape = arcLine;
                repaint();
                return;
            }
            // Draw the curve
            else {
                x3 = getImageX((int) arcRadius.getX());
                y3 = getImageY((int) arcRadius.getY());

                GeneralPath b = new GeneralPath();
                b.moveTo(x1,y1);
                b.quadTo(x3,y3,x2,y2);
                curShape = b;
                // Reset variables so that we can start fresh the next
                // curve we draw
                arcLineDrawn = false;
                arcRadius = null;
            }
            break;
            
        // Due to the way rectangles are often slanted in dog images in
        // particular, a simple rectangular region is insufficient.  We
        // instead make the user specify the four points that make up
        // the rectangular region (though in a strict geometric sense,
        // the polygonal region is not rectangular).  We do this by
        // having the user trace either A) the parallel vertical sides
        // B) the horizontal vertical top and bottom or C) the two diagonals
        case RECT:
            // We just finished classifying the first segment of the rectangle
            if (line1.getFirst() == null) {
                // Keep track of our endpoints
                line1.setFirst(new Point(x1, y1));
                line1.setSecond(new Point(x2, y2));
                
                curRectangle = new GeneralPath();
                curRectangle.moveTo((float)line1.getFirst().getX(), 
                                    (float)line1.getFirst().getY());
                curRectangle.lineTo((float)line1.getSecond().getX(), 
                (float)line1.getSecond().getY());
                curShape = curRectangle;
                repaint();
                return;
            }
            // We just finished classifying the second segment of the rectangle
            else {

                line2.setFirst(new Point(x1, y1));
                line2.setSecond(new Point(x2, y2));
                
                curRectangle.moveTo(x1, y1); 
                curRectangle.lineTo(x2, y2);
                curShape = curRectangle;
                repaint();
                
                classifier.addRectangularObject((int)e.getX(),(int)e.getY(),
                                     line1, line2);
                curShape = null;

                // Reset to draw another object
                line1.setFirst(null);
                redoStack.clear();
                return;
            }
        } // end switch

        redoStack.clear();
        repaint();
               
        // Notify the classifier module that we have a new object classified
        classifier.addObject((int)e.getX(),(int)e.getY(),
                         x1,y1,x2,y2,x3,y3);
        curShape = null;
        repaint();
    } // end mouseReleased



    ////////////////////////////////////////////////////////////
    // CONVENIENCE METHODS
    ////////////////////////////////////////////////////////////

     /** 
      * Clips an integer to be strictly between min and max.  Used
      * primarily to ensure that certain objects do not go off of the frame
      * @param x the value to clip
      * @param min the minimum value x can take
      * @param max the maximum value x can take
      * @return x if x is between min and max, min if x is < min, and max if
      * x>max
      */
    protected int clip(int x, int min, int max) {
        if (x<min) { return min; }
        else if (x>max) { return max; }
        else { return x; }
    }

    /** @return the minimum of a variable length set of numbers */
    protected int min(int ... numbers) {
        int min = java.lang.Integer.MAX_VALUE;
        for (int i = 0; i < numbers.length; i++) {
            min = Math.min(min, numbers[i]);
        }
        return min;
    }
    /** @return the maximum of a variable length set of numbers */
    protected int max(int ... numbers) {
        int max = java.lang.Integer.MIN_VALUE;
        for (int i = 0; i < numbers.length; i++) {
            max = Math.max(max, numbers[i]);
        }
        return max;
    }
    
    /** c style print function for fast debugging..*/
    public void printf(String ... args) {
        for (int i = 0; i < args.length; i++) {
            System.out.print(args[i] + " ");
        }
        System.out.println();
    }


}