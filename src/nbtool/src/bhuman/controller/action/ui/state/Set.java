package controller.action.ui.state;

import common.Log;
import controller.action.ActionType;
import controller.action.GCAction;
import controller.action.ui.half.FirstHalf;
import data.AdvancedData;
import data.GameControlData;
import data.Rules;


/**
 * @author Michel Bartsch
 * 
 * This action means that the state is to be set to set.
 */
public class Set extends GCAction
{
    /**
     * Creates a new Set action.
     * Look at the ActionBoard before using this.
     */
    public Set()
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
        if (data.gameState == GameControlData.STATE_SET) {
            return;
        }
        if (Rules.league.returnRobotsInGameStoppages) {
            data.resetPenaltyTimes();
        }
        if ((data.gameType != GameControlData.GAME_PLAYOFF) && data.timeBeforeCurrentGameState != 0) {
            data.addTimeInCurrentState();
        }
        data.whenCurrentGameStateBegan = data.getTime();
        
        if (data.secGameState == GameControlData.STATE2_PENALTYSHOOT) {
            data.timeBeforeCurrentGameState = 0;
            if (data.gameState != GameControlData.STATE_INITIAL) {
                data.kickOffTeam = data.team[data.kickOffTeam == data.team[0].teamNumber ? 1 : 0].teamNumber;
                FirstHalf.changeSide(data);
            }

            if (data.gameState != GameControlData.STATE_PLAYING) {
                data.team[data.team[0].teamColor == data.kickOffTeam ? 0 : 1].penaltyShot++;
            }
        }
        data.gameState = GameControlData.STATE_SET;
        Log.state(data, "Set");
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
        return (data.gameState == GameControlData.STATE_READY)
            || (data.gameState == GameControlData.STATE_SET)
            || ((data.secGameState == GameControlData.STATE2_PENALTYSHOOT)
              && ((data.gameState != GameControlData.STATE_PLAYING)
                || (Rules.league.penaltyShotRetries))
              && !data.timeOutActive[0]
              && !data.timeOutActive[1]
              && !data.refereeTimeout)
            || data.testmode;
    }
}