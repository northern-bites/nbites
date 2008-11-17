package edu.bowdoin.robocup.TOOL.PlayBookEditor;

import javax.swing.undo.*;
import javax.swing.*;
import java.util.ArrayList;
import java.util.LinkedList;

/**
 * Heavily based on code from 
 * http://www.javaworld.com/javaworld/jw-06-1998/jw-06-undoredo.html?page=1 
 * @author Nicholas Dunn
 */

public class ClearEdit extends AbstractUndoableEdit {

    private LinkedList<PlayBookObject> curSelections, curSelectionsBackup;
    private ArrayList<PlayBookObject> allZones, allZonesBackup;

    public ClearEdit(ArrayList<PlayBookObject> zones, 
                     LinkedList<PlayBookObject> curSelections) {
        this.allZones = zones;
        this.curSelections = curSelections;

        curSelectionsBackup = new LinkedList<PlayBookObject>(curSelections);
        allZonesBackup = new ArrayList<PlayBookObject>(allZones);
        // Clear both the current selections and all model elements.
        zones.clear();
        curSelections.clear();

    }

    public void undo() throws CannotUndoException {
        curSelections.clear();
        for (PlayBookObject z : curSelectionsBackup) {
            curSelections.add(z);
        }

        allZones.clear();
        for (PlayBookObject z : allZonesBackup) {
            allZones.add(z);
        }


    }

    public void redo() throws CannotRedoException {
        curSelections.clear();
        allZones.clear();

    }

    public boolean canUndo() { return true; }

    public boolean canRedo() { return true; }

    public String getPresentationName() { return "Clear"; }
}
