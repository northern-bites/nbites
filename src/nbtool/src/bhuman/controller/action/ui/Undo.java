package controller.action.ui;

import common.Log;
import controller.EventHandler;
import controller.action.ActionType;
import controller.action.GCAction;
import data.AdvancedData;


/**
 * @author Michel Bartsch
 * 
 * This action means that the operator wants to go back in the timeline.
 */
public class Undo extends GCAction
{
    /** This is true, if this action has just been executed */
    public boolean executed = false;
    /** How far to go back in the timeline by this action. */
    private int states;
    
    
    /**
     * Creates a new Undo action.
     * Look at the ActionBoard before using this.
     * 
     * @param states        How far to go back in the timeline by this action.
     */
    public Undo(int states)
    {
        super(ActionType.UI);
        this.states = states;
    }

    /**
     * Performs this action to manipulate the data (model).
     * 
     * @param data      The current data to work on.
     */
    @Override
    public void perform(AdvancedData data)
    {
        if ((EventHandler.getInstance().lastUIEvent == this)
         && (!executed) ) {
            executed= true;
            Log.toFile("Undo "+states+" States to "+Log.goBack(states));
        } else {
            executed = false;
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