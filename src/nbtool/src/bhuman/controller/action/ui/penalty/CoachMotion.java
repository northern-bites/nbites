package controller.action.ui.penalty;

import common.Log;

import data.AdvancedData;
import data.PlayerInfo;
import data.Rules;

public class CoachMotion extends Penalty
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
        data.whenPenalized[side][number] = data.getTime();
        data.team[side].coach.penalty = PlayerInfo.PENALTY_SPL_COACH_MOTION;
        data.ejected[side][number] = true;
        Log.state(data, "Coach Motion "+ Rules.league.teamColorName[data.team[side].teamColor]);
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
        return Rules.league.isCoachAvailable;
    }
}
