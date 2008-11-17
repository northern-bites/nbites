package edu.bowdoin.robocup.TOOL.WorldController;

public class Dog {

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
    private DogData data;
  
    public Dog(int team, int color, int number) {
	this(team,color,number,new DogData());
    }

    public Dog(int team, int color, int number, DogData data) {
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

    public DogData getData() {
	return data;
    }

    public static Dog parseData(byte[] rawData, int length) {
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
		Integer dogX = 
		    new Integer((int)Double.parseDouble(words[PACKET_PLAYER_X]));
		Integer dogY = 
		    new Integer((int)Double.parseDouble(words[PACKET_PLAYER_Y]));
		Integer dogHeading = 
		    new Integer((int)Double.parseDouble(words[PACKET_PLAYER_H]));
		Integer dogUncertX = 
		    new Integer((int)Double.parseDouble(words[PACKET_UNCERT_X]));
		Integer dogUncertY = 
		    new Integer((int)Double.parseDouble(words[PACKET_UNCERT_Y]));
		Integer dogUncertH = 
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

		return new Dog(team,color,number,new DogData(time,dogX,dogY,dogHeading,
							     dogUncertX, dogUncertY,
							     dogUncertY,
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

class DogData {
  
    private Integer time;
    private Integer dogX;
    private Integer dogY;
    private Integer dogHeading;
    private Integer dogUncertX, dogUncertY, dogUncertH;
    private Integer ballX;
    private Integer ballY;
    private Integer ballXUncert;
    private Integer ballYUncert;
    private Integer ballXVel;
    private Integer ballYVel;
    private Integer ballDist;
    private boolean ballTrapped;
    private Integer calledSubRole;
  
    public DogData() {
	time = new Integer(0);
	ballX = new Integer(0);
	ballY = new Integer(0);
	ballDist = new Integer(0);
	ballTrapped = false;
	dogX = new Integer(0);
	dogY = new Integer(0);
	dogHeading = new Integer(0);
	dogUncertX = new Integer(0);
	dogUncertY = new Integer(0);
	dogUncertH = new Integer(0);
	ballXVel = new Integer(0);
	ballYVel = new Integer(0);
	calledSubRole = new Integer(0);
    }
  
    public DogData(Integer time, Integer dogX, Integer dogY, Integer dogHeading, 
		   Integer dogUncertX, Integer dogUncertY, Integer dogUncertH,
		   Integer ballX, Integer ballY, 
		   Integer ballXUncert, Integer ballYUncert,
		   Integer ballXVel, Integer ballYVel, 
		   Integer ballDist, Integer ballTrapped,
		   Integer calledSubRole) {
	this.time = time;
	this.dogX = dogX;
	this.dogY = dogY;
	this.dogHeading = dogHeading;
	this.dogUncertX = dogUncertX;
	this.dogUncertY = dogUncertY;
	this.dogUncertH = dogUncertH;
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
  
    public Integer getDogX() {
	return dogX;
    }
  
    public Integer getDogY() {
	return dogY;
    }

    public Integer getDogHeading() {
	return dogHeading;
    }

    public Integer getDogUncertX() {
	return dogUncertX;
    }
  
    public Integer getDogUncertY() {
	return dogUncertY;
    }

    public Integer getDogUncertH() {
	return dogUncertH;
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

    public double getDogHeadingRadians() {
	return dogHeading.intValue()/180.0 * Math.PI;
    }

    public Integer getCalledSubRole() {
	return calledSubRole;
    }
}
