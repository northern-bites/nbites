package edu.bowdoin.robocup.TOOL.PlayBookEditor;

import javax.swing.undo.*;
import javax.swing.*;
import java.util.ArrayList;
import java.util.LinkedList;


/**
 * A relabel command changes the id of the PlayBookObject or PlayBookObjects *
 * @see http://www.javaworld.com/javaworld/jw-06-1998/undoredo/
 *
 * @author Nicholas Dunn
 */


public class RelabelEdit extends AbstractUndoableEdit {

    private LinkedList<PlayBookObject> elements;
    String[] oldIDs;
    String newID;

    public RelabelEdit(LinkedList<PlayBookObject> elements, String newID) {
        this.elements = elements;
        this.newID = newID;
        oldIDs = new String[elements.size()];

        // Populate the array of oldIDs with the previous ID values
        int i = 0;
        for (PlayBookObject z : elements) {
            oldIDs[i++] = z.getID();
        }
    }


    /**
     * Remove all the elements of the linked list from the ArrayList.
     */
    public void undo() throws CannotUndoException {
        int i = 0;
        for (PlayBookObject z : elements) {
            z.setID(oldIDs[i++]);
        }
    }

    public void redo() throws CannotRedoException {
        for (PlayBookObject z: elements) {
            z.setID(newID);
        }
    }

    public boolean canUndo() { return true; }

    public boolean canRedo() { return true; }

    public String getPresentationName() { return "Relabel"; }



}