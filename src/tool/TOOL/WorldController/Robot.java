package TOOL.WorldController;

import java.io.ByteArrayInputStream;
import java.io.DataInputStream;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.util.Vector;

public class Robot {

    final static int NUM_MSG_COMPONENTS = 17;
    final static String HEADER = "ilikeyoulots";//"ilikeyoulots";
    final static int HEADER_DATA_SIZE = HEADER.length() + 1 + 5 * 8;

    final static int PACKET_TEAM_HEADER = 0;
    final static int PACKET_TEAM_NUMBER = 1;
    final static int PACKET_TEAM_COLOR = 2;
    final static int PACKET_PLAYER_NUMBER = 3;
    final static int PACKET_TIME_STAMP = 4;
    final static int PACKET_PLAYER_X = 5;
    final static int PACKET_PLAYER_Y = 6;
    final static int PACKET_PLAYER_H = 7;
    final static int PACKET_UNCERT_X = 8;
    final static int PACKET_UNCERT_Y = 9;
    final static int PACKET_UNCERT_H = 10;
    final static int PACKET_BALL_X = 11;
    final static int PACKET_BALL_Y = 12;
    final static int PACKET_BALL_UNCERT_X = 13;
    final static int PACKET_BALL_UNCERT_Y = 14;
    final static int PACKET_BALL_DIST = 15;
    final static int PACKET_ROLE = 16;
    final static int PACKET_SUB_ROLE = 17;
    final static int PACKET_BALL_VEL_X = 18;
    final static int PACKET_BALL_VEL_Y = 19;

    private Integer team;
    private Integer color;
    private Integer number;
    private RobotData data;
	private boolean isJustModel;

    public Robot(int team, int color, int number, boolean isModel) {
        this(team,color,number,new RobotData(), isModel);
    }

    public Robot(int team, int color, int number,
				 RobotData data, boolean isModel) {
        this.team = new Integer(team);
        this.color = new Integer(color);
        this.number = new Integer(number);
        this.data = data;
		this.isJustModel = isModel;
    }

	public void updateData(LocalizationPacket robotLoc,
						   LocalizationPacket ballLoc,
						   boolean isJustModel) {
		RobotData newData = new RobotData(0,
										  (float)robotLoc.getXEst(),
										  (float)robotLoc.getYEst(),
										  (float)robotLoc.getHeadingEst(),
										  (float)robotLoc.getXUncert(),
										  (float)robotLoc.getYUncert(),
										  (float)robotLoc.getHUncert(),
										  (float)ballLoc.getXEst(),
										  (float)ballLoc.getYEst(),
										  (float)ballLoc.getXUncert(),
										  (float)ballLoc.getYUncert(),
										  (float)ballLoc.getXVelocity(),
										  (float)ballLoc.getYVelocity(),
										  0.0f, false, 0);
		this.data = newData;
		this.isJustModel = isJustModel;
	}

    public Integer getTeam() {
        return team;
    }

    public Integer getColor() {
        return color;
    }

    public Integer getNumber() {
        return number;
    }

    public Integer getHash() {
        return (team * 10) + number;
    }

    public RobotData getData() {
        return data;
    }

    public static Robot parseData(byte[] rawData, int length) {
        // ensure packet length
        if (length < HEADER_DATA_SIZE) {
            // System.out.println("UDP PACKET REJECTED: length: " + length +
            //                    " < HEADER_DATA_SIZE: " + HEADER_DATA_SIZE);
            return null;
        }
        // swap endianness of all float values
        //RobotData.swapb_array(rawData, 32, (length - 32) / 4 * 4);
        // open data input stream to read raw values
        DataInputStream input = new DataInputStream(
                                                    new ByteArrayInputStream(rawData, 0, length));
        // begin parsing
        try {
            // parse header string
            byte[] rawHeader = new byte[HEADER.length()];
            input.readFully(rawHeader);
            String header = new String(rawHeader, "ASCII");
            if (!header.equals(HEADER)) {
                //System.out.println("UDP PACKET REJECTED: header = " + header);
                return null;
            }
            // parse header data
            long timeStamp = RobotData.swapb_long(input.readLong());
            input.readInt();    // Just read another int. Why? Good question...
            int packetNum = RobotData.swapb_int(input.readInt());
            int team = RobotData.swapb_int(input.readInt());
            int player = RobotData.swapb_int(input.readInt());
            int color = RobotData.swapb_int(input.readInt());
            // parse float values
            Vector<Float> values = new Vector<Float>();
            while (input.available() > 0)
                values.add(Float.intBitsToFloat(
                                                RobotData.swapb_int(input.readInt())
                                                ));

            return new Robot(team, color, player,
                             new RobotData(timeStamp, values), false);

        }catch(UnsupportedEncodingException e) {
            System.out.println("ASCII Encoding not supported!!!");
            return null;
        }catch(IOException e) {
            System.out.println("UDP PACKET REJECTED: " + e);
            return null;
        }

    }
}

class RobotData {

    private Integer time;
    private Float robotX;
    private Float robotY;
    private Float robotHeading;
    private Float robotUncertX, robotUncertY, robotUncertH;
    private Float ballX;
    private Float ballY;
    private Float ballXUncert;
    private Float ballYUncert;
    private Float ballDist;
    private boolean ballTrapped;
    private Integer calledRole;
    private Integer calledSubRole;
    private Float chaseTime;
    private Float ballXVel;
    private Float ballYVel;

    public RobotData() {
        time = new Integer(0);
        ballX = new Float(0);
        ballY = new Float(0);
        ballDist = new Float(0);
        ballTrapped = false;
        ballXVel = new Float(0);
        ballYVel = new Float(0);
        robotX = new Float(0);
        robotY = new Float(0);
        robotHeading = new Float(0);
        robotUncertX = new Float(0);
        robotUncertY = new Float(0);
        robotUncertH = new Float(0);
        calledSubRole = new Integer(0);
        calledRole = new Integer(0);
        ballXVel = new Float(0);
        ballYVel = new Float(0);
    }

	// EXPECTS HEADING AS RADIANS
    public RobotData(Integer time, Float robotX, Float robotY, Float robotHeading,
                     Float robotUncertX, Float robotUncertY, Float robotUncertH,
                     Float ballX, Float ballY,
                     Float ballXUncert, Float ballYUncert,
                     Float ballXVel, Float ballYVel,
                     Float ballDist, boolean ballTrapped,
                     Integer calledSubRole) {
        this.time = time;
        this.robotX = robotX;
        this.robotY = robotY;
        this.robotHeading = robotHeading;
        this.robotUncertX = robotUncertX;
        this.robotUncertY = robotUncertY;
        this.robotUncertH = robotUncertH;
        this.ballX = ballX;
        this.ballY = ballY;
        this.ballXUncert = ballXUncert;
        this.ballYUncert = ballYUncert;
        this.ballXVel = ballYVel;
        this.ballYVel = ballYVel;
        this.ballDist = ballDist;
        this.ballTrapped = ballTrapped;
        this.calledSubRole = calledSubRole;
    }

	// EXPECTS HEADING VALUES AS DEGREES! Uses Python (UDP) data, with
	// degree heading values. Really ugly, I'm aware.
    public RobotData(long timeStamp, Vector<Float> values) {
        time = (int)timeStamp;
        // see order in man/noggin/Brain.py
        robotX        = values.get(0);
        robotY        = values.get(1);
        robotHeading  = values.get(2)/180.0f * (float)Math.PI;
        robotUncertX  = values.get(3);
        robotUncertY  = values.get(4);
        robotUncertH  = values.get(5);
        ballX         = values.get(6);
        ballY         = values.get(7);
        ballXUncert   = values.get(8);
        ballYUncert   = values.get(9);
        ballDist      = values.get(10);
        calledRole    = values.get(11).intValue();
        calledSubRole = values.get(12).intValue();
        chaseTime     = values.get(13);
        try {
            ballXVel      = values.get(14);
            ballYVel      = values.get(15);
        } catch (ArrayIndexOutOfBoundsException e) {
            System.out.println("\n\nYou are using an old version of man. "+
                               " Please update. :)");
            ballXVel      = new Float(0);
            ballYVel      = new Float(0);
        }
        ballTrapped   = false;
    }

    //public Integer getIDNumber() {
    //return number;
    //}
    public Integer getTime() {
        return time;
    }

    public Float getRobotX() {
        return robotX;
    }

    public Float getRobotY() {
        return robotY;
    }

	// Should be in radians, not degrees
    public Float getRobotHeading() {
        return robotHeading;
    }

    public Float getRobotUncertX() {
        return robotUncertX;
    }

    public Float getRobotUncertY() {
        return robotUncertY;
    }

    public Float getRobotUncertH() {
        return robotUncertH;
    }

    public Float getBallX() {
        return ballX;
    }

    public Float getBallY() {
        return ballY;
    }

    public Float getBallXUncert() {
        return ballXUncert;
    }

    public Float getBallYUncert() {
        return ballYUncert;
    }

    public Float getBallXVel() {
        return ballXVel;
    }

    public Float getBallYVel() {
        return ballYVel;
    }

    public Float getBallDistance() {
        return ballDist;
    }

    public Integer getCalledSubRole() {
        return calledSubRole;
    }

    public static int swapb_int(int v) {
        return (v >>> 24) | (v << 24) |
            ((v << 8) & 0x00FF0000) | ((v >> 8) & 0x0000FF00);
    }

    public static long swapb_long(long x) {
        return (x>>56) |
            ((x<<40) & 0x00FF000000000000L) |
            ((x<<24) & 0x0000FF0000000000L) |
            ((x<<8)  & 0x000000FF00000000L) |
            ((x>>8)  & 0x00000000FF000000L) |
            ((x>>24) & 0x0000000000FF0000L) |
            ((x>>40) & 0x000000000000FF00L) |
            (x<<56);
    }

    public static void swapb_array(byte[] bytes, int offset, int length) {
        assert length >= 4;
        byte tmp;
        for (int i = offset; i < offset + length; i+=4) {
            tmp = bytes[i];
            bytes[i    ] = bytes[i + 3];
            bytes[i + 3] = tmp;
            tmp = bytes[i + 1];
            bytes[i + 1] = bytes[i + 2];
            bytes[i + 2] = tmp;
        }
    }

}
