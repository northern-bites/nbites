package controller.action.ui.state;

import common.Log;
import controller.action.ActionType;
import controller.action.GCAction;
import data.AdvancedData;
import data.GameControlData;
import data.Rules;

/**
 * @author Michel Bartsch
 * 
 * This action means that the state is to be set to initial.
 */
public class Initial extends GCAction
{
    /**
     * Creates a new Initial action.
     * Look at the ActionBoard before using this.
     */
    public Initial()
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
        if (data.gameState == GameControlData.STATE_INITIAL) {
            return;
        }
        if (Rules.league.returnRobotsInGameStoppages) {
            data.resetPenaltyTimes();
        }
        data.whenCurrentGameStateBegan = data.getTime();
        data.gameState = GameControlData.STATE_INITIAL;
        Log.state(data, "Initial");
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
        return (data.gameState == GameControlData.STATE_INITIAL)
            || data.testmode;
    }
}