package controller;

import controller.action.ActionBoard;

/**
 *
 * @author Michel Bartsch
 *
 * This class is no thread, it continous the main-thread and fires the action ClockTick.
 */
public class Clock
{
    /**
     * The time in millis to sleep before next ClockTick. This does not mean
     * it fires after this time, it will allways take some more millis depending
     * on the performance.
     */
    public static final int HEARTBEAT = 500; // 2Hz

    /** The instance of the singleton. */
    private static Clock instance;

    /** The thread of this clock. */
    private Thread thread;
    
    /**
     * Returns the instance of the singleton. If the Clock wasn't initialized once before, a new instance will
     * be created and returned (lazy instantiation)
     *
     * @return  The instance of the Clock
     */
    public static Clock getInstance()
    {
        if (instance == null) {
            instance = new Clock();
        }
        return instance;
    }
    /**
     * Lets the Clock start to run.
     */
    public void start()
    {
        thread = Thread.currentThread();
        while (!thread.isInterrupted())
        {
            ActionBoard.clock.actionPerformed(null);
            
            try {
                Thread.sleep(HEARTBEAT);
            } catch (InterruptedException e) {
                thread.interrupt();
            }
        }
    }

    public void interrupt()
    {
        thread.interrupt();
    }
}