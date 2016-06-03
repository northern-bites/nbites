package teamcomm.net.logging;

import common.Log;
import data.GameControlData;
import java.io.BufferedInputStream;
import java.io.EOFException;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.util.Deque;
import java.util.LinkedList;
import javax.swing.event.EventListenerList;
import teamcomm.data.GameState;
import teamcomm.net.SPLStandardMessagePackage;
import teamcomm.net.SPLStandardMessageReceiver;

/**
 * Task handling the replaying of a log file.
 *
 * @author Felix Thielke
 */
class LogReplayTask implements Runnable {

    public static final int PLAYBACK_TASK_DELAY = 50; // ms

    private static class LoggedObject {

        public final long time;
        public final Object object;
        public final int typeid;

        public LoggedObject(final long time, final Object object) {
            this.time = time;
            this.object = object;
            this.typeid = -1;
        }

        public LoggedObject(final long time, final int typeid) {
            this.time = time;
            this.object = null;
            this.typeid = typeid;
        }
    }

    private final EventListenerList listeners;

    private final Deque<LoggedObject> prevObjects = new LinkedList<>();
    private final Deque<LoggedObject> nextObjects = new LinkedList<>();
    private LoggedObject curObject;

    private ObjectInputStream stream;

    private long currentPosition = 0;
    private float playbackFactor = 0;

    public LogReplayTask(final File logfile, final EventListenerList listeners) throws IOException {
        this.listeners = listeners;
        stream = new ObjectInputStream(new BufferedInputStream(new FileInputStream(logfile)));
        next();
    }

    public void close() {
        if (stream != null) {
            try {
                stream.close();
            } catch (IOException ex) {
            }
            stream = null;
        }
    }

    public boolean isPaused() {
        synchronized (this) {
            return playbackFactor == 0;
        }
    }

    public void setPlaybackSpeed(final float factor) {
        synchronized (this) {
            playbackFactor = factor;
        }
    }

    @Override
    public void run() {
        if (curObject != null) {
            if (!isPaused()) {
                final boolean forward;
                synchronized (this) {
                    forward = playbackFactor > 0;
                    currentPosition += (long) ((float) PLAYBACK_TASK_DELAY * playbackFactor);
                }

                if (forward) {
                    while (currentPosition >= curObject.time) {
                        handleObject(curObject);
                        if (!next()) {
                            synchronized (this) {
                                playbackFactor = 0;
                            }
                            currentPosition = curObject.time;
                            break;
                        }
                    }
                } else {
                    while (currentPosition <= curObject.time) {
                        handleObject(curObject);
                        if (!prev()) {
                            synchronized (this) {
                                playbackFactor = 0;
                            }
                            currentPosition = curObject.time;
                            break;
                        }
                    }
                }
            }

            final LogReplayEvent e;
            synchronized (this) {
                e = new LogReplayEvent(this, currentPosition, prevObjects.isEmpty(), stream == null && nextObjects.isEmpty(), playbackFactor);
            }
            for (final LogReplayEventListener listener : listeners.getListeners(LogReplayEventListener.class)) {
                listener.logReplayStatus(e);
            }
        }
    }

    private boolean prev() {
        if (!prevObjects.isEmpty()) {
            if (curObject != null) {
                nextObjects.push(curObject);
            }
            curObject = prevObjects.pollFirst();
            return true;
        }
        return false;
    }

    private boolean next() {
        LoggedObject obj = nextObjects.pollFirst();
        if (obj == null && stream != null) {
            try {
                final long time = stream.readLong();
                if (stream.readBoolean()) {
                    obj = new LoggedObject(time, stream.readObject());
                } else {
                    obj = new LoggedObject(time, stream.readInt());
                }
            } catch (EOFException e) {
                try {
                    stream.close();
                } catch (IOException ex) {
                }
                stream = null;
            } catch (IOException | ClassNotFoundException e) {
                Log.error("error while reading log file: " + e.getClass().getSimpleName() + ": " + e.getMessage());
                try {
                    stream.close();
                } catch (IOException ex) {
                }
                stream = null;
            }
        }
        if (obj != null) {
            if (curObject != null) {
                prevObjects.push(curObject);
            }
            curObject = obj;
            return true;
        }
        return false;
    }

    private void handleObject(final LoggedObject obj) {
        if (obj.object != null) {
            if (obj.object instanceof SPLStandardMessagePackage) {
                SPLStandardMessageReceiver.getInstance().addToPackageQueue((SPLStandardMessagePackage) obj.object);
            } else if (obj.object instanceof GameControlData) {
                GameState.getInstance().updateGameData((GameControlData) obj.object);
            }
        } else {
            switch (obj.typeid) {
                case 1:
                    GameState.getInstance().updateGameData(null);
                    break;
            }
        }
    }
}
