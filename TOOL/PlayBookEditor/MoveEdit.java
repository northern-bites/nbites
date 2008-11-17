package edu.bowdoin.robocup.TOOL.PlayBookEditor;

import javax.swing.undo.*;
import javax.swing.*;
import java.util.LinkedList;

/**
 * Heavily based on code from 
 * http://www.javaworld.com/javaworld/jw-06-1998/jw-06-undoredo.html?page=1 
 * @author Nicholas Dunn
 */
public class MoveEdit extends AbstractUndoableEdit {

    private LinkedList<PlayBookObject> curSelections, toMove;
    private int dx, dy;

    public MoveEdit(LinkedList<PlayBookObject> zones, int dx, int dy) {
        curSelections = zones;
        toMove = new LinkedList<PlayBookObject>(zones);
        this.dx = dx;
        this.dy = dy;
       
    }
    
    public void undo() throws CannotUndoException {
        //System.out.println(toMove + " is moving " + (-dx) + " x, " + (-dy) + "y in undo()");
        curSelections.clear();
        
        for (PlayBookObject z : toMove) {
            z.move(-dx, -dy);
            curSelections.add(z);
        }
        


    }

    public void redo() throws CannotRedoException {
        // System.out.println(toMove + " is moving " + (dx) + " x, " + (dy) + "y in redo()");
        curSelections.clear();
        for (PlayBookObject z : toMove) {
            z.move(dx, dy);
            curSelections.add(z);
        }

    }

    public boolean canUndo() { return true; }

    public boolean canRedo() { return true; }

    public String getPresentationName() { return "Move"; }

}
