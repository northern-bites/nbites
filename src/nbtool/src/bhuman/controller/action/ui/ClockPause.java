package controller.action.ui;

import common.Log;
import controller.action.ActionBoard;
import controller.action.ActionType;
import controller.action.GCAction;
import data.AdvancedData;


/**
 * @author Michel Bartsch
 * 
 * This action means that the clock is to be paused.
 */
public class ClockPause extends GCAction
{
    /**
     * Creates a new ClockPause action.
     * Look at the ActionBoard before using this.
     */
    public ClockPause()
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
        if (ActionBoard.clock.isClockRunning(data)) {
            if (data.manPlay) {
                data.manRemainingGameTimeOffset += data.getTime() - data.manWhenClockChanged;
                data.manPlay = false;
            } else {
                data.manWhenClockChanged = data.getTime();
                data.manPause = true;
            }
            Log.state(data, "Time manual paused");
        } else {
            if (data.manPause) {
                data.manPause = false;
                data.manTimeOffset -= data.getTime() - data.manWhenClockChanged;
            } else {
                data.manWhenClockChanged = data.getTime();
                data.manPlay = true;
            }
            Log.state(data, "Time manual running");
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
        return data.testmode;
    }
}