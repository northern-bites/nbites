package TOOL.Classifier;

//import TOOL.GUI.ID_Queryable;
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
public class RemoveEdit<T> extends AbstractUndoableEdit {

    private AbstractList<T> model, curSelections;
    private Vector<T> backup;

    private int[] indices;
    /*private int id;
    private ID_Queryable parent;
    */

    public RemoveEdit(AbstractList<T> model, 
                      int[] indices,
                      AbstractList<T> curSelections/*,
                                                  ID_Queryable parent, int id) {*/
                      ){
                      
        this.model = model;
        this.curSelections = curSelections;
        this.indices = indices;
        //        this.parent = parent;
        backup = new Vector<T>(curSelections);

        // Remove the objects from the cur selections, and from the complete 
        // set of objects
        curSelections.clear();
        for (T z : backup) {
            model.remove(z);
        }       
    }

    public void undo() throws CannotUndoException {
        int i = 0;
        curSelections.clear();

        for (T z: backup) {
            model.add(indices[i++], z);
            curSelections.add(z);
        }
        
    }

    public void redo() throws CannotRedoException {

        int i = indices.length - 1;
        curSelections.clear();
        for (T z: backup) {
            model.remove(indices[i--]);
        }

    }
    
    public boolean canUndo() { return true; } // id == parent.getID(); }

    public boolean canRedo() { return true; } //id == parent.getID(); }

    public String getPresentationName() { return "Remove"; }

}
