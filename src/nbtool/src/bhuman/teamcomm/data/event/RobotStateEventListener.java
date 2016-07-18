package teamcomm.data.event;

import java.util.EventListener;

/**
 * Interface for listeners for events being sent when the state of a robot
 * changes.
 *
 * @author Felix Thielke
 */
public interface RobotStateEventListener extends EventListener {

    /**
     * Called when the state of the robot which sent this event changes.
     *
     * @param e event
     */
    public void robotStateChanged(RobotStateEvent e);

    /**
     * Called when the state of the robot which sent this event changes.
     *
     * @param e event
     */
    public void connectionStatusChanged(RobotStateEvent e);
}
