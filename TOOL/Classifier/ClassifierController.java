package edu.bowdoin.robocup.TOOL.Classifier;

import java.awt.*;

import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.event.MouseWheelListener;
import java.awt.event.MouseWheelEvent;
import java.awt.event.KeyListener;
import java.awt.event.KeyEvent;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

import java.awt.geom.Rectangle2D;

import javax.swing.JPopupMenu;
import javax.swing.JMenu;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;

import java.awt.Point;

import edu.bowdoin.robocup.TOOL.GUI.DefinableShape;
import edu.bowdoin.robocup.TOOL.TOOL;
import javax.swing.*;


/**
 * @author Nicholas Dunn
 * @date 5/7/08
 *
 * An implementation of the ModelViewController design methodology for our
 * ClassifierModule.
 *
 * This class handles all user interactions such as mouse clicks, mouse drags,
 * button presses, key presses, etc.
 *
 * In general, we will use the ClassifierModel's public methods to change
 * the underlying data (such as after having deleted a classifier object)
 * which will then change the underlying data (and then will refresh the
 * view automatically).
 *
 * Certain other events cause us to change the view without modifying the 
 * Model - for instance, when defining a shape we draw the outline but we do 
 * not add it to the model until we finish defining it.
 */

public class ClassifierController implements 
                                             MouseListener,
                                             MouseMotionListener,
                                             MouseWheelListener
{

    ClassifierModel model;
    ClassifierView view;
    ClassifierModePanel panel;
    ClassifierButtonPanel buttons;

    private Point startPoint;
    private Point releasePoint;
    private Point draggingPoint;

    private Point selectionAreaStart;
    private Point selectionCurPoint;
    
    private int POPUP_MENU_BUTTON = MouseEvent.BUTTON3;
    private int POPUP_MENU_CTL_BUTTON = MouseEvent.BUTTON1;

    
    
    private int x1, y1;
    private int x2, y2;
    
    private JPopupMenu popupMenu;

    private PopupMenu popup;

    private JOptionPane optionPane;
    
    private JDialog dialog;
    private JList classifications;
    
    public ClassifierController(ClassifierModel model, ClassifierView view,
                                ClassifierModePanel panel,
                                ClassifierButtonPanel buttons) {
        this.model = model;
        this.view = view;
        this.panel = panel;
        this.buttons = buttons;

        // Initialize the optionPane so that we can classify things
        optionPane = new JOptionPane(
                                     "Classify component as:",
                                     JOptionPane.PLAIN_MESSAGE,
                                     JOptionPane.DEFAULT_OPTION,
                                     null,null);
        classifications = new JList(new String[] {"test"});
        
    }

    protected void createDialogBoxWithSubdivisions(String[] headings, String[] ... args) {
        DefaultListModel listModel = new DefaultListModel();
        
        
        for (String[] s : args) {
            listModel.addElement(new JList(s));
        }

        classifications = new JList(listModel);
        classifications.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
        
        optionPane.setOptions(new Object[] {classifications});

    }

    protected void createDialogBox(String[] labels) {
        classifications = new JList(labels);
       
        classifications.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
        
        optionPane.setOptions(new Object[] {classifications});
        
    }

    protected int showDialogBox(int x, int y) {
        classifications.addMouseListener(new ListListener());
        //        classifications.addKeyistener(new ListEnterListener());

        dialog = optionPane.createDialog(null, "Classify");
        dialog.setLocation(x,y);
        dialog.setVisible(true);
        
        return classifications.getSelectedIndex();
    }
        
     /**
     * Logic for selection mode
     *
     * If mouse pressed outside of any object, begin drawing a rectangular 
     * selection area.  Anything within that region is added to the selections
     * within the model.
     *
     * Else if the mouse is pressed within an object and dragged, all 
     * selected objects are moved
     */

    public void mousePressed(MouseEvent e) {
        

        if (!model.hasDataSetLoaded()) { return; }

        if (e.isPopupTrigger()) { return; }

        Point imageCoord = view.getImageCoordinate(e.getPoint());
        if (model.getMode() == ClassifierModel.ClassifierMode.SELECTION) {
            ClassifierObject container = model.getContainingObject(imageCoord);
            if (container != null) {
                // We're dragging if we continue to move the mouse
                draggingPoint = imageCoord;
                selectionAreaStart = null;
                // Check to see if we have the object we clicked in
                // already in our selected list; if not add it 
                if (!model.getSelected().contains(container)) {
                    // Shift entails adding to a selection
                    if (e.isShiftDown()) {
                        model.addSelected(container);
                    }
                    // Otherwise we replace what's in the selection with just
                    // ourself
                    else {
                        model.setSelected(container);
                    }
                }
            }
            // We clicked outside of everything, start selecting
            else {
                draggingPoint = null;
                selectionAreaStart = imageCoord;
            }
        }
        else {

        }
        startPoint = imageCoord;
    }
    
    public void fixPopupMenuFromObject(ClassifierObject o) {
        


    }

    public void fixPopupMenu() {
        switch (model.getMode()) {
        case BLUE_CORNER_ARC:
            createDialogBox(CornerArc.BLUE_IDS);
            break;
        case YELLOW_CORNER_ARC:
            createDialogBox(CornerArc.YELLOW_IDS);
            break;
        case CENTER_CIRCLE:
            createDialogBox(CenterCircle.IDS);
            break;
        case BALL:
            createDialogBox(Ball.IDS);
            break;
        case BLUE_GOAL:
            createDialogBox(Goal.BLUE_IDS);
            break;
        case YELLOW_GOAL:
            createDialogBox(Goal.YELLOW_IDS);
            break;
        case LINE:
            createDialogBox(Line.IDS);
            break;
        case T_CORNER:
            createDialogBox(Corner.T_CORNER_IDS);
            break;
        case INNER_L_CORNER:
            createDialogBox(Corner.L_CORNER_IDS);
            break;
        case OUTER_L_CORNER:
            createDialogBox(Corner.L_CORNER_IDS);
            break;

        case BLUE_YELLOW_BEACON:
        case YELLOW_BLUE_BEACON:               
            createDialogBox(Beacon.IDS);
            break;
        case DOG:
            createDialogBox(Dog.IDS);
            break;
        default:
            System.out.println("Fell through all the cases in fix popupMenu()");
        }
        
        
    }


    public void mouseReleased(MouseEvent e) {
        if (!model.hasDataSetLoaded()) { return; }
        
        // They right clicked; we handle right clicks in mouseClicked() method
        if (e.getButton() == POPUP_MENU_BUTTON ||
            e.getButton() == POPUP_MENU_CTL_BUTTON &&
            e.isControlDown()) {
            return;
        }

        Point imageCoord = view.getImageCoordinate(e.getPoint());
        
        releasePoint = imageCoord;
        int dx = (int) (releasePoint.getX() - startPoint.getX());
        int dy = (int) (releasePoint.getY() - startPoint.getY());
        
        if (model.getMode() != ClassifierModel.ClassifierMode.SELECTION) {
            // Check to make sure the user is done defining the shape
            if (view.shapeDefined()) {
                
                if (!bigEnough(view.getShape())) {
                    buttons.setText("Shape was too small.");
                    view.clearCurrentlyDrawnShape();
                    return;
                }

                
                fixPopupMenu();
                int xPopupLoc = (int) (TOOL.instance.getLocation().getX() + 
                                       e.getX());
                int yPopupLoc = (int) (TOOL.instance.getLocation().getY() +
                                       e.getY() + view.getShape().getHeight());
                
                int choiceIndex = showDialogBox(xPopupLoc, yPopupLoc);
                if (choiceIndex == -1) {
                    buttons.setText("Cancelled selection");
                    
                    // Clear the canvas of the drawn shape
                    view.clearCurrentlyDrawnShape();
                }
                else {
                    String choice = (String) classifications.getModel().
                        getElementAt(choiceIndex);
                    buttons.setText("Choice was " + choice);
                    addObject(choice, view.getShape(), view.getXScale()); 
                }
            }
        }

        else {
            // They were defining a selection area of the screen; add those
            // selections that were a part of it
            if (selectionAreaStart != null) {
                model.clearSelected();
                addSelections(startPoint, releasePoint);
            }
            else if (draggingPoint != null) {
                model.commitMove(model.getSelected(), dx, dy);
                view.fixMode(model.getMode());
            }
        }


    }


    
    /**
     * Determine if the shape that they drew was big enough to count as
     * a valid classified object.
     */
    public boolean bigEnough(DefinableShape s) {
        ClassifierModel.ClassifierMode mode = model.getMode();
        return s.getSize() > mode.getMinSize();
    }

    /**
     * Given a start and end point defining a rectangular region,
     * adds all objects within that space to the selected list.
     */
    public void addSelections(Point start, Point end) {
        double minX = Math.min(start.getX(), 
                            end.getX());
        double minY = Math.min(start.getY(),
                            end.getY());
        double width = Math.abs(start.getX() - end.getX());
        double height = Math.abs(start.getY() - end.getY());

        
        // Find all objects within the selection region, add them to 
        // selection
        for (ClassifierObject o : model.getVisibleObjects()) {
            if (o.intersects(minX, minY, width, height) ||
                o.contains(end)) {
                model.addSelected(o);
            }
        }
        
    }

    

    /**
     * On a mouse right click, we determine if the mouse was inside any of the
     * objects.  If it is, then we show a popup window to allow them to change
     * the classification of the object they have selected.
     */
    public void mouseClicked(MouseEvent e) {
        // If they're not right clicking, exit
        if (!(e.getButton() == POPUP_MENU_BUTTON ||
              e.getButton() == POPUP_MENU_CTL_BUTTON &&
              e.isControlDown())) {
            return;
        }
            
        if (!model.hasDataSetLoaded()) { return; }
        Point imageCoord = view.getImageCoordinate(e.getPoint());
        ClassifierObject container = model.getContainingObject(imageCoord);
        if (container != null) {
            fixPopupMenu();
            showDialogBox((int) e.getX(), (int) e.getY());
                
        }
        
        
    }
    public void mouseEntered(MouseEvent e) {}
    public void mouseExited(MouseEvent e) {}
    public void mouseMoved(MouseEvent e) {} 


    public void mouseDragged(MouseEvent e) {
        if (!model.hasDataSetLoaded()) { return; }
       

        Point imageCoord = view.getImageCoordinate(e.getPoint());
                        
        // We're defining a selection 
        if (model.getMode() == ClassifierModel.ClassifierMode.SELECTION
            && selectionAreaStart != null) {/*
            selectionCurPoint = imageCoord;
            model.clearSelected();
            addSelections(selectionAreaStart, selectionCurPoint);*/
        }

        // We're dragging
        else if (model.getMode() == ClassifierModel.ClassifierMode.SELECTION &&
                 draggingPoint != null && model.hasSelectedObjects()) {
            int dx = (int) (imageCoord.getX() - draggingPoint.getX());
            int dy = (int) (imageCoord.getY() - draggingPoint.getY());
            
            view.fixMode(ClassifierModel.ClassifierMode.MOVING);

            model.tempMove(dx, dy);
            draggingPoint = imageCoord;

        }
    }

    /**
     * Calculates the checkerboard distance between two points, or simply the
     * difference in x + the difference in y.  Used to ascertain whether or not
     * the mouse is close enough to the corners of a zone to enable resize mode.
     * @param x1 x coord of point one
     * @param y1 y coord of point one
     * @param x2 x coord of point two
     * @param y2 y coord of point two
     */
    public int checkerboardDistance(int x1, int y1, int x2, int y2) {
        return Math.abs(x1 - x2) + Math.abs(y1 - y2);
    }

    /**
     * Calculates the checkerboard distance between two points, or simply the
     * difference in x + the difference in y.  Used to ascertain whether or not
     * the mouse is close enough to the corners of a zone to enable resize mode.
     * This is a convenience method that takes two Points and then breaks them
     * into constituent (x,y) coords to pass into getCheckerboardDist(x1,y1..)
     * @param one the first Point
     * @param two the second Point
     */
    public int checkerboardDistance(Point one, Point two) {
        return checkerboardDistance((int)one.getX(), (int)one.getY(),
                                   (int)two.getX(), (int)two.getY());
    }
    

    public void addObject(String idString, DefinableShape s, double scale) {
        if (!model.hasDataSetLoaded()) { return; }

        ClassifierModel.ClassifierMode mode = model.getMode();
        int id;
        ClassifierObject obj;

        switch (mode) {
        case BALL:
            id = Ball.getIDFromString(idString);
            obj = new Ball(id, s, scale);

            break;
        case CENTER_CIRCLE:
            id = CenterCircle.getIDFromString(idString);
            obj = new CenterCircle(id, s, scale);

            break;
        case BLUE_GOAL:
        case YELLOW_GOAL:
            id = Goal.getIDFromString(idString);
            obj = new Goal(id, s, scale);

            break;

        case YELLOW_BLUE_BEACON:
        case BLUE_YELLOW_BEACON:
            id = Beacon.getIDFromString(idString);
            obj = new Beacon(id, s, scale);

            break;
        case LINE:
            id = Line.getIDFromString(idString);
            obj = new Line(id, s, scale);

            break;
        case T_CORNER:
        case INNER_L_CORNER:
        case OUTER_L_CORNER:
            id = Corner.getIDFromString(idString);
            obj = new Corner(id, s, scale);

            break;
        case YELLOW_CORNER_ARC:
        case BLUE_CORNER_ARC:
            id = CornerArc.getIDFromString(idString);
            obj = new CornerArc(id, s, scale);
            break;

        case DOG:
            id = Dog.getIDFromString(idString);
            obj = new Dog(id, s, scale);
            break;
        default:
            System.out.println("in add object, " + mode + " broke through all the cases");
            return;
        }
        
        //        System.out.println("Id was " + id + " for " + idString);
        
        model.add(obj);

    }


     /**
     * Holding meta or shift while scrolling the mouse wheel advances or
     * regresses the current image.  Pressing shift while scrolling
     * lets you switch modes (as long as they are not currently defining
     * a shape)
     */
    public void mouseWheelMoved(MouseWheelEvent e) {
        if (e.isShiftDown() && !view.isDrawing()) {
            panel.advanceMode(e.getWheelRotation());
        }
        else if (e.isMetaDown() || e.isControlDown()) {
	    if (e.getWheelRotation() > 0) {
		model.getNextImage();
            }
	    else{
                model.getLastImage();
	    }
	}
    }
    
    

    class ListEnterListener implements KeyListener {
        public void keyPressed(KeyEvent e) {
            dialog.setVisible(false);
            dialog.dispose();
        }
        public void keyTyped(KeyEvent e) {
            dialog.setVisible(false);
            dialog.dispose();
        }
        public void keyReleased(KeyEvent e) {
            dialog.setVisible(false);
            dialog.dispose();
        }


    };
    
    // http://java.sun.com/j2se/1.4.2/docs/api/javax/swing/JList.html
    class ListListener extends MouseAdapter {
        public void mouseClicked(MouseEvent e) {
            if (e.getClickCount() == 2) {
                dialog.setVisible(false);
                dialog.dispose();
            }
        }
    };


}
