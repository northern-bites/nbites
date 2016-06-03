package teamcomm.data;

import teamcomm.data.event.RobotStateEvent;
import teamcomm.data.event.RobotStateEventListener;
import data.PlayerInfo;
import data.SPLStandardMessage;
import java.util.LinkedList;
import java.util.ListIterator;
import javax.swing.event.EventListenerList;

/**
 * Class representing the state of a robot.
 *
 * @author Felix Thielke
 */
public class RobotState {

    public static enum ConnectionStatus {

        INACTIVE(10000),
        OFFLINE(2000),
        HIGH_LATENCY(500),
        ONLINE(0);

        public final int threshold;

        private ConnectionStatus(final int threshold) {
            this.threshold = threshold;
        }
    }

    private static final int AVERAGE_CALCULATION_TIME = 10000;

    private final String address;
    private SPLStandardMessage lastMessage;
    private long lastMessageTimestamp;
    private final LinkedList<Long> recentMessageTimestamps = new LinkedList<>();
    private int messageCount = 0;
    private int illegalMessageCount = 0;
    private final int teamNumber;
    private Integer playerNumber = null;
    private byte penalty = PlayerInfo.PENALTY_NONE;
    private ConnectionStatus lastConnectionStatus = ConnectionStatus.ONLINE;

    private final EventListenerList listeners = new EventListenerList();

    /**
     * Constructor.
     *
     * @param address IP address of the robot
     * @param teamNumber team number associated with the port the robot sends
     * his messages on
     */
    public RobotState(final String address, final int teamNumber) {
        this.address = address;
        this.teamNumber = teamNumber;
    }

    /**
     * Handles a message received by the robot this object corresponds to.
     *
     * @param message received message or null if the message was invalid
     */
    public void registerMessage(final SPLStandardMessage message) {
        if (!message.valid) {
            illegalMessageCount++;
        }
        lastMessage = message;
        if (message.playerNumValid) {
            playerNumber = (int) message.playerNum;
        }
        lastMessageTimestamp = System.currentTimeMillis();
        synchronized (recentMessageTimestamps) {
            recentMessageTimestamps.addFirst(lastMessageTimestamp);
        }
        messageCount++;

        for (final RobotStateEventListener listener : listeners.getListeners(RobotStateEventListener.class)) {
            listener.robotStateChanged(new RobotStateEvent(this));
            listener.connectionStatusChanged(new RobotStateEvent(this));
        }
    }

    /**
     * Returns the IP address of the robot.
     *
     * @return IP address
     */
    public String getAddress() {
        return address;
    }

    /**
     * Returns the most recent legal message received from this robot.
     *
     * @return message
     */
    public SPLStandardMessage getLastMessage() {
        return lastMessage;
    }

    /**
     * Returns the average number of messages per second.
     *
     * @return number of messages per second
     */
    public double getMessagesPerSecond() {
        synchronized (recentMessageTimestamps) {
            final ListIterator<Long> it = recentMessageTimestamps.listIterator(recentMessageTimestamps.size());

            final long curTime = System.currentTimeMillis();
            while (curTime - it.previous() > AVERAGE_CALCULATION_TIME) {
                it.remove();
            }

            return recentMessageTimestamps.size() > 0 ? (recentMessageTimestamps.size() * 1000.0 / Math.max(1000, curTime - recentMessageTimestamps.getLast())) : 0;
        }
    }

    /**
     * Updates the current network status of the robot internally. Sends events
     * about a change of the connection status if needed.
     *
     * @return the current connection status
     */
    public ConnectionStatus updateConnectionStatus() {
        final ConnectionStatus c = getConnectionStatus();
        if (c != lastConnectionStatus) {
            lastConnectionStatus = c;
            for (final RobotStateEventListener listener : listeners.getListeners(RobotStateEventListener.class)) {
                listener.connectionStatusChanged(new RobotStateEvent(this));
            }
        }
        return c;
    }

    /**
     * Returns the current network status of the robot.
     *
     * @return connection status
     */
    public ConnectionStatus getConnectionStatus() {
        final long timeSinceLastMessage = System.currentTimeMillis() - lastMessageTimestamp;
        for (final ConnectionStatus c : ConnectionStatus.values()) {
            if (timeSinceLastMessage >= c.threshold) {
                return c;
            }
        }

        return ConnectionStatus.ONLINE;
    }

    /**
     * Returns the total count of received messages.
     *
     * @return total message count
     */
    public int getMessageCount() {
        return messageCount;
    }

    /**
     * Returns the total count of illegal messages.
     *
     * @return illegal messasge count
     */
    public int getIllegalMessageCount() {
        return illegalMessageCount;
    }

    /**
     * Returns the ratio of illegal messages to the total count of messages.
     *
     * @return ratio
     */
    public double getIllegalMessageRatio() {
        return (double) illegalMessageCount / (double) messageCount;
    }

    /**
     * Returns the team number of this robot.
     *
     * @return team number
     */
    public int getTeamNumber() {
        return teamNumber;
    }

    /**
     * Returns the player number of the robot or null if it did not send any.
     *
     * @return player number or null
     */
    public Integer getPlayerNumber() {
        return playerNumber;
    }

    /**
     * Returns the current penalty of the robot.
     *
     * @return penalty
     * @see PlayerInfo#penalty
     */
    public byte getPenalty() {
        return penalty;
    }

    /**
     * Sets the current penalty of the robot.
     *
     * @param penalty penalty
     * @see PlayerInfo#penalty
     */
    public void setPenalty(final byte penalty) {
        this.penalty = penalty;
    }

    /**
     * Registeres a GUI component as a listener receiving events when this robot
     * sends a message.
     *
     * @param listener component
     */
    public void addListener(final RobotStateEventListener listener) {
        listeners.add(RobotStateEventListener.class, listener);
    }

    /**
     * Removes an event listener from this robot.
     *
     * @param listener listener
     */
    public void removeListener(final RobotStateEventListener listener) {
        listeners.remove(RobotStateEventListener.class, listener);
    }
}
