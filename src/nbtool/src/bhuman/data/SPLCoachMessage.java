package data;

import java.io.Serializable;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import controller.EventHandler;

public class SPLCoachMessage implements Serializable
{
    private static final long serialVersionUID = -1586200167710301614L;

    /** Some constants from the C-structure. */
    public static final int SPL_COACH_MESSAGE_PORT = 3839;
    public static final String SPL_COACH_MESSAGE_STRUCT_HEADER = "SPLC";
    public static final int SPL_COACH_MESSAGE_STRUCT_VERSION = 3;
    public static final int SPL_COACH_MESSAGE_SIZE = 81;
    public static final long SPL_COACH_MESSAGE_RECEIVE_INTERVALL = 10000; // in ms
    public static final long SPL_COACH_MESSAGE_MIN_SEND_INTERVALL = 0; // in ms
    public static final long SPL_COACH_MESSAGE_MAX_SEND_INTERVALL = 0; // in ms
    public static final int SIZE = 4 // header size
                                   + 1 // byte for the version
                                   + 1 // team number
                                   + 1 // sequence number
                                   + SPL_COACH_MESSAGE_SIZE;

    public String header;   // header to identify the structure
    public byte version;    // version of the data structure
    public byte team;       // unique team number
    public byte sequence;   // sequence number of this message (set by the coach)
    public byte[] message;  // what the coach says
    private long sendTime;  // delay in ms that the message will be held back

    public SPLCoachMessage()
    {
        sendTime = generateSendIntervallForSPLCoachMessage() + System.currentTimeMillis();
    }

    public byte[] toByteArray()
    {
        ByteBuffer buffer = ByteBuffer.allocate(SIZE);
        buffer.order(ByteOrder.LITTLE_ENDIAN);

        buffer.put(header.getBytes());
        buffer.put(version);
        buffer.put(team);
        buffer.put(sequence);
        buffer.put(message);

        return buffer.array();
    }

    public boolean fromByteArray(ByteBuffer buffer)
    {
        try {
            buffer.order(ByteOrder.LITTLE_ENDIAN);
            byte[] header = new byte[4];
            buffer.get(header);
            this.header = new String(header);
            if (!this.header.equals(SPL_COACH_MESSAGE_STRUCT_HEADER)) {
                return false;
            } else {
                version = buffer.get();
                if (version != SPL_COACH_MESSAGE_STRUCT_VERSION) {
                    return false;
                } else {
                    team = buffer.get();
                    if ((team != EventHandler.getInstance().data.team[0].teamNumber) && (team != EventHandler.getInstance().data.team[1].teamNumber)) {
                        return false;
                    }
                    sequence = buffer.get();
                    message = new byte[SPLCoachMessage.SPL_COACH_MESSAGE_SIZE];
                    buffer.get(message);
                    return true;
                }
            }
        } catch (RuntimeException e) {
            return false;
        }
    }

    public long getRemainingTimeToSend()
    {
        long remainingTime = sendTime - System.currentTimeMillis();
        return remainingTime > 0 ? remainingTime : 0;
    }

    private long generateSendIntervallForSPLCoachMessage()
    {
        return (long) (Math.random() * (SPLCoachMessage.SPL_COACH_MESSAGE_MAX_SEND_INTERVALL
                - SPLCoachMessage.SPL_COACH_MESSAGE_MIN_SEND_INTERVALL))
                + SPLCoachMessage.SPL_COACH_MESSAGE_MIN_SEND_INTERVALL;
    }
}
