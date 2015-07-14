package teamcomm.data.event;

import java.util.EventListener;

/**
 * Interface for listeners for events being sent when the state of a team
 * changes.
 *
 * @author Felix Thielke
 */
public interface TeamEventListener extends EventListener {

    /**
     * Called when the state of a team changes.
     *
     * @param e event
     */
    public void teamChanged(TeamEvent e);
}
