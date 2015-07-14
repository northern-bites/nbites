package controller.action.ui;

import controller.action.ActionType;
import controller.action.GCAction;
import data.AdvancedData;


/**
 * @author Thomas Roefer
 * 
 * Cancels an undo action in progress.
 * This a dummy action, because the undo buttons track
 * whether they created the previous event, and this is
 * one possibilty to generate a different event.
 */
public class CancelUndo extends GCAction
{
    /**
     * Creates a new CancelUndo action.
     * Look at the ActionBoard before using this.
     */
    public CancelUndo()
    {
      super(ActionType.UI);
    }

    /**
     * Performs this action to manipulate the data (model).
     * 
     * @param data      The current data to work on.
     */
    @Override
    public void perform(AdvancedData data)
    {
    }
    
    /**
     * Checks if this action is legal with the given data (model).
     * Illegal actions are not performed by the EventHandler.
     * 
     * @param data      The current data to check with.
     */
    @Override
    public boolean isLegal(AdvancedData data)
    {
        return true;
    }
}