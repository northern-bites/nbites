package TOOL.Image;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.lang.Math;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.Stack;

import java.awt.geom.*;
import java.io.*;

import TOOL.TOOL;
import TOOL.Classifier.*;


public class DrawingPanel extends ImagePanel implements MouseListener,
                                                              MouseMotionListener{
    
    // Keep track of xy locations to draw
    public int x1,y1,x2,y2;

    protected boolean filled;
    protected Color curColor;

    public boolean drawing;

    protected Classifier.ClassMode classifierMode;
    protected enum DrawingMode {ARC, LINE, ELLIPSE, RECT, POINT};
    protected DrawingMode drawMode;

    protected ArrayList<Shape> curShapes;

    protected Stack<Shape> undoStack;
    protected Stack<Shape> redoStack;


    protected Shape curShape;

    public static final int LINE_LENGTH = 5;

    public static final int MIN_RECT_WIDTH = 5;
    public static final int MIN_RECT_HEIGHT = 5;


    public DrawingPanel() {
        super();
        
        drawing = false;
        drawMode = DrawingMode.LINE;
        curShapes = new ArrayList<Shape>();

        addMouseMotionListener(this);

        undoStack = new Stack<Shape>();
        redoStack = new Stack<Shape>();
        
        curColor = Color.black;
        filled = false;
    }
    

    public void undo() {
        if (undoStack.empty()) {
            //setText("Undo stack is empty; cannot undo.");
            return;
        }

        Shape s = undoStack.pop();
        redoStack.push(s);
        
        repaint();


    }

    public void redo() {
        if (redoStack.empty()) {
            return;
        }

        Shape s = redoStack.pop();
        undoStack.push(s);
        repaint();

    }

    
    public void setMode(Classifier.ClassMode classMode) {/*
        switch (classMode) {
        case BALL:
        case CENTER_CIRCLE:
            drawMode = DrawingMode.ELLIPSE;
            break;
        case LINE:
            drawMode = DrawingMode.LINE;
            break;
            
        case CORNER:
            drawMode = DrawingMode.POINT;
            break;
        case CORNER_ARC:
            drawMode = DrawingMode.ARC;
            break;
        case BEACON:
        case GOAL:
            drawMode = DrawingMode.RECT;
            break;
            }*/


    }
    
    public void setDrawing(boolean choice) {
        drawing = choice;

    }

    public void setFilled(boolean choice) {
        filled = choice;
    }


    public void setColor(Color n) {

        curColor = n;
    }


    public void paint(Graphics g) {

        super.paint(g);
        if (!drawing) { return; }

        Graphics2D g2 = (Graphics2D) g;
        g2.setStroke(new BasicStroke(5));
        g2.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER, .6F));

        g2.setColor(curColor);

        
        Iterator i = undoStack.iterator();
        while (i.hasNext()) {
            Shape b = (Shape) i.next();
            if (b.contains(x2,y2)) {
                g2.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER,
						    0.6f));
                g2.setColor(Color.blue);
                g2.fill(b);
            }
            else {
                g2.setColor(Color.black);
                g2.draw(b);
            }
        }
        
        

        
        switch (drawMode) {
        case ELLIPSE:
            g2.drawOval(Math.min(x1, x2), Math.min(y1, y2),Math.abs(x2-x1),Math.abs(y2-y1));
            break;
        case LINE:
            
            g2.drawLine(x1,y1,x2,y2);
        
            break;
        case POINT:
            g2.drawLine(x2-LINE_LENGTH/2,y2,x2+LINE_LENGTH/2,
                                           y2);
            g2.drawLine(x2,y2-LINE_LENGTH/2,x2,
                        y2+LINE_LENGTH/2);
            
            //g2.drawPoint
            break;
        case ARC:
            g2.drawArc(x2,y2,Math.abs(x2-x1),Math.abs(y2-y1),0,90);
            break;
        case RECT:
            if (filled) {
                g2.fillRect(Math.min(x1, x2), Math.min(y1, y2), Math.abs(x2-x1), Math.abs(y2-y1));
            }
            else {
                g2.drawRect(Math.min(x1, x2), Math.min(y1, y2), Math.abs(x2-x1), Math.abs(y2-y1));
            }
            break;

        }
                 
    }


    
    



    public void mouseClicked(MouseEvent e) {
        
        int x2 = (int) e.getX();
        int y2 = (int) e.getY();
    }

    


    public void mousePressed(MouseEvent e) {
        drawing = true;
        x1 = (int)e.getX();
        y1 = (int)e.getY();
    }
    public void mouseReleased(MouseEvent e) {
        x2 = (int) e.getX();
        y2 = (int) e.getY();

        drawing = false;

        int width, height;
        width = Math.abs(x2-x1);
        height = Math.abs(y2-y1);
        
        if (drawMode != DrawingMode.POINT && (width <= MIN_RECT_WIDTH ||
                                           height <= MIN_RECT_HEIGHT)) {
            //return;
        }

        switch (drawMode) {
        case ELLIPSE:
            undoStack.push(new Ellipse2D.Float(x1, y1, Math.abs(x2-x1),
                                              Math.abs(y2-y1)));

            break;
        case LINE:
            undoStack.push(new Line2D.Float(x1,y1,x2,y2));
            
            TOOL.CONSOLE.message("(" + x1 + "," + y1 + "),("+x2+","+y2+")");

            break;
        case POINT:
            // Horizontal line
            undoStack.push(new Line2D.Float(x2-LINE_LENGTH/2,y2,x2+LINE_LENGTH/2,
                                           y2));
            // vertical line
            undoStack.push(new Line2D.Float(x2,y2-LINE_LENGTH/2,x2,
                                           y2+LINE_LENGTH/2));


            break;
        case ARC:
            undoStack.push(new Arc2D.Float(x1,y1,Math.abs(x2-x1),
                                          Math.abs(y2-y1),0,90,
                                          Arc2D.OPEN));
                                          
            break;
        case RECT:

            width = Math.abs(x2-x1);
            height = Math.abs(y2-y1);
            
            undoStack.push(new Rectangle(Math.min(x1,x2),
                                         Math.min(y1,y2),
                                         Math.abs(x2-x1),
                                         Math.abs(y2-y1)));
            break;
        }
        
        redoStack.clear();
        


        repaint();
    }


    public boolean isBigEnough(Shape s) {
        
        return true;
    }

    public void clearCanvas() {
        undoStack.clear();
        repaint();
    }


    public void mouseEntered(MouseEvent e) {}
    public void mouseExited(MouseEvent e) {}

    public void mouseMoved(MouseEvent e) {}
    public void mouseDragged(MouseEvent e) {

        x2 = (int) e.getX();
        y2 = (int) e.getY();
        
        
        if (drawMode == DrawingMode.POINT) { return; }
      

        repaint();

    }

}
