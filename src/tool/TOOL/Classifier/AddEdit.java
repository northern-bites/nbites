package TOOL.Classifier;

import javax.swing.undo.*;
import javax.swing.*;
import java.util.AbstractList;
import java.util.Vector;

/**
 * Heavily based on code from 
 * http://www.javaworld.com/javaworld/jw-06-1998/undoredo/
 * 
 * @author Nicholas Dunn
 */
public class AddEdit<T> extends AbstractUndoableEdit {

    private T element;
    private int index;
    private AbstractList<T> model;
    private Vector<T> oldSelections;
    private AbstractList<T> curSelections;
   

    public AddEdit(AbstractList<T> model, 
                   AbstractList<T> curSelections,
                   T element, int index) {

        this.model = model;
        this.element = element;
        this.index = index;
        this.curSelections = curSelections;

        oldSelections = new Vector<T>(curSelections);
        
        model.add(index, element);

        curSelections.clear();
        curSelections.add(element);
    }

    public void undo() throws CannotUndoException {
        model.remove(index);
        
        curSelections.clear();
        for (T z : oldSelections) {
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
