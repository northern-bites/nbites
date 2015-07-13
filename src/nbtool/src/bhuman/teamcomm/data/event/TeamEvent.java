package teamcomm.data.event;

import java.util.Collection;
import java.util.EventObject;
import teamcomm.data.GameState;
import teamcomm.data.RobotState;

/**
 * Class for events being sent when the state of a team changes.
 *
 * @author Felix Thielke
 */
public class TeamEvent extends EventObject {

    private static final long serialVersionUID = 6644539300556793797L;

    /**
     * Side the team is playing on.
     *
     * @see GameState#TEAM_LEFT
     * @see GameState#TEAM_RIGHT
     */
    public final int side;

    /**
     * Number of the team.
     */
    public final int teamNumber;

    /**
     * Robots belonging to the team in the order of their player numbers.
     */
    public final Collection<RobotState> players;

    /**
     * Constructor.
     *
     * @param source source of this event
     * @param side side the team is playing on
     * @param teamNumber number of the team
     * @param players robots belonging to the team in the order of their player
     * numbers
     */
    public TeamEvent(final Object source, final int side, final int teamNumber, final Collection<RobotState> players) {
        super(source);

        this.side = side;
        this.teamNumber = teamNumber;
        this.players = players;
    }
}
