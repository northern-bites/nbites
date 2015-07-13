package controller.action.clock;

import controller.action.ActionBoard;
import controller.action.ActionType;
import controller.action.GCAction;
import data.AdvancedData;
import data.GameControlData;
import data.Rules;


/**
 * @author Michel Bartsch
 * 
 * This action means that some time has been passed.
 */
public class ClockTick extends GCAction
{
    /**
     * Creates a new ClockTick action.
     * Look at the ActionBoard before using this.
     */
    public ClockTick()
    {
        super(ActionType.CLOCK);
    }

    /**
     * Performs this action to manipulate the data (model).
     * 
     * @param data      The current data to work on.
     */
    @Override
    public void perform(AdvancedData data)
    {
        if (data.gameState == GameControlData.STATE_READY
               && data.getSecondsSince(data.whenCurrentGameStateBegan) >= Rules.league.readyTime) {
            ActionBoard.set.perform(data);
        } else if (data.gameState == GameControlData.STATE_FINISHED) {
            Integer remainingPauseTime = data.getRemainingPauseTime();
            if (remainingPauseTime != null) {
                if (data.firstHalf == GameControlData.C_TRUE && remainingPauseTime <= Rules.league.pauseTime / 2) {
                    ActionBoard.secondHalf.perform(data);
                } else if (data.firstHalf != GameControlData.C_TRUE && remainingPauseTime <= Rules.league.pausePenaltyShootOutTime / 2) {
                    ActionBoard.penaltyShoot.perform(data);
                }
            }
        }
        data.updateCoachMessages();
        data.updatePenalties();
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
    
    public boolean isClockRunning(AdvancedData data)
    {
        boolean halfNotStarted = data.timeBeforeCurrentGameState == 0 && data.gameState != GameControlData.STATE_PLAYING;
        return !((data.gameState == GameControlData.STATE_INITIAL)
         || (data.gameState == GameControlData.STATE_FINISHED)
         || (
                ((data.gameState == GameControlData.STATE_READY)
               || (data.gameState == GameControlData.STATE_SET))
                && (((data.gameType == GameControlData.GAME_PLAYOFF) && Rules.league.playOffTimeStop) || halfNotStarted)
                )
         || data.manPause)
         || data.manPlay;
    }
}