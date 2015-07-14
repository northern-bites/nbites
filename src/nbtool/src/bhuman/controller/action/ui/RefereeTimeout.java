package controller.action.ui;

import common.Log;

import controller.action.ActionBoard;
import controller.action.ActionType;
import controller.action.GCAction;
import data.AdvancedData;
import data.GameControlData;

public class RefereeTimeout extends GCAction
{
    public RefereeTimeout()
    {
        super(ActionType.UI);
    }

    @Override
    public void perform(AdvancedData data)
    {
        if (!data.refereeTimeout) {
            data.previousSecGameState = data.secGameState;
            data.secGameState = GameControlData.STATE2_TIMEOUT;
            data.refereeTimeout = true;
            Log.setNextMessage("Referee Timeout");
            if (data.gameState == GameControlData.STATE_PLAYING) {
                data.addTimeInCurrentState();
            }
            if (data.previousSecGameState == GameControlData.STATE2_PENALTYSHOOT 
                    && (data.gameState == GameControlData.STATE_SET || data.gameState == GameControlData.STATE_PLAYING)) {
                data.team[data.kickOffTeam == data.team[0].teamNumber ? 0 : 1].penaltyShot--;
            }
            
            data.gameState = -1; //something impossible to force execution of next call
            ActionBoard.initial.perform(data);
        } else {
            data.secGameState = data.previousSecGameState;
            data.previousSecGameState = GameControlData.STATE2_TIMEOUT;
            data.refereeTimeout = false;
            Log.setNextMessage("End of Referee Timeout");
            if (data.secGameState != GameControlData.STATE2_PENALTYSHOOT) {
                ActionBoard.ready.perform(data);
            }
        }
    }

    @Override
    public boolean isLegal(AdvancedData data)
    {
        return data.gameState != GameControlData.STATE_FINISHED
                && !data.timeOutActive[0] && !data.timeOutActive[1];
    }

}
