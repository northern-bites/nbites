package edu.bowdoin.robocup.TOOL.PlayBookEditor;

import javax.swing.undo.*;
import javax.swing.*;
import java.util.ArrayList;
import java.util.LinkedList;


/**
 * A paste command adds the contents of clipboard (a list of elements, possibly
 * of size one) to the complete list of zones.  It also changes the current
 * selections of the parent module to match the clipboard contents
 *
 * @see http://www.javaworld.com/javaworld/jw-06-1998/undoredo/
 *
 * @author Nicholas Dunn
 */
public class PasteEdit extends AbstractUndoableEdit {

    private LinkedList<PlayBookObject> clipboard, curSelections, oldSelections;
    /** The index of where the first element of the linked list will go in the
        model array list  */
    private int index;
    private ArrayList<PlayBookObject> model;

    public PasteEdit(ArrayList<PlayBookObject> model,
                     LinkedList<PlayBookObject> clipboard,
                     LinkedList<PlayBookObject> curSelections,
                     int index) {
        this.model = model;
        this.clipboard = clipboard;
        this.index = index;
        this.curSelections = curSelections;

        oldSelections = new LinkedList<PlayBookObject>(curSelections);
        curSelections.clear();
        for (PlayBookObject z : clipboard) {
	    z.setID(Integer.toString(model.size()));
            model.add(z);
            curSelections.add(z);
        }
    }

    /**
     * Remove all the elements of the linked list from the ArrayList.
     */
    public void undo() throws CannotUndoException {
        int size = clipboard.size();

        // Remove them in reverse order
        for (int i = size + index; i > index; i--) {
            model.remove(i);
        }
        curSelections.clear();

        for (PlayBookObject z : oldSelections) {
            //System.out.println("removing from cur selections: " + z);
            curSelections.add(z);
        }
    }

    public void redo() throws CannotRedoException {

        int i = index;
        curSelections.clear();
        for (PlayBookObject z: clipboard) {
            model.add(i++, z);
            curSelections.add(z);
        }
    }
    public boolean canUndo() { return true; }

    public boolean canRedo() { return true; }

    public String getPresentationName() { return "Paste"; }
}
