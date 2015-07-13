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
 * This action means that a team has scored or it`s score is to be decreased.
 */
public class Goal extends GCAction
{
    /** On which side (0:left, 1:right) */
    private int side;
    /** This value will be added to the score. */
    private int set;
    
    
    /**
     * Creates a new Goal action.
     * Look at the ActionBoard before using this.
     * 
     * @param side      On which side (0:left, 1:right)
     * @param set       This value will be added to the score.
     */
    public Goal(int side, int set)
    {
        super(ActionType.UI);
        this.side = side;
        this.set = set;
    }

    /**
     * Performs this action to manipulate the data (model).
     * 
     * @param data      The current data to work on.
     */
    @Override
    public void perform(AdvancedData data)
    {
        data.team[side].score += set;
        if (set == 1) {
            if (data.secGameState != GameControlData.STATE2_PENALTYSHOOT) {
                data.kickOffTeam = data.team[1 - side].teamNumber;
                Log.setNextMessage("Goal for "+Rules.league.teamColorName[data.team[side].teamColor]);
                ActionBoard.ready.perform(data);
            } else {
                data.team[side].singleShots += (1<<(data.team[side].penaltyShot-1));
                Log.setNextMessage("Goal for "+Rules.league.teamColorName[data.team[side].teamColor]);
                ActionBoard.finish.perform(data);
            }
        } else {
            Log.state(data, "Goal decrease for "+Rules.league.teamColorName[data.team[side].teamColor]);
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
        return ((set == 1)
              && (data.gameState == GameControlData.STATE_PLAYING)
              && ( (data.secGameState != GameControlData.STATE2_PENALTYSHOOT)
                || (data.kickOffTeam == data.team[side].teamNumber)) )
            || data.testmode;
    }
}