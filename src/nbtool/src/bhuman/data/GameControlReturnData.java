package data;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;


/**
 * @author Michel Bartsch
 * 
 * This class is what robots send to the GameCOntroller.
 * It just represents this data, reads and writes between C-structure and
 * Java, nothing more.
 */
public class GameControlReturnData
{
    /** The header to identify the structure. */
    public static final String GAMECONTROLLER_RETURN_STRUCT_HEADER = "RGrt";
    /** The version of the data structure. */
    public static final byte GAMECONTROLLER_RETURN_STRUCT_VERSION = 2;
    public static final byte GAMECONTROLLER_RETURN_STRUCT_VERSION1 = 1;

    /** What a player may say. */
    public static final byte GAMECONTROLLER_RETURN_MSG_MAN_PENALISE = 0;
    public static final byte GAMECONTROLLER_RETURN_MSG_MAN_UNPENALISE = 1;
    public static final byte GAMECONTROLLER_RETURN_MSG_ALIVE = 2;
    
    
    /** The size in bytes this class has packed. */
    public static final int SIZE =
            4 + // header
            1 + // version
            1 + // team
            1 + // player
            1; // message
    
    /** The size in bytes this class has packed for version 1 */
    public static final int SIZE1 =
            4 + // header
            4 + // version
            2 + // team
            2 + // player
            4; // message

    //this is streamed
    String header;          // header to identify the structure
    byte version;            // version of the data structure
    public byte team;      // unique team number
    public byte player;    // player number
    public byte message;     // what the player says
    
    
    /**
     * Packing this Java class to the C-structure to be send.
     * @return Byte array representing the C-structure.
     */
    public byte[] toByteArray()
    {
        ByteBuffer buffer = ByteBuffer.allocate(SIZE);
        buffer.order(ByteOrder.LITTLE_ENDIAN);

        buffer.put(header.getBytes());
        buffer.put(version);
        buffer.put(team);
        buffer.put(player);
        buffer.put(message);
        return buffer.array();
    }

    /**
     * Changes the state of this object to the state of the given byte-stream.
     *
     * @param buffer    the byte-stream to parse
     * @return          returns true if and only if the state of the object could be changed, false otherwise
     */
    public boolean fromByteArray(ByteBuffer buffer)
    {
        try {
            buffer.order(ByteOrder.LITTLE_ENDIAN);

            byte[] header = new byte[4];
            buffer.get(header, 0, 4);
            this.header = new String(header);
            if (!this.header.equals(GAMECONTROLLER_RETURN_STRUCT_HEADER)) {
            	System.out.println("\theader wrong" + this.header);
                return false;
            } else {
                version = buffer.get();
                switch (version) {
                case GAMECONTROLLER_RETURN_STRUCT_VERSION:
                	System.out.println("RETURN DATA USING NEWEST VERSION");
                    team = buffer.get();
                    player = buffer.get();
                    message = buffer.get();
                    
                    System.out.println("\tthis one was good");
                    return true;

                case GAMECONTROLLER_RETURN_STRUCT_VERSION1:
                	System.out.println("ERROR DATA USING OLD VERSION");

                    if (   Rules.league.compatibilityToVersion7
                        && buffer.get() == 0
                        && buffer.getShort() == 0)
                    {
                        team = (byte)buffer.getShort();
                        player = (byte)buffer.getShort();
                        message = (byte)buffer.getInt();
                        return true;
                    }
                    break;

                default:
                    break;
                }

                return false;
            }
        } catch (RuntimeException e) {
            return false;
        }
    }
}
