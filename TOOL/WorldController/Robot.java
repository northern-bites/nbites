package TOOL.WorldController;

import java.io.ByteArrayInputStream;
import java.io.DataInputStream;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.util.Vector;

public class Robot {

    final static int NUM_MSG_COMPONENTS = 17;
    final static String HEADER = "borat_foobar";
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

    private Integer team;
    private Integer color;
    private Integer number;
    private RobotData data;

    public Robot(int team, int color, int number) {
	this(team,color,number,new RobotData());
    }

    public Robot(int team, int color, int number, RobotData data) {
	this.team = new Integer(team);
	this.color = new Integer(color);
	this.number = new Integer(number);
	this.data = data;
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
	    System.out.println("UDP PACKET REJECTED: length: " + length +
                               " < HEADER_DATA_SIZE: " + HEADER_DATA_SIZE);
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
                System.out.println("UDP PACKET REJECTED: header = " + header);
                return null;
            }
            // parse header data
            long timeStamp = RobotData.swapb_long(input.readLong());
            input.readInt();
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
                             new RobotData(timeStamp, values));

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

    // XXX - these need to be changed to Float objects
    private Integer time;
    private Integer robotX;
    private Integer robotY;
    private Integer robotHeading;
    private Integer robotUncertX, robotUncertY, robotUncertH;
    private Integer ballX;
    private Integer ballY;
    private Integer ballXUncert;
    private Integer ballYUncert;
    private Integer ballXVel;
    private Integer ballYVel;
    private Integer ballDist;
    private boolean ballTrapped;
    private Integer calledSubRole;
    private Integer chaseTime;

    public RobotData() {
	time = new Integer(0);
	ballX = new Integer(0);
	ballY = new Integer(0);
	ballDist = new Integer(0);
	ballTrapped = false;
	ballXVel = new Integer(0);
	ballYVel = new Integer(0);
	robotX = new Integer(0);
	robotY = new Integer(0);
	robotHeading = new Integer(0);
	robotUncertX = new Integer(0);
	robotUncertY = new Integer(0);
	robotUncertH = new Integer(0);
	calledSubRole = new Integer(0);
    }

    public RobotData(Integer time, Integer robotX, Integer robotY, Integer robotHeading,
		   Integer robotUncertX, Integer robotUncertY, Integer robotUncertH,
		   Integer ballX, Integer ballY,
		   Integer ballXUncert, Integer ballYUncert,
		   Integer ballXVel, Integer ballYVel,
		   Integer ballDist, Integer ballTrapped,
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
	this.ballTrapped = (ballTrapped.intValue()==1);
	this.calledSubRole = calledSubRole;
    }

    public RobotData(long timeStamp, Vector<Float> values) {
	time = (int)timeStamp;
        // see order in man/noggin/Brain.py
	robotX        = values.get(0).intValue();
	robotY        = values.get(1).intValue();
	robotHeading  = values.get(2).intValue();
	robotUncertX  = values.get(3).intValue();
	robotUncertY  = values.get(4).intValue();
	robotUncertH  = values.get(5).intValue();
	ballX         = values.get(6).intValue();
	ballY         = values.get(7).intValue();
        ballXUncert   = values.get(8).intValue();
        ballYUncert   = values.get(9).intValue();
        ballDist      = values.get(10).intValue();
	calledSubRole = values.get(11).intValue();
        chaseTime     = values.get(12).intValue();
        // XXX - not currently sending these!
	ballXVel = new Integer(0);
	ballYVel = new Integer(0);
	ballTrapped = false;
    }

    //public Integer getIDNumber() {
    //return number;
    //}
    public Integer getTime() {
	return time;
    }

    public Integer getRobotX() {
	return robotX;
    }

    public Integer getRobotY() {
	return robotY;
    }

    public Integer getRobotHeading() {
	return robotHeading;
    }

    public Integer getRobotUncertX() {
	return robotUncertX;
    }

    public Integer getRobotUncertY() {
	return robotUncertY;
    }

    public Integer getRobotUncertH() {
	return robotUncertH;
    }

    public Integer getBallX() {
	return ballX;
    }

    public Integer getBallY() {
	return ballY;
    }

    public Integer getBallXUncert() {
	return ballXUncert;
    }

    public Integer getBallYUncert() {
	return ballYUncert;
    }

    public Integer getBallXVel() {
	return ballXVel;
    }

    public Integer getBallYVel() {
	return ballYVel;
    }

    public Integer getBallDistance() {
	return ballDist;
    }

    public double getRobotHeadingRadians() {
	return robotHeading.intValue()/180.0 * Math.PI;
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
