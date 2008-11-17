package edu.bowdoin.robocup.TOOL.Classifier;

import edu.bowdoin.robocup.TOOL.TOOL;

import edu.bowdoin.robocup.TOOL.Image.TOOLImage;
import edu.bowdoin.robocup.TOOL.Data.DataListener;
import edu.bowdoin.robocup.TOOL.Data.DataSet;
import edu.bowdoin.robocup.TOOL.Data.Frame;
import edu.bowdoin.robocup.TOOL.Data.DataListener;
import edu.bowdoin.robocup.TOOL.Calibrate.VisionState;
import edu.bowdoin.robocup.TOOL.GUI.Movable;

import java.util.Observable;
import java.util.AbstractList;
import java.util.ArrayList;
import java.util.LinkedList;
import javax.swing.event.*;
import javax.swing.undo.*;

import java.awt.Point;
import edu.bowdoin.robocup.TOOL.Misc.Pair;

/**
 * @author Nicholas Dunn
 * @date 5/7/08
 *
 * An implementation of the ModelViewController design methodology for our
 * ClassifierModule.
 *
 * This class holds all the data for our Classifier module, so all the
 * classifier objects and the frame to which they correspond.
 *
 * This class provides a public interface for other classes to modify the data,
 * in particular to add objects, delete them, select them, and so forth.
 *
 * Furthermore, it automatically updates our View component to have a current
 * view of our data (for instance, when we switch frames).
 *
 */

public class ClassifierModel extends Observable implements DataListener//,
                                                           //ID_Queryable
{
    
    public static final int NO_IMAGES_LOADED = -1;

    // The TOOL object into which this Model is added.
    protected TOOL tool;
    protected TOOLImage rawImage;
    protected UndoManager undoManager;         // history list
    protected UndoableEditSupport undoSupport; // event support

    // Holds all the objects for all the frames.  Each frame has
    // a corresponding ArrayList.
    protected ArrayList<ArrayList<ClassifierObject>> classifiedObjects;

    

    // Holds all the objects currently selected
    protected LinkedList<ClassifierObject> selected;
    
    // Number of images in the current directory; determines the size of the
    // array we allocate
    protected int numImages = NO_IMAGES_LOADED;
    // The index into our array
    protected int curImageIndex;
    protected DataSet images = null;

    protected static final int MIN_SIZE = 5;
    // We don't care about how far the mouse was dragged for a point
    protected static final int MIN_CORNER_SIZE = 0;
    
    



    public enum ClassifierMode
    {
        LINE(MIN_SIZE),
            INNER_L_CORNER(MIN_CORNER_SIZE),
            OUTER_L_CORNER(MIN_CORNER_SIZE), 
            T_CORNER(MIN_CORNER_SIZE),
            BLUE_GOAL(MIN_SIZE),
            YELLOW_GOAL(MIN_SIZE),
            BALL(MIN_SIZE),
            CENTER_CIRCLE(MIN_SIZE),
            SELECTION(MIN_SIZE),
            BLUE_CORNER_ARC(MIN_SIZE),
            YELLOW_CORNER_ARC(MIN_SIZE),
            BLUE_YELLOW_BEACON(MIN_SIZE),
            YELLOW_BLUE_BEACON(MIN_SIZE),
            DOG(MIN_SIZE),
            MOVING(0);
        
        //private class instantiation;
        private int minSize;
        ClassifierMode(int minSize) {
            this.minSize = minSize;
        }
        public int getMinSize() { return minSize; }
    };
   
    // the "moving" class mode doesn't correspond to an actual object
    public static final int NUM_CLASS_OBJECTS = 
        ClassifierMode.values().length  - 1;

    protected ClassifierMode mode;
  

    /**
     * Default constructor.  
     * @param t The TOOL instance associated with this Model.
     */
    public ClassifierModel(TOOL t) {
        this.tool = t;
        
        // We start with an empty list of selected items.
        selected = new LinkedList<ClassifierObject>();
        mode = ClassifierMode.SELECTION;
        
        // Take care of all the undo/redos of the model
        undoManager = new UndoManager();         // history list
        undoSupport = new UndoableEditSupport();
        undoSupport.addUndoableEditListener(new UndoAdapter());

    }


    /**
     * Initializes the array of arraylists to have size numElements; each
     * arraylist is created to be empty
     */
    private void initializeDataStructures(int numElements) {
        classifiedObjects = new ArrayList<ArrayList<ClassifierObject>>(numElements);
        for (int i = 0; i < numElements; i++) {
            classifiedObjects.add(new ArrayList<ClassifierObject>());
        }     
    }

    
    /**
     * @return true if and only if a data set has been loaded
     */
    public boolean hasDataSetLoaded() {
        return images != null;
    }


    /**
     * Called whenever the datamanaer switches data sets
     */
    public void notifyDataSet(DataSet s, Frame f) {
        // This is the first data set to be loaded
        if (images == null) {
            numImages = s.size();
            images = s;
            initializeDataStructures(numImages);
        }
        // We changed directory
        else if (images != s) {
            System.out.println("changed directories");
        }
        notifyFrame(f);
    }
    
    /**
     * Called whenever data manager changes frames; we notify all observers
     * of our model (so that they can get the most recent image, for instance)
     */ 
    public void notifyFrame(Frame f) {
        if (f == null || !f.hasImage()) { 
            return; 
        }
        rawImage = f.image();
        curImageIndex = tool.getDataManager().activeFrameIndex();
        
        setChanged();
        notifyObservers();
    }
    
    /* Public methods for View to access */
    public ClassifierMode getMode() {
        return mode;
    }

    public void setMode(ClassifierMode mode) {
        this.mode = mode;
        setChanged();
        notifyObservers();
    }

    public boolean hasClassifiedObjects() {        
        return classifiedObjects != null && 
            !classifiedObjects.get(curImageIndex).isEmpty();
    }


    /** 
     * Returns all the classifier objects visible in this frame (null if
     * nothing has yet been classified)
     */
    public AbstractList<ClassifierObject> getVisibleObjects() { 
        return classifiedObjects.get(curImageIndex); 
    }


    /** Returns the TOOL instance associated with this ClassifierModel */
    public TOOL getTOOL() { return tool; }

    /**
     * Returns the first object it finds that contains the point (x,y) in
     * picture coordinates, not screen coordinates.  Returns null if none
     * contain the point.
     * @param x the x coordinate in the image
     * @param y the y coordinate in the image
     */
    public ClassifierObject getContainingObject(int x, int y) {

        // Step through all the visible objects and check each in turn
        for (ClassifierObject e : getVisibleObjects()) {
            if (e.contains(x, y)) {
                return e;
            }
        }
        // None contained the point, return null
        return null;
    }

    /**
     * Convenience method that takes in a Point, takes the individual x and y
     * coords, and calls getContainingObject(int x, int y).
     * @param p the coordinates to check if any objects contain
     */
    public ClassifierObject getContainingObject(Point p) {
        return getContainingObject((int) p.getX(), (int)p.getY());
    }
    
    /** @return true if and only if there are selected objects in the frame */
    public boolean hasSelectedObjects() {
        return !selected.isEmpty();
    }

    /**
     * @return the currently selected objects.  This list might be empty
     */
    public AbstractList<ClassifierObject> getSelected() {
        return selected;
    }


    /**
     * Changes the currently selected object.  Notifies view to refresh.
     * @param a the new ClassifierObject to be currently selected
     */
    public void setSelected(ClassifierObject a) {
        selected.clear();
        selected.add(a);

        setChanged();
        notifyObservers();
    }

    /**
     * Adds all the visible objects on the screen to the selected list
     */
    public void selectAll() {
        selected.clear();
        for (ClassifierObject a : getVisibleObjects()) {
            selected.add(a);
        }
        setChanged();
        notifyObservers();
    }

    /**
     * Empties out the selected list and notifies observers of the change
     */
    public void clearSelected() {
        selected.clear();
        setChanged();
        notifyObservers();
    }

    public void addSelected(ClassifierObject a) {
        selected.add(a);
        setChanged();
        notifyObservers();
    }


    /** @return the current image to display; if no Data sets are loaded, 
     * returns null */
    public TOOLImage getCurrentImage() {
        return rawImage;
    }

    /**
     * @return the hashcode of the current image; allows us to make sure
     * we only redo/undo moves that are visible
     */
    public int getID() {
        return rawImage.hashCode();
    }

    /**
     * Allows the view to query whether or not there are any frames
     * after the current one - if not we disable the next button.
     */
    public boolean framesAfter() {
        return tool.getDataManager().hasElementAfter();
    }

    /**
     * Allows the view to query whether or not there are any frames
     * after the current one - if not we disable the previous button.
     */
    public boolean framesBefore() {
        return tool.getDataManager().hasElementBefore();
    }



   
    /**
     * @return the 0 based index of our currently viewable image.
     */
    public int getCurrentImageIndex() {
        return curImageIndex;
    }

    /** @return the number of images in the current data set */
    public int getNumberOfImages() {
        return numImages;
    }

    /**
     * @return the UndoManager associated with this model; allows the
     * view to deterine what to display on its buttons via calls to
     * getUndoPresentationName() and getRedoPresentationName()
     */
    public UndoManager getUndoManager() {
        return undoManager;
    }

    public String getUndoText() {
        return undoManager.getUndoPresentationName();
    }

    public String getRedoText() {
        return undoManager.getRedoPresentationName();
    }

    public boolean canUndo() {
        return undoManager.canUndo();
    }
    
    public boolean canRedo() {
        return undoManager.canRedo();
    }


    /* Public methods for Controller to access */
    
    /**
     * Asks the datamanager to change the current frame to the one specified
     * at index i.  Note that notifyFrame alerts listeners if there has in
     * fact been a change.
     */
    public void setImage(int i) {
        selected.clear();
        tool.getDataManager().set(i);
    }
    
    /** 
     * Goes one frame previous (if it exists)
     * Note that notifyFrame alerts listeners if there has in
     * fact been a change.
     */
    public void getLastImage() {
        selected.clear();
        tool.getDataManager().last();
    }
    
    /**
     * Goes one frame forward (if it exists)
     * Note that notifyFrame alerts listeners if there has in
     * fact been a change.
     */
    public void getNextImage() {
        selected.clear();
        tool.getDataManager().next();
        
    }
    


    /**
     * @param o the newly classified object that must be added to the model.
     */
    public void add(ClassifierObject o) {
        // The index at which we add our element is simply equal to
        // the number of elements currently in the list
        int numElements = getVisibleObjects().size();
        UndoableEdit edit = new AddEdit<ClassifierObject>(getVisibleObjects(),
                                        selected, o, numElements);
        undoSupport.postEdit(edit);

        setChanged();
        notifyObservers();
    }

    
    public void tempMove(int dx, int dy) {
        for (Movable z : getSelected()) {
            z.move(dx, dy);
        }
        setChanged();
        notifyObservers();
    }


    

    public void commitMove(AbstractList<ClassifierObject> toMove, int dx, int dy) {//AbstractList<Movable> toMove, int dx, int dy) {
        /*
        if (toMove.isEmpty()) { return; }
        */
        UndoableEdit edit =
            new MoveEdit<ClassifierObject>(getSelected(), dx, dy);
        
        //UndoableEdit edit = new MoveEdit(toMove, dx, dy);
        undoSupport.postEdit(edit);

        setChanged();
        notifyObservers();

    }

    /**
     * Move a group and commit the move to the undo stack.
     */
    public void move(AbstractList<Movable> toMove, int dx, int dy) {
        if (toMove.isEmpty()) { return; }
        
        UndoableEdit edit =
            new MoveEdit<Movable>(toMove, dx, dy);
        undoSupport.postEdit(edit);

        setChanged();
        notifyObservers();
    }


    /**
     * @param o the classified object that must be removed from the model
     */
    public void remove(AbstractList<ClassifierObject> toRemove) {
        if (toRemove == null) { return; }
        
        int[] indices = new int[toRemove.size()];
        // Search our visible objects for each element of the toRemove list
        // and note where it is found.  This allows us to undo and redo
        // the move correctly.
        for (int i = 0; i < indices.length; i++) {
            indices[i] = getVisibleObjects().indexOf(toRemove.get(i));
        }
        UndoableEdit edit =
            new RemoveEdit<ClassifierObject>(getVisibleObjects(), indices, selected);//, this, getID());
        undoSupport.postEdit(edit);

        
        setChanged();
        notifyObservers();
    }
    
    /**
     * Allows a user to relabel an object he places down on the screen;
     * for instance if he calls a goal post the left one but he sees it's
     * actually the right, he can reclassify it via this method.
     * @param o
     * @param id the new ID for the classified object.  
     */
    public void relabel(ClassifierObject o, int id) {
        setChanged();
        notifyObservers();
    }


    /**
     *
     */ 
    // TODO:  Figure out how to resize
    public void resize(ClassifierObject o) {
        setChanged();
        notifyObservers();
    }


    /**
     * Clears all classified objects from the currently visible frame.  
     * Anything in the selected list will be removed as well. 
     */
    public void clear() {
        // Takes care of deleting the objects from the lists.
        UndoableEdit edit =
            new ClearEdit<ClassifierObject>(getVisibleObjects(), selected);
        undoSupport.postEdit(edit);

        setChanged();
        notifyObservers();
    }

    /**
     * If there is a move to undo, undoes the last action.
     */
    public void undo() {
        if (undoManager.canUndo()) {
            undoManager.undo();
            setChanged();
            notifyObservers();
        }
    }

    /**
     * If there is an action to redo, redoes the last action.
     */
    public void redo() {
        if (undoManager.canRedo()) {
            undoManager.redo();
            setChanged();
            notifyObservers();
        }
    }

    
    
    
    /**
     * An undo/redo adapter.  This is notified whenever an undoable action
     * occurs.
     * @see http://www.javaworld.com/javaworld/jw-06-1998/undoredo/UndoPanel.java
     */
    private class UndoAdapter implements UndoableEditListener {
        public void undoableEditHappened (UndoableEditEvent evt) {
            UndoableEdit edit = evt.getEdit();
            undoManager.addEdit( edit );
        }
    }

}
