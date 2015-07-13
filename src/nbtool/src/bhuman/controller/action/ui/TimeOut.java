package controller.action.ui;

import common.Log;
import controller.action.ActionBoard;
import controller.action.ActionType;
import controller.action.GCAction;
import data.AdvancedData;
import data.GameControlData;
import data.Rules;

/**
 * @author Michel Bartsch
 * 
 * This action means that a timeOut is to be taken or ending.
 */
public class TimeOut extends GCAction
{
    /** On which side (0:left, 1:right) */
    private int side;

    /**
     * Creates a new TimeOut action.
     * Look at the ActionBoard before using this.
     * 
     * @param side      On which side (0:left, 1:right)
     */
    public TimeOut(int side)
    {
        super(ActionType.UI);
        this.side = side;
    }

    /**
     * Performs this action to manipulate the data (model).
     * 
     * @param data      The current data to work on.
     */
    @Override
    public void perform(AdvancedData data)
    {
        if (!data.timeOutActive[side]) {
            data.previousSecGameState = data.secGameState;
            data.secGameState = GameControlData.STATE2_TIMEOUT;
            data.timeOutActive[side] = true;
            data.timeOutTaken[side] = true;
            if (data.previousSecGameState != GameControlData.STATE2_PENALTYSHOOT) {
                data.kickOffTeam = data.team[1 - side].teamNumber;
            } else if (data.gameState == GameControlData.STATE_SET) {
                data.team[data.kickOffTeam == data.team[0].teamNumber ? 0 : 1].penaltyShot--;
            }
            Log.setNextMessage("Timeout "+Rules.league.teamColorName[data.team[side].teamColor]);
            data.gameState = -1; // something impossible to force execution of next call
            ActionBoard.initial.perform(data);
        } else {
            data.secGameState = data.previousSecGameState;
            data.previousSecGameState = GameControlData.STATE2_TIMEOUT;
            data.timeOutActive[side] = false;
            Log.setNextMessage("End of Timeout "+Rules.league.teamColorName[data.team[side].teamColor]);
            if (data.secGameState != GameControlData.STATE2_PENALTYSHOOT) {
                ActionBoard.ready.perform(data);
            }
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
      return data.timeOutActive[side]
            || ((data.gameState == GameControlData.STATE_INITIAL ||
                  data.gameState == GameControlData.STATE_READY ||
                  data.gameState == GameControlData.STATE_SET)
                && !data.timeOutTaken[side]
                && !data.timeOutActive[side == 0 ? 1 : 0]
                && !(data.secGameState == GameControlData.STATE2_TIMEOUT))
            || data.testmode;
    }
}