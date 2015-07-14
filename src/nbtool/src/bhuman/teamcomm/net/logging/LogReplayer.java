package teamcomm.net.logging;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.ScheduledFuture;
import java.util.concurrent.TimeUnit;
import javax.swing.event.EventListenerList;
import teamcomm.data.GameState;
import teamcomm.net.SPLStandardMessageReceiver;

/**
 * Singleton class for replaying log files.
 *
 * @author Felix Thielke
 */
public class LogReplayer {

    private static final LogReplayer instance = new LogReplayer();

    private final ScheduledExecutorService scheduler = Executors.newScheduledThreadPool(1);
    private ScheduledFuture<?> taskHandle;

    private LogReplayTask task;

    private final EventListenerList listeners = new EventListenerList();

    /**
     * Returns the only instance of this class.
     *
     * @return instance
     */
    public static LogReplayer getInstance() {
        return instance;
    }

    /**
     * Opens a log file.
     *
     * @param logfile file
     * @throws FileNotFoundException if the file could not be found
     * @throws IOException if an other I/O error happened
     */
    public void open(final File logfile) throws FileNotFoundException, IOException {
        // Close currently opened log
        if (task != null && taskHandle != null) {
            taskHandle.cancel(false);
            task.close();
            for (final LogReplayEventListener listener : listeners.getListeners(LogReplayEventListener.class)) {
                listener.logReplayEnded();
            }
        }

        // Drain package queue of SPLStandardMessageReceiver
        SPLStandardMessageReceiver.getInstance().clearPackageQueue();

        // Reset GameState
        GameState.getInstance().reset();

        // Open new log
        task = new LogReplayTask(logfile, listeners);
        taskHandle = scheduler.scheduleAtFixedRate(task, LogReplayTask.PLAYBACK_TASK_DELAY, LogReplayTask.PLAYBACK_TASK_DELAY, TimeUnit.MILLISECONDS);
        for (final LogReplayEventListener listener : listeners.getListeners(LogReplayEventListener.class)) {
            listener.logReplayStarted();
        }
    }

    /**
     * Returns whether a log file is currently opened.
     *
     * @return boolean
     */
    public boolean isReplaying() {
        return task != null;
    }

    /**
     * Returns whether the replay of a log file iis currently closed.
     *
     * @return boolean
     */
    public boolean isPaused() {
        return task == null || task.isPaused();
    }

    /**
     * Sets the speed of the playback.
     *
     * @param factor playback speed. 1 is normal speed, 0 is paused.
     */
    public void setPlaybackSpeed(final float factor) {
        if (task != null) {
            task.setPlaybackSpeed(factor);
        }
    }

    /**
     * Closes the currently opened log file.
     */
    public void close() {
        if (task != null) {
            // Close currently opened log
            taskHandle.cancel(false);
            task.close();
            task = null;
            taskHandle = null;

            // Send close event
            for (final LogReplayEventListener listener : listeners.getListeners(LogReplayEventListener.class)) {
                listener.logReplayEnded();
            }

            // Drain package queue of SPLStandardMessageReceiver
            SPLStandardMessageReceiver.getInstance().clearPackageQueue();

            // Reset GameState
            GameState.getInstance().reset();
        }
    }

    /**
     * Adds a listener to receive events about log replaying.
     *
     * @param listener listener
     * @see LogReplayEvent
     */
    public void addListener(final LogReplayEventListener listener) {
        listeners.add(LogReplayEventListener.class, listener);
    }

    /**
     * Removes an event listener.
     *
     * @param listener listener
     */
    public void removeListener(final LogReplayEventListener listener) {
        listeners.remove(LogReplayEventListener.class, listener);
    }
}
