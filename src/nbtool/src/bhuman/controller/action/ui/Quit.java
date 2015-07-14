package controller.action.ui;

import controller.Clock;
import controller.EventHandler;
import controller.action.ActionType;
import controller.action.GCAction;
import data.AdvancedData;


/**
 * @author Michel Bartsch
 * 
 * This action means that the operator tries to close the GameController.
 */
public class Quit extends GCAction
{
    /**
     * Creates a new Quit action.
     * Look at the ActionBoard before using this.
     */
    public Quit()
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
        if (EventHandler.getInstance().lastUIEvent == this) {
            Clock.getInstance().interrupt();
        }
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