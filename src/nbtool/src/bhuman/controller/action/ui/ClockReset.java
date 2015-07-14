package controller.action.ui;

import common.Log;
import controller.action.ActionType;
import controller.action.GCAction;
import data.AdvancedData;


/**
 * @author Michel Bartsch
 * 
 * This action means that the clock is to be resetted.
 */
public class ClockReset extends GCAction
{
    /**
     * Creates a new ClockReset action.
     * Look at the ActionBoard before using this.
     */
    public ClockReset()
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
        data.timeBeforeCurrentGameState = 0;
        data.whenCurrentGameStateBegan = data.getTime();
        data.manWhenClockChanged = data.whenCurrentGameStateBegan;
        data.manRemainingGameTimeOffset = 0;
        Log.state(data, "Time reset");
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
        return data.testmode;
    }
}