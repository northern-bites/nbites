package teamcomm.data.event;

import java.util.EventObject;

/**
 * Class for events being sent when the state of a robot changes.
 *
 * @author Felix Thielke
 */
public class RobotStateEvent extends EventObject {

    private static final long serialVersionUID = 5732929692893474554L;

    /**
     * Constructor.
     *
     * @param source the source of this event
     */
    public RobotStateEvent(Object source) {
        super(source);
    }
}
