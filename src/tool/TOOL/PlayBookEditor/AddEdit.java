package TOOL.PlayBookEditor;

import javax.swing.undo.*;
import javax.swing.*;
import java.util.ArrayList;
import java.util.LinkedList;

/**
 * Heavily based on code from
 * http://www.javaworld.com/javaworld/jw-06-1998/undoredo/
 *
 * @author Nicholas Dunn
 */
public class AddEdit extends AbstractUndoableEdit {

    private PlayBookObject element;
    private int index;
    private ArrayList<PlayBookObject> model;
    private LinkedList<PlayBookObject> oldSelections;
    private LinkedList<PlayBookObject> curSelections;

    public AddEdit(ArrayList<PlayBookObject> model,
                   LinkedList<PlayBookObject> curSelections,
                   PlayBookObject element, int index) {

        this.model = model;
        this.element = element;
        this.index = index;
        this.curSelections = curSelections;

        oldSelections = new LinkedList<PlayBookObject>(this.curSelections);
        model.add(index, element);

        curSelections.clear();
        curSelections.add(element);
    }

    public void undo() throws CannotUndoException {
        model.remove(index);
        curSelections.clear();
        for (PlayBookObject z : oldSelections) {
            curSelections.add(z);
        }
    }

    public void redo() throws CannotRedoException {
        model.add(index, element);
        curSelections.clear();
        curSelections.add(element);
    }
    public boolean canUndo() { return true; }

    public boolean canRedo() { return true; }

    public String getPresentationName() { return "Add"; }

}
