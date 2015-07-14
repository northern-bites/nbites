package controller.action;

import controller.EventHandler;
import data.AdvancedData;
import data.PlayerInfo;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;


/**
 * @author Michel Bartsch
 * 
 * This is an abstract class every action needs to extend to provide the
 * basic features. By extending this it also becomes an ActionListener,
 * wich is needed to add them to GUI buttons.
 */
public abstract class GCAction implements ActionListener
{
    /** The type of an action to easily distinguish them. */
    public ActionType type;
    
    /**
     * Creates a new GCAction.
     * 
     * @param type      The type of the action.
     */
    public GCAction(ActionType type)
    {
        this.type = type;
    }
    
    /**
     * This gets called when the button an action is added to was pushed or
     * if the action is called otherwise.
     * The action`s perform method will not be executed right away but
     * later in the GUI`s thread.
     * 
     * @param e      The event that happened, but this is ignored.
     */
    @Override
    public void actionPerformed(ActionEvent e)
    {
        EventHandler.getInstance().register(this);
    }
    
    /**
     * This is the essential method of each action.
     * It is called automatically after the actionPerformed method was called.
     * Here you can manipulate the given data without worrying.
     * 
     * @param data      The current data to work on.
     */
    public abstract void perform(AdvancedData data);
    
    /**
     * This is to perform an action on a specific player. It is not needed
     * to override this if not needed.
     * 
     * @param data      The current data to work on.
     * @param player    The player on which the action is to be performed.
     * @param side      The side this player is playing for, 0: left, 1: right
     * @param number    The players number, beginning at 0!
     */
    public void performOn(AdvancedData data, PlayerInfo player, int side, int number) {}
    
    /**
     * Must be override to determine if the action is legal at a specific
     * state of the game.
     * Actions that are not legal will not be executed by the EventHandler.
     * 
     * @param data      The current data to calculate the legality by.
     * 
     * @return This is true if the action is legal.
     */
    public abstract boolean isLegal(AdvancedData data);
}