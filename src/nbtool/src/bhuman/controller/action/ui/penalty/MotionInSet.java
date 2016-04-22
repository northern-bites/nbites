package controller.action.ui.penalty;

import common.Log;
import data.AdvancedData;
import data.GameControlData;
import data.PlayerInfo;
import data.Rules;

/**
 * @author Michel Bartsch
 * 
 * This action means that the illegal motion in set robot penalty has been selected.
 */
public class MotionInSet extends Penalty
{
    /**
     * Performs this action`s penalty on a selected player.
     * 
     * @param data      The current data to work on.
     * @param player    The player to penalise.
     * @param side      The side the player is playing on (0:left, 1:right).
     * @param number    The player`s number, beginning with 0!
     */
    @Override
    public void performOn(AdvancedData data, PlayerInfo player, int side, int number)
    {
        player.penalty = PlayerInfo.PENALTY_SPL_ILLEGAL_MOTION_IN_SET;
        handleRepeatedPenaltyEjection(data, player, side, number);
        data.whenPenalized[side][number] = data.getTime();
        Log.state(data, "Illegal Motion in Set "+
                Rules.league.teamColorName[data.team[side].teamColor]
                + " " + (number+1));
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
        return (data.gameType == GameControlData.GAME_PLAYOFF
                && data.secGameState == GameControlData.STATE2_NORMAL
                && data.gameState == GameControlData.STATE_SET)
                || data.testmode;
    }
}