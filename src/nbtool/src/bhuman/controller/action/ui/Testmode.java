package controller.action.ui;

import common.Log;
import controller.action.ActionType;
import controller.action.GCAction;
import data.AdvancedData;


/**
 * @author Michel Bartsch
 * 
 * This action means that the testmode should be toggled on or off.
 */
public class Testmode extends GCAction
{
    /**
     * Creates a new Testmode action.
     * Look at the ActionBoard before using this.
     */
    public Testmode()
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
        data.testmode = !data.testmode;
        Log.toFile("Testmode = "+data.testmode);
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