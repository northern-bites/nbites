package controller.action.ui.penalty;

import common.Log;
import data.AdvancedData;
import data.GameControlData;
import data.PlayerInfo;
import data.Rules;

/**
 * @author Michel Bartsch
 * 
 * This action means that the player pushing penalty has been selected.
 */
public class Pushing extends Penalty
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
        player.penalty = PlayerInfo.PENALTY_SPL_PLAYER_PUSHING;
        handleRepeatedPenaltyEjection(data, player, side, number);
        data.whenPenalized[side][number] = data.getTime();
        /*//not required in RoboCup 2016?
        if (data.gameState == GameControlData.STATE_PLAYING) {
            data.pushes[side]++;
            for (int pushes : Rules.league.pushesToEjection) {
                if (data.pushes[side] == pushes) {
                    data.ejected[side][number] = true;
                }
            }
        }*/
        
        Log.state(data, "Player Pushing "+
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
        return (data.gameState == GameControlData.STATE_READY)
            || (data.gameState == GameControlData.STATE_PLAYING) 
            || (data.testmode);
    }
}