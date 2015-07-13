package controller.action.net;

import common.Log;
import controller.action.ActionType;
import controller.action.GCAction;
import data.AdvancedData;
import data.GameControlData;
import data.PlayerInfo;
import data.Rules;

/**
 * @author Michel Bartsch
 * 
 * This action means that a player has been penalised or unpenalised manually
 * via the chest button.
 */
public class Manual extends GCAction
{
    /** On which side (0:left, 1:right) */
    private int side;
    /** The players`s number, beginning with 0! */
    private int number;
    /** If true, this action means manual unpenalising, otherwise manual penalising.  */
    private boolean unpen;
    
    /**
     * Creates a new Manual action.
     * Look at the ActionBoard before using this.
     * 
     * @param side      On which side (0:left, 1:right)
     * @param number    The players`s number, beginning with 0!
     * @param unpen     If true, this action means manual unpenalising,
     *                  otherwise manual penalising.
     */
    public Manual(int side, int number, boolean unpen)
    {
        super(ActionType.NET);
        this.side = side;
        this.number = number;
        this.unpen = unpen;
    }

    /**
     * Performs this action to manipulate the data (model).
     * 
     * @param data      The current data to work on.
     */
    @Override
    public void perform(AdvancedData data)
    {
        if (!unpen) {
            data.team[side].player[number].penalty = PlayerInfo.PENALTY_MANUAL;
            data.whenPenalized[side][number] = data.getTime();
            if((data.gameState != GameControlData.STATE_INITIAL) 
                    && (data.gameState != GameControlData.STATE_FINISHED)){
                Log.state(data, "Manually Penalised "+
                        Rules.league.teamColorName[data.team[side].teamColor]
                        + " " + (number+1));
            }
        } else {
            data.team[side].player[number].penalty = PlayerInfo.PENALTY_NONE;
            if((data.gameState != GameControlData.STATE_INITIAL) 
                    && (data.gameState != GameControlData.STATE_FINISHED)){
                Log.state(data, "Manually Unpenalised "+
                       Rules.league.teamColorName[data.team[side].teamColor]
                       + " " + (number+1));    
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
        return true;
    }
}