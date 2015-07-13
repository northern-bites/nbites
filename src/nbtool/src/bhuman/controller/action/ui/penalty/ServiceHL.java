package controller.action.ui.penalty;

import common.Log;
import data.AdvancedData;
import data.PlayerInfo;
import data.Rules;

/**
 *
 * @author Daniel Seifert
 */
public class ServiceHL extends PickUp
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
        if (player.penalty == PlayerInfo.PENALTY_NONE) {
            data.whenPenalized[side][number] = data.getTime();
            player.penalty = PlayerInfo.PENALTY_HL_SERVICE;
            Log.state(data, "Request for Service " +
                    Rules.league.teamColorName[data.team[side].teamColor]
                    + " " + (number+1));
        } else {
            player.penalty = PlayerInfo.PENALTY_HL_SERVICE;
            Log.state(data, "Additional Request for Service " +
                    Rules.league.teamColorName[data.team[side].teamColor]
                    + " " + (number+1));
        }
    }
}
