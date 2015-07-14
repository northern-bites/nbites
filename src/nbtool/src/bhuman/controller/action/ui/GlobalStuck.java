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
 * This action means that a global game stuck has occured.
 */
public class GlobalStuck extends GCAction
{
    /** On which side (0:left, 1:right) */
    private int side;
    
    
    /**
     * Creates a new GlobalStuck action.
     * Look at the ActionBoard before using this.
     * 
     * @param side      On which side (0:left, 1:right)
     */
    public GlobalStuck(int side)
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
        data.kickOffTeam = data.team[1 - side].teamNumber;
        Log.setNextMessage("Global Game Stuck, Kickoff "+Rules.league.teamColorName[data.team[data.kickOffTeam == data.team[0].teamNumber ? 0 : 1].teamColor]);
        ActionBoard.ready.perform(data);
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
        return (data.gameState == GameControlData.STATE_PLAYING) || data.testmode;
    }
}