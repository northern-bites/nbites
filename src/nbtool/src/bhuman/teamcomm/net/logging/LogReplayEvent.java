package teamcomm.net.logging;

import java.util.EventObject;

/**
 * Class for events being sent when the LogReplayTask has updated its state.
 *
 * @author Felix Thielke
 */
public class LogReplayEvent extends EventObject {

    private static final long serialVersionUID = 8348108129211449571L;

    /**
     * Position of the replaying in milliseconds.
     */
    public final long timePosition;

    /**
     * Whether the replayer is at the beginning of the log file.
     */
    public final boolean atBeginning;

    /**
     * Whether the replayer is at the end of the log file.
     */
    public final boolean atEnd;

    /**
     * The current speed of the replayer. 0 means that it is paused.
     */
    public final float playbackSpeed;

    /**
     * Constructor.
     *
     * @param source source of this event
     * @param timePosition position of the replaying in milliseconds.
     * @param atBeginning whether the replayer is at the beginning of the log
     * file
     * @param atEnd whether the replayer is at the end of the log file
     * @param playbackSpeed the current speed of the replayer; 0 means that it
     * is paused
     */
    public LogReplayEvent(final Object source, final long timePosition, final boolean atBeginning, final boolean atEnd, final float playbackSpeed) {
        super(source);
        this.timePosition = timePosition;
        this.atBeginning = atBeginning;
        this.atEnd = atEnd;
        this.playbackSpeed = playbackSpeed;
    }
}
