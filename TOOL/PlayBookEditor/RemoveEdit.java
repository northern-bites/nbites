package edu.bowdoin.robocup.TOOL.PlayBookEditor;

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
public class RemoveEdit extends AbstractUndoableEdit {

    private LinkedList<PlayBookObject> curSelections, backup;

    private int[] indices;
    private ArrayList<PlayBookObject> model;

    public RemoveEdit(ArrayList<PlayBookObject> model, 
                      int[] indices,
                      LinkedList<PlayBookObject> curSelections) {
        this.model = model;
        this.curSelections = curSelections;
        this.indices = indices;
        backup = new LinkedList<PlayBookObject>(curSelections);

        // Remove the objects from the cur selections, and from the complete 
        // set of objects
        curSelections.clear();
        for (PlayBookObject z : backup) {
            model.remove(z);
        }

       
    }

    public void undo() throws CannotUndoException {
        int i = 0;
        curSelections.clear();

        

        for (PlayBookObject z: backup) {
            model.add(indices[i++], z);
            curSelections.add(z);
        }
        
    }

    public void redo() throws CannotRedoException {

        System.out.println("length of model... " + model.size());

        int i = indices.length - 1;
        //        int i = 0;
        curSelections.clear();
        for (PlayBookObject z: backup) {
            model.remove(indices[i--]);
        }

    }
    
    public boolean canUndo() { return true; }

    public boolean canRedo() { return true; }

    public String getPresentationName() { return "Remove"; }

}
