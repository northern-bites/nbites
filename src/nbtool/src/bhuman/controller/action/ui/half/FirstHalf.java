package controller.action.ui.half;

import common.Log;
import controller.action.ActionType;
import controller.action.GCAction;
import data.AdvancedData;
import data.GameControlData;
import data.Rules;
import data.TeamInfo;

/**
 * @author Michel Bartsch
 * 
 * This action means that the half is to be set to the first half.
 */
public class FirstHalf extends GCAction
{
    /**
     * Creates a new FirstHalf action.
     * Look at the ActionBoard before using this.
     */
    public FirstHalf()
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
        if (data.firstHalf != GameControlData.C_TRUE || data.secGameState == GameControlData.STATE2_PENALTYSHOOT) {
            data.firstHalf = GameControlData.C_TRUE;
            data.secGameState = GameControlData.STATE2_NORMAL;
            changeSide(data);
            data.kickOffTeam = (data.leftSideKickoff ? data.team[0].teamNumber : data.team[1].teamNumber);
            data.gameState = GameControlData.STATE_INITIAL;
            // Don't set data.whenCurrentGameStateBegan, because it's used to count the pause
            Log.state(data, "1st Half");
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
        return ((data.firstHalf == GameControlData.C_TRUE)
                && (data.secGameState == GameControlData.STATE2_NORMAL))
                || (data.testmode);
    }
    
    /**
     * Switches sides for the teams, both for first to second and also
     * second to first half if needed.
     * 
     * @param data      The current data to work on.
     */
    public static void changeSide(AdvancedData data)
    {
        TeamInfo team = data.team[0];
        data.team[0] = data.team[1];
        data.team[1] = team;
        boolean[] ejected = data.ejected[0];
        data.ejected[0] = data.ejected[1];
        data.ejected[1] = ejected;
        // if necessary, swap back team colors
        if (data.secGameState != GameControlData.STATE2_PENALTYSHOOT 
                && data.colorChangeAuto) {
            byte color = data.team[0].teamColor;
            data.team[0].teamColor = data.team[1].teamColor;
            data.team[1].teamColor = color;
        }
        
        if (Rules.league.timeOutPerHalf && (data.secGameState != GameControlData.STATE2_PENALTYSHOOT)) {
            data.timeOutTaken = new boolean[] {false, false};
        } else {
            boolean timeOutTaken = data.timeOutTaken[0];
            data.timeOutTaken[0] = data.timeOutTaken[1];
            data.timeOutTaken[1] = timeOutTaken;
        }
        
        data.timeBeforeCurrentGameState = 0;
        data.whenDropIn = 0;
        if(data.secGameState != GameControlData.STATE2_PENALTYSHOOT) {
            data.resetPenalties();
        }
    }
}