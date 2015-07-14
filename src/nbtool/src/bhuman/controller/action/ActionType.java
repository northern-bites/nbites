package controller.action;


/**
 * Each action has such a type to easily distinguish between them.
 */
public enum ActionType
{
    /** It is a clock action, actually only the ClockTick has this */
    CLOCK,
    /** It is an actions executed by an user interface. */
    UI,
    /** The action is executed by network events. */
    NET
}