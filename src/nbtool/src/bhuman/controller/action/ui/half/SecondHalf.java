package controller.action.ui.half;

import common.Log;
import controller.action.ActionType;
import controller.action.GCAction;
import data.AdvancedData;
import data.GameControlData;
import data.Rules;


/**
 * @author Michel Bartsch
 * 
 * This action means that the half is to be set to the second half.
 */
public class SecondHalf extends GCAction
{
    /**
     * Creates a new SecondHalf action.
     * Look at the ActionBoard before using this.
     */
    public SecondHalf()
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
        if (data.firstHalf != GameControlData.C_FALSE || data.secGameState == GameControlData.STATE2_PENALTYSHOOT) {
            data.firstHalf = GameControlData.C_FALSE;
            data.secGameState = GameControlData.STATE2_NORMAL;
            FirstHalf.changeSide(data);
            data.kickOffTeam = (data.leftSideKickoff ? data.team[0].teamNumber : data.team[1].teamNumber);
            data.gameState = GameControlData.STATE_INITIAL;
            // Don't set data.whenCurrentGameStateBegan, because it's used to count the pause
            Log.state(data, "2nd Half");
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
        return (!Rules.league.dropInPlayerMode
                && (((data.firstHalf != GameControlData.C_TRUE)
                        && (data.secGameState == GameControlData.STATE2_NORMAL))
                   || ((data.secGameState == GameControlData.STATE2_NORMAL)
                        && (data.gameState == GameControlData.STATE_FINISHED))))
                || data.testmode;
    }
}