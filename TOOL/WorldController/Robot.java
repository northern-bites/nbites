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
        DataInputStream input = new DataInputStream(
                new ByteArrayInputStream(rawData));

        if (length < HEADER_DATA_SIZE) {
	    System.out.println("UDP PACKET REJECTED: length: " + length +
                               " < HEADER_DATA_SIZE: " + HEADER_DATA_SIZE);
            return null;
        }

        byte[] rawHeader = new byte[HEADER.length()];

        try {
            input.readFully(rawHeader);
            //input.readByte(); // skip NULL char

            String header = new String(rawHeader, "ASCII");
            if (!header.equals(HEADER)) {
                System.out.println("UDP PACKET REJECTED: header = " + header);
                return null;
            }

            long timeStamp = RobotData.swapb_long(input.readLong());
            int team = RobotData.swapb_int(input.readInt());
            int player = RobotData.swapb_int(input.readInt());
            int color = RobotData.swapb_int(input.readInt());
            System.out.println("packet = " + new String(rawData, "ASCII"));
            System.out.println("time = " + timeStamp + ", team = " + team +
                               ", player = " + player + ", color = " + color);

            Vector<Float> vals = new Vector<Float>();
            while (input.available() > 0)
                vals.add(input.readFloat());

        }catch(UnsupportedEncodingException e) {
            System.out.println("ASCII Encoding not supported!!!");
            return null;
        }catch(IOException e) {
            System.out.println("UDP PACKET REJECTED: " + e);
            return null;
        }

/*

	    try {
		// See TypeDefs.py Packet: for info about our UDP API
		Integer team = new Integer(words[PACKET_TEAM_NUMBER]);
		Integer color = new Integer(words[PACKET_TEAM_COLOR]);
		Integer number = new Integer(words[PACKET_PLAYER_NUMBER]);
		Integer time = new Integer(words[PACKET_TIME_STAMP]);
		Integer robotX =
		    new Integer((int)Double.parseDouble(words[PACKET_PLAYER_X]));
		Integer robotY =
		    new Integer((int)Double.parseDouble(words[PACKET_PLAYER_Y]));
		Integer robotHeading =
		    new Integer((int)Double.parseDouble(words[PACKET_PLAYER_H]));
		Integer robotUncertX =
		    new Integer((int)Double.parseDouble(words[PACKET_UNCERT_X]));
		Integer robotUncertY =
		    new Integer((int)Double.parseDouble(words[PACKET_UNCERT_Y]));
		Integer robotUncertH =
		    new Integer((int)Double.parseDouble(words[PACKET_UNCERT_H]));
		Integer ballX =
		    new Integer((int)Double.parseDouble(words[PACKET_BALL_X]));
		Integer ballY =
		    new Integer((int)Double.parseDouble(words[PACKET_BALL_Y]));
		Integer ballXUncert =
		    new Integer((int)Double.parseDouble(words[PACKET_BALL_UNCERT_X]));
		Integer ballYUncert =
		    new Integer((int)Double.parseDouble(words[PACKET_BALL_UNCERT_Y]));
		Integer ballXVel = 0;
		Integer ballYVel = 0;
		Integer ballDist = 0;
		Integer ballTrapped = 0;//new Integer(words[14]);
		Integer calledSubRole =
		    new Integer(Integer.parseInt(words[PACKET_ROLE]));

		return new Robot(team,color,number,new RobotData(time,robotX,robotY,robotHeading,
							     robotUncertX, robotUncertY,
							     robotUncertY,
							     ballX,ballY,
							     ballXUncert,ballYUncert,
							     ballXVel,ballYVel,
							     0,0,calledSubRole));
	    }catch (Exception e) {
		e.printStackTrace();
		return null;
	    }
	}
	else {
	    System.out.println("UDP PACKET REJECTED B/C OF HEADER: " + words[0]);
	}
        */
	return null;
    }
}

class RobotData {

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

    public RobotData() {
	time = new Integer(0);
	ballX = new Integer(0);
	ballY = new Integer(0);
	ballDist = new Integer(0);
	ballTrapped = false;
	robotX = new Integer(0);
	robotY = new Integer(0);
	robotHeading = new Integer(0);
	robotUncertX = new Integer(0);
	robotUncertY = new Integer(0);
	robotUncertH = new Integer(0);
	ballXVel = new Integer(0);
	ballYVel = new Integer(0);
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
      for (int i = offset; i < length; i+=4) {
        tmp = bytes[i];
        bytes[i    ] = bytes[i + 3];
        bytes[i + 3] = tmp;
        tmp = bytes[i + 1];
        bytes[i + 1] = bytes[i + 2];
        bytes[i + 2] = tmp;
      }
    }

}
