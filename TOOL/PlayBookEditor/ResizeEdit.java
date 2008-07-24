package TOOL.PlayBookEditor;

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

    private PlayBookObject zone;
    private int direction;
    private Point originalPoint, newPoint;

    public ResizeEdit(PlayBookObject zone, int direction, Point originalPoint, 
                    Point newPoint) {
        this.zone = zone;
        this.direction = direction;
        this.originalPoint = originalPoint;
        this.newPoint = newPoint;
    }
    
    public void undo() throws CannotUndoException {
        zone.resize(originalPoint, direction);
    }

    public void redo() throws CannotRedoException {
        zone.resize(newPoint, direction);
    }

    public boolean canUndo() { return true; }

    public boolean canRedo() { return true; }

    public String getPresentationName() { return "Resize"; }

}