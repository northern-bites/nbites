package edu.bowdoin.robocup.TOOL.Classifier;

import edu.bowdoin.robocup.TOOL.GUI.ID_Changeable;

import javax.swing.undo.*;
import javax.swing.*;
import java.util.AbstractList;


/**
 * A relabel command changes the id of the ClassifierObject or ClassifierObjects *
 * @see http://www.javaworld.com/javaworld/jw-06-1998/undoredo/
 * 
 * @author Nicholas Dunn
 */


public class RelabelEdit extends AbstractUndoableEdit {
    
    private AbstractList<ID_Changeable> elements;
    int[] oldIDs;
    int newID;

    public RelabelEdit(AbstractList<ID_Changeable> elements, int newID) {
        this.elements = elements;
        this.newID = newID;
        oldIDs = new int[elements.size()];

        // Populate the array of oldIDs with the previous ID values 
        int i = 0;
        for (ID_Changeable z : elements) {
            oldIDs[i++] = z.getID();
        }
        
    }

    /**
     * Remove all the elements of the linked list from the ArrayList.
     */
    public void undo() throws CannotUndoException {
        int i = 0;
        for (ID_Changeable z : elements) {
            z.setID(oldIDs[i++]);
        }
    }

    public void redo() throws CannotRedoException {
        for (ID_Changeable z: elements) {
            z.setID(newID);
        }
    }
    
    public boolean canUndo() { return true; }

    public boolean canRedo() { return true; }

    public String getPresentationName() { return "Relabel"; }



}
