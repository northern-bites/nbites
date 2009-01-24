package TOOL.Classifier;

import TOOL.GUI.Resizable;
import javax.swing.undo.*;
import javax.swing.*;
import java.awt.Point;


/**
 * Heavily based on code from 
 * http://www.javaworld.com/javaworld/jw-06-1998/undoredo/
 * 
 * @author Nicholas Dunn
 */

public class ResizeEdit extends AbstractUndoableEdit {

    private Resizable fieldObject;
    private int direction;
    private Point originalPoint, newPoint;

    public ResizeEdit(Resizable fieldObject, int direction, 
                      Point originalPoint, Point newPoint) {
        this.fieldObject = fieldObject;
        this.direction = direction;
        this.originalPoint = originalPoint;
        this.newPoint = newPoint;
    }
    
    public void undo() throws CannotUndoException {
        fieldObject.resize();
        //fieldObject.resize(originalPoint, direction);
    }

    public void redo() throws CannotRedoException {
        fieldObject.resize();
        //fieldObject.resize(newPoint, direction);
    }

    public boolean canUndo() { return true; }

    public boolean canRedo() { return true; }

    public String getPresentationName() { return "Resize"; }

}
