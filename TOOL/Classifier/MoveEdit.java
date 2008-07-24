package TOOL.Classifier;

import TOOL.GUI.Movable;

import javax.swing.undo.*;
import javax.swing.*;
import java.util.AbstractList;
import java.util.Vector;

/**
 * Heavily based on code from 
 * http://www.javaworld.com/javaworld/jw-06-1998/jw-06-undoredo.html?page=1 
 * @author Nicholas Dunn
 */
public class MoveEdit<T extends Movable> extends AbstractUndoableEdit {

    private AbstractList<T> curSelections;
    private Vector<T> toMove;
    private int dx, dy;

    public MoveEdit(AbstractList<T> fieldObjects,
                    int dx, int dy) {
        //curSelections = null;
        //curSelections = fieldObjects;
        toMove = new Vector<T>(fieldObjects);
        /*
        // Do the actual move
        for (T obj : curSelections) {
            obj.move(dx, dy);
        }
        */
        this.dx = dx;
        this.dy = dy;
       
    }
    
    public void undo() throws CannotUndoException {
        //curSelections.clear();
        
        for (T z : toMove) {
            z.move(-dx, -dy);
            //curSelections.add(objToMove);
        }
        
    }

    public void redo() throws CannotRedoException {
        //curSelections.clear();
        for (T z : toMove) {
            z.move(dx, dy);
            //curSelections.add(objToMove);
        }

    }

    public boolean canUndo() { return true; }

    public boolean canRedo() { return true; }

    public String getPresentationName() { return "Move"; }

}
