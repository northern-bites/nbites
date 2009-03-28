package TOOL.WorldController;

public class Robot {

    final static int NUM_MSG_COMPONENTS = 17;
    final static String HEADER = "borat";

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
	String data = (new String(rawData,0,200));
	String[] words = data.split(" ");
	//System.out.println("parseData: " + data + " length: " + length);
	if (words.length < NUM_MSG_COMPONENTS) {
	    System.out.println("UDP PACKET REJECTED: packet: " + data + " length: " + words.length + " < NUM_MSG_COMPONENTS: " + NUM_MSG_COMPONENTS);
	    return null;
	}

	if (words[PACKET_TEAM_HEADER].equals(HEADER)) {
	    //We don't need to worry about it while its only us in the lab
	    //return null;
	    //System.out.println("UDP PACKET ACCEPTED: " + data);

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
}
