package teamcomm.net;

import java.io.Serializable;

/**
 * Class for a message. Instances of this class are stored in the log files.
 *
 * @author Felix Thielke
 */
public class SPLStandardMessagePackage implements Serializable {

    private static final long serialVersionUID = 758311663011901849L;

    /**
     * Host address from which this message was received.
     */
    public final String host;

    /**
     * Number of the team to which the port belongs on which this message was
     * received.
     */
    public final int team;

    /**
     * Raw message data.
     */
    public final byte[] message;

    /**
     * Constructor.
     *
     * @param host host address from which this message was received
     * @param team number of the team to which the port belongs on which this
     * message was received
     * @param message raw message data
     */
    public SPLStandardMessagePackage(final String host, final int team, final byte[] message) {
        this.host = host;
        this.team = team;
        this.message = message;
    }

}
