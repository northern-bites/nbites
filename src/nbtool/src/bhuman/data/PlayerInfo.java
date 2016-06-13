package data;

import java.io.Serializable;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;


/**
 * @author Michel Bartsch
 * 
 * This class is part of the data wich are send to the robots.
 * It just represents this data, reads and writes between C-structure and
 * Java, nothing more.
 */
public class PlayerInfo implements Serializable
{
    private static final long serialVersionUID = -8480462279073509072L;

    /** What type of penalty a player may have. */
    public static final byte PENALTY_NONE = 0;

    public static final byte PENALTY_SPL_ILLEGAL_BALL_CONTACT = 1;
    public static final byte PENALTY_SPL_PLAYER_PUSHING = 2;
    public static final byte PENALTY_SPL_ILLEGAL_MOTION_IN_SET = 3;
    public static final byte PENALTY_SPL_INACTIVE_PLAYER = 4;
    public static final byte PENALTY_SPL_ILLEGAL_DEFENDER = 5;
    public static final byte PENALTY_SPL_LEAVING_THE_FIELD = 6;
    public static final byte PENALTY_SPL_KICK_OFF_GOAL = 7;
    public static final byte PENALTY_SPL_REQUEST_FOR_PICKUP = 8;
    public static final byte PENALTY_SPL_COACH_MOTION = 9;

    public static final byte PENALTY_HL_BALL_MANIPULATION = 1;
    public static final byte PENALTY_HL_PHYSICAL_CONTACT = 2;
    public static final byte PENALTY_HL_ILLEGAL_ATTACK = 3;
    public static final byte PENALTY_HL_ILLEGAL_DEFENSE = 4;
    public static final byte PENALTY_HL_PICKUP_OR_INCAPABLE = 5;
    public static final byte PENALTY_HL_SERVICE = 6;

    public static final byte PENALTY_SUBSTITUTE = 14;
    public static final byte PENALTY_MANUAL = 15;

    /** The size in bytes this class has packed. */
    public static final int SIZE =
            1 + // penalty
            1;  // secsToUnpen

    /** The size in bytes this class has packed in version 7. */
    public static final int SIZE7 =
            2 + // penalty
            2;  // secsToUnpen

    //this is streamed
    public byte penalty = PENALTY_NONE; // penalty state of the player
    public byte secsTillUnpenalised;    // estimate of time till unpenalised

    /**
     * Packing this Java class to the C-structure to be send.
     * @return Byte array representing the C-structure.
     */
    public byte[] toByteArray()
    {
        ByteBuffer buffer = ByteBuffer.allocate(SIZE);
        buffer.order(ByteOrder.LITTLE_ENDIAN);
        buffer.put(penalty);
        buffer.put(secsTillUnpenalised);
        return buffer.array();
    }

    /**
     * Packing this Java class to the C-structure to be send, using version 7
     * of the protocol.
     * @return Byte array representing the C-structure.
     */
    public byte[] toByteArray7()
    {
        ByteBuffer buffer = ByteBuffer.allocate(SIZE7);
        buffer.order(ByteOrder.LITTLE_ENDIAN);
        buffer.putShort(penalty);
        buffer.putShort(secsTillUnpenalised);
        return buffer.array();
    }

    /**
     * Unpacking the C-structure to the Java class.
     * 
     * @param buffer    The buffered C-structure.
     */
    public void fromByteArray(ByteBuffer buffer)
    {
        buffer.order(ByteOrder.LITTLE_ENDIAN);
        penalty = buffer.get();
        secsTillUnpenalised = buffer.get();
    }

    @Override
    public String toString()
    {
        String out = "----------------------------------------\n";
        String temp;

        if (Rules.league instanceof SPL) {
            switch (penalty) {
                case PENALTY_NONE:                      temp = "none"; break;
                case PENALTY_SPL_ILLEGAL_BALL_CONTACT:  temp = "illegal ball contact"; break;
                case PENALTY_SPL_PLAYER_PUSHING:        temp = "pushing"; break;
                case PENALTY_SPL_ILLEGAL_MOTION_IN_SET: temp = "illegal motion in set"; break;
                case PENALTY_SPL_INACTIVE_PLAYER:       temp = "inactive"; break;
                case PENALTY_SPL_ILLEGAL_DEFENDER:      temp = "illegal defender"; break;
                case PENALTY_SPL_LEAVING_THE_FIELD:     temp = "leaving the field"; break;
                case PENALTY_SPL_KICK_OFF_GOAL:         temp = "kickoff goal"; break;
                case PENALTY_SPL_REQUEST_FOR_PICKUP:    temp = "request for pickup"; break;
                case PENALTY_SPL_COACH_MOTION:          temp = "coach motion"; break;
                case PENALTY_SUBSTITUTE:                temp = "substitute"; break;
                case PENALTY_MANUAL:                    temp = "manual"; break;
                default: temp = "undefined("+penalty+")";
            }
        } else {
            switch (penalty) {
                case PENALTY_NONE:
                case PENALTY_HL_BALL_MANIPULATION:      temp = "none"; break;
                case PENALTY_HL_PHYSICAL_CONTACT:       temp = "pushing"; break;
                case PENALTY_HL_ILLEGAL_ATTACK:         temp = "illegal attack"; break;
                case PENALTY_HL_ILLEGAL_DEFENSE:        temp = "illegal defender"; break;
                case PENALTY_HL_PICKUP_OR_INCAPABLE:    temp = "pickup/incapable"; break;
                case PENALTY_HL_SERVICE:                temp = "service"; break;
                case PENALTY_MANUAL:                    temp = "manual"; break;
                case PENALTY_SUBSTITUTE:               temp = "substitute"; break;
                default: temp = "undefined("+penalty+")";
            }
        }
        out += "            penalty: "+temp+"\n";
        out += "secsTillUnpenalised: "+secsTillUnpenalised+"\n";
        return out;
    }
}