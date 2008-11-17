package edu.bowdoin.robocup.TOOL.GUI;
import java.awt.Cursor;
import java.awt.Point;

public interface ResizeHandle {
    /**
     * Returns the cursor that should be displayed when mousing over the handle
     */ 
    public Cursor getCursor();

    /**
     * 
     */
    public void setCursor(Cursor e);


    /**
     * Called whenever user clicks and drags the handle
     */
    public void handleDragged(Point newLoc);

    

}