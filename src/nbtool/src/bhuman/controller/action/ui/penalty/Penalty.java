package controller.action.ui.penalty;

import controller.EventHandler;
import controller.action.ActionType;
import controller.action.GCAction;
import data.AdvancedData;
import data.PlayerInfo;
import data.Rules;

/**
 *
 * @author Michel-Zen
 */
public abstract class Penalty extends GCAction
{
    /**
     * Creates a new Defender action.
     * Look at the ActionBoard before using this.
     */
    public Penalty()
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
        if (EventHandler.getInstance().lastUIEvent == this) {
            EventHandler.getInstance().noLastUIEvent = true;
        }
    }

    /**
     * Performs an ejection if the robot exceeds limit on repeated penalties
     *
     * @param data      The current data to work on.
     * @param player    The already penalised player.
     * @param side      The side the player is playing on (0:left, 1:right).
     * @param number    The player`s number, beginning with 0!
     */
    protected void handleRepeatedPenaltyEjection(final AdvancedData data, final PlayerInfo player,
            final int side, final int number, final int... states) {
        if (containsState(states, data.gameState)) {
            data.penaltyCount[side][number][player.penalty]++;
            final int penaltyTime = Rules.league.penaltyTime[player.penalty][((data.penaltyCount[side][number][player.penalty] > Rules.league.penaltyTime[player.penalty].length)
                    ? Rules.league.penaltyTime[player.penalty].length
                    : data.penaltyCount[side][number][player.penalty]) - 1];
            if (penaltyTime >= Rules.league.halfTime * 2) {
                data.ejected[side][number] = true;
            }
        }
    }

    /** may get removed if not used */
    private boolean containsState(final int[] states, final int state) {
        if(states == null || states.length == 0) {
            return true;
        }
        for(final int s : states) {
            if(s==state) {
                return true;
            }
        }
        return false;
    }
}