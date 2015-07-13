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
public class TeamInfo implements Serializable
{
    private static final long serialVersionUID = 2795660408542807763L;

    /**
     * How many players a team may have.
     * Actually that many players in each team need to be sent, even if
     * playersPerTeam in GameControlData is less.
     */
    public static final byte MAX_NUM_PLAYERS = 11;
    
    /** The size in bytes this class has packed. */
    public static final int SIZE =
            1 + // teamNumber
            1 + // teamColor
            1 + // score
            1 + // penaltyShot
            2 + // singleShots
            1 + // coach's sequence number
            SPLCoachMessage.SPL_COACH_MESSAGE_SIZE + // coach's message
            (MAX_NUM_PLAYERS + 1) * PlayerInfo.SIZE; // +1 for the coach

    /** The size in bytes this class has packed for version 7. */
    public static final int SIZE7 =
            1 + // teamNumber
            1 + // teamColor
            1 + // goal color
            1 + // score
            (MAX_NUM_PLAYERS) * PlayerInfo.SIZE7;

    //this is streamed
    public byte teamNumber;                                         // unique team number
    public byte teamColor;                                          // colour of the team
    public byte score;                                              // team's score
    public byte penaltyShot = 0;                                    // penalty shot counter
    public short singleShots = 0;                                   // bits represent penalty shot success
    public byte coachSequence;                                      // sequence number of the last coach message
    public byte[] coachMessage = new byte[SPLCoachMessage.SPL_COACH_MESSAGE_SIZE];
    public PlayerInfo coach = new PlayerInfo();
    public PlayerInfo[] player = new PlayerInfo[MAX_NUM_PLAYERS];   // the team's players
    
    /**
     * Creates a new TeamInfo.
     */
    public TeamInfo()
    {
        for (int i=0; i<player.length; i++) {
            player[i] = new PlayerInfo();
        }
    }
    
    /**
     * Packing this Java class to the C-structure to be send.
     * @return Byte array representing the C-structure.
     */
    public byte[] toByteArray()
    {
        ByteBuffer buffer = ByteBuffer.allocate(SIZE);
        buffer.order(ByteOrder.LITTLE_ENDIAN);
        buffer.put(teamNumber);
        buffer.put(teamColor);
        buffer.put(score);
        buffer.put(penaltyShot);
        buffer.putShort(singleShots);
        buffer.put(coachSequence);
        buffer.put(coachMessage);
        buffer.put(coach.toByteArray());
        for (int i=0; i<MAX_NUM_PLAYERS; i++) {
            buffer.put(player[i].toByteArray());
        }

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
        buffer.put(teamNumber);
        buffer.put(teamColor);
        buffer.put((byte) 1); // goal color is always yellow
        buffer.put(score);
        for (int i=0; i<MAX_NUM_PLAYERS; i++) {
            buffer.put(player[i].toByteArray7());
        }

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
        teamNumber = buffer.get();
        teamColor = buffer.get();
        score = buffer.get();
        penaltyShot = buffer.get();
        singleShots = buffer.getShort();
        coachSequence = buffer.get();
        buffer.get(coachMessage);
        coach.fromByteArray(buffer);
        for (int i=0; i<player.length; i++) {
            player[i].fromByteArray(buffer);
        }
    }
    
    @Override
    public String toString()
    {
        String out = "--------------------------------------\n";
        String temp;
        
        out += "         teamNumber: "+teamNumber+"\n";
        switch (teamColor) {
            case GameControlData.TEAM_BLUE: temp = "blue"; break;
            case GameControlData.TEAM_RED:  temp = "red";  break;
            case GameControlData.TEAM_YELLOW: temp = "yellow"; break;
            case GameControlData.TEAM_BLACK:  temp = "black";  break;
            default: temp = "undefinied("+teamColor+")";
        }
        out += "          teamColor: "+temp+"\n";
        out += "              score: "+score+"\n";
        out += "        penaltyShot: "+penaltyShot+"\n";
        out += "        singleShots: "+Integer.toBinaryString(singleShots)+"\n";
        out += "      coachSequence: "+coachSequence+"\n";
        out += "       coachMessage: "+new String(coachMessage)+"\n";
        out += "        coachStatus: "+coach.toString()+"\n";
        return out;
    }
}