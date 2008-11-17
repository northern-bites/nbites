package edu.bowdoin.robocup.TOOL.Classifier;

import javax.swing.undo.*;
import javax.swing.*;
import java.util.AbstractList;
import java.util.Vector;

/**
 * Heavily based on code from 
 * http://www.javaworld.com/javaworld/jw-06-1998/jw-06-undoredo.html?page=1 
 * @author Nicholas Dunn
 */

public class ClearEdit<T> extends AbstractUndoableEdit {

    private AbstractList<T> allFieldObjects, curSelections;
    private Vector<T> curSelectionsBackup, allFieldObjectsBackup;

    public ClearEdit(AbstractList<T> fieldObjects, 
                     AbstractList<T> curSelections) {
        this.allFieldObjects = fieldObjects;
        this.curSelections = curSelections;

        curSelectionsBackup = new Vector<T>(curSelections);
        allFieldObjectsBackup = new Vector<T>(allFieldObjects);
        
        // Clear both the current selections and all model elements.
        fieldObjects.clear();
        curSelections.clear();

    }

    public void undo() throws CannotUndoException {
        curSelections.clear();
        for (T z : curSelectionsBackup) {
            curSelections.add(z);
        }

        allFieldObjects.clear();
        for (T z : allFieldObjectsBackup) {
            allFieldObjects.add(z);
        }


    }

    public void redo() throws CannotRedoException {
        curSelections.clear();
        allFieldObjects.clear();

    }

    public boolean canUndo() { return true; }

    public boolean canRedo() { return true; }

    public String getPresentationName() { return "Clear"; }
}
