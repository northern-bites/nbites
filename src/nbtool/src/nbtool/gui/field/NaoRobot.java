package nbtool.gui.field;

import java.awt.*;
import java.awt.geom.*;
import java.lang.Math;

import nbtool.data.NBitesTeamBroadcast;

public class NaoRobot {
	private final static float naoRobotSize = 15.f;
	private final static int playerNumberOffset = 10;
	double halfViewAngle = Math.toRadians(31);
	
	public int fieldWalkingToX, fieldWalkingToY, playerNumber, fieldBallX, fieldBallY, 
				fieldSharedBallX, fieldSharedBallY;
	public float fieldX,fieldY,fieldH,fieldXDist,fieldYDist,fieldKickDestX, fieldKickDestY, radius;
	
	int screenX, screenY, screenKickDestX, screenKickDestY,screenWalkingToX, screenWalkingToY, 
		screenBallX, screenBallY,screenSharedBallX, screenSharedBallY, ballDist;
	float screenXDist, screenYDist, screenH, ballBearing;
	
	public boolean in_kicking_state = false; 
	public boolean seeing_ball = false;
	public boolean fallen = false;
	public boolean sharedBall = false;
	protected Color naoColor;
	float TO_RAD = (float) (Math.PI/180);

	public NaoRobot() { 
		screenX = 0;
		screenY = 0;
		setRadiusAndColor(); 
	}

	NaoRobot(float xCoord, float yCoord) {
		screenX = (int) xCoord;
		screenY = (int) yCoord;
		screenH = 0.0f;
		setRadiusAndColor();
	}

	NaoRobot(float xCoord, float yCoord, float heading) {
		screenX = (int) xCoord;
		screenY = (int) yCoord;
		screenH = -heading;
		setRadiusAndColor();
	}

	public void setRadiusAndColor() {
		radius = naoRobotSize;
		naoColor = Color.cyan;
	}
	
	public void drawNaoLoc(Graphics2D g2, boolean shouldFlip) {
		if(shouldFlip) {
			screenX = (int) (FieldConstants.FIELD_WIDTH-screenX);
			screenH = (float) (Math.PI+screenH);
		} else {
			screenY = (int) (FieldConstants.FIELD_HEIGHT-screenY);
		}
		
		g2.setColor(naoColor);
		g2.fill(new Ellipse2D.Float(screenX-naoRobotSize/2, screenY-naoRobotSize/2, naoRobotSize, naoRobotSize));

		//heading
		g2.setColor(Color.black);
		g2.setStroke(new BasicStroke(3));
		int xDist = (int)(20 * Math.cos(screenH)+screenX);
		int yDist = (int)(20 * Math.sin(screenH)+screenY);
		g2.drawLine((int)screenX,(int)(screenY),xDist,yDist);

		//field of view
		g2.setColor(Color.red);
		g2.setStroke(new BasicStroke(2));
		//right
		xDist = (int)(20 * Math.cos(screenH+halfViewAngle)+screenX);
		yDist = (int)(20 * Math.sin(screenH+halfViewAngle)+screenY);
		g2.drawLine((int)screenX,(int)screenY,xDist,yDist);
		//left
		xDist = (int)(20 * Math.cos(screenH-halfViewAngle)+screenX);
		yDist = (int)(20 * Math.sin(screenH-halfViewAngle)+screenY);
		g2.drawLine((int)screenX,(int)screenY,xDist,yDist);
	}

	public void drawNaoWV(Graphics2D g2, boolean shouldFlip) {
			if(shouldFlip) {
				screenX = (int) (FieldConstants.FIELD_WIDTH-fieldX);
				screenY = (int) fieldY;
				screenH = (float) (180+fieldH);
				screenKickDestX = (int) (FieldConstants.FIELD_WIDTH-fieldKickDestX);
				screenKickDestY  = (int) fieldKickDestY;
				screenWalkingToX = (int) (FieldConstants.FIELD_WIDTH-fieldWalkingToX);
				screenWalkingToY = (int) fieldWalkingToY;
			} else {
				screenX = (int) fieldX;
				screenY = (int) (FieldConstants.FIELD_HEIGHT- fieldY);
				screenH = fieldH;
				screenKickDestX  = (int) fieldKickDestX;
				screenKickDestY = (int) (FieldConstants.FIELD_HEIGHT-fieldKickDestY);
				screenWalkingToX = (int) fieldWalkingToX;
				screenWalkingToY = (int) (FieldConstants.FIELD_HEIGHT-fieldWalkingToY);
			}

		g2.setColor(naoColor);
		g2.fill(new Ellipse2D.Float(screenX-naoRobotSize/2, screenY-naoRobotSize/2, naoRobotSize, naoRobotSize));

		//heading
		g2.setColor(Color.black);
		g2.setStroke(new BasicStroke(3));
		screenXDist = (float)(20 * Math.cos(TO_RAD*screenH)+screenX);
		screenYDist = (float)(20 * Math.sin(TO_RAD*screenH)+screenY);
		g2.draw(new Line2D.Float(screenX, screenY, screenXDist, screenYDist));

		//field of view
		g2.setColor(Color.red);
		g2.setStroke(new BasicStroke(2));
		//right
		screenXDist = (float)(20 * Math.cos(TO_RAD*screenH+halfViewAngle)+screenX);
		screenYDist = (float)(20 * Math.sin(TO_RAD*screenH+halfViewAngle)+screenY);
		g2.draw(new Line2D.Float(screenX, screenY, screenXDist, screenYDist));
		
		//left
		screenXDist = (float)(20 * Math.cos(TO_RAD*screenH-halfViewAngle)+screenX);
		screenYDist = (float)(20 * Math.sin(TO_RAD*screenH-halfViewAngle)+screenY);
		g2.draw(new Line2D.Float(screenX, screenY, screenXDist, screenYDist));
		
		//player number
		g2.setColor(naoColor);
		g2.drawString(Integer.toString(playerNumber), screenX+playerNumberOffset, screenY-playerNumberOffset);
		
		//where am i kicking
		g2.setColor(Color.blue);
		if(!in_kicking_state) {
			screenKickDestX = (int)screenX;
			screenKickDestY = (int)screenY;
		}
		g2.draw(new Line2D.Float(screenX,screenY,screenKickDestX,screenKickDestY));
		
		//where am i walking
		g2.setColor(Color.red);
		if(!fallen) {
			screenWalkingToX = (int)screenX;
			screenWalkingToY = (int)screenY;
		}
		g2.drawLine((int)screenX,(int)screenY,screenWalkingToX,screenWalkingToY);
		
		//where is my ball
		g2.setColor(Color.orange);
		if(seeing_ball) {
			Point ballCenter = new Point((int)(screenX+ballDist*Math.cos(TO_RAD*screenH+TO_RAD*ballBearing)),
									  (int)(screenY-ballDist*Math.sin(TO_RAD*screenH+TO_RAD*ballBearing)));
			g2.fill(new Ellipse2D.Double(ballCenter.x, ballCenter.y, 8, 8));
			g2.drawString(Integer.toString(playerNumber), ballCenter.x+1, ballCenter.y-1);	
		}
		
		//where is my shared ball | only for player 4&5: chasers
		g2.setColor(new Color(204,0,100));
		if(sharedBall) {
			g2.fill(new Ellipse2D.Double(screenSharedBallX-4, screenSharedBallY-4,8,8));
			g2.drawString(Integer.toString(playerNumber), screenSharedBallX+1, screenSharedBallX-1);
		}
		
	}
	
	public void wvNao(NBitesTeamBroadcast tb) {
		fieldX = tb.dataWorldModel.getMyX();
		fieldY = tb.dataWorldModel.getMyY();
		fieldH = -tb.dataWorldModel.getMyH();
		playerNumber = tb.dataTeamPacket.getPlayerNumber();
		
		if(tb.dataWorldModel.getFallen()) {
			naoColor = Color.red;
		} else {
			naoColor = Color.cyan;
		}
		
		if(tb.dataWorldModel.getInKickingState()) {
			in_kicking_state = true;
			fieldKickDestX = (int)tb.dataWorldModel.getKickingToX();
			fieldKickDestY = (int)tb.dataWorldModel.getKickingToY();
		}
		
		if(!tb.dataWorldModel.getFallen()) {
			fallen = true;
			fieldWalkingToX = (int)tb.dataWorldModel.getWalkingToX();
			fieldWalkingToY = (int)tb.dataWorldModel.getWalkingToY();
		}
		
		if(tb.dataWorldModel.getBallOn()) {
			seeing_ball = true;
			ballDist = (int) tb.dataWorldModel.getBallDist();
			ballBearing = (int) tb.dataWorldModel.getBallBearing();
		}
	}
	
	public void wvSharedBall(NBitesTeamBroadcast tb) {
		sharedBall = true;
		float sinHB, cosHB;
		
		float hb = TO_RAD*fieldH+TO_RAD*tb.dataWorldModel.getBallBearing();
		sinHB = (float) Math.sin(hb);
		cosHB = (float) Math.cos(hb);
			
		float newBallX = fieldX + tb.dataWorldModel.getBallDist()*cosHB;
		float newBallY = fieldY + tb.dataWorldModel.getBallDist()*sinHB;
			
		fieldBallX = (int) newBallX;
		fieldBallY = (int) newBallY;

	}

	public void moveTo(float xCoord, float yCoord, float heading) {
		screenX = (int)xCoord;
		screenY = (int)yCoord;
		screenH = -heading;
	}

	public float getX() { return fieldX; }
	public float getY() { return fieldY; }
	public float getH() { return fieldH; }
}