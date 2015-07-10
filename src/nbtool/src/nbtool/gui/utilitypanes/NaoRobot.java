package nbtool.gui.utilitypanes;

import java.awt.*;
import java.awt.geom.*;
import java.awt.geom.Line2D.Float;
import java.awt.event.*;
import javax.swing.*;
import java.lang.Math;

import nbtool.data.TeamBroadcast;

public class NaoRobot {
	private final static float naoRobotSize = 15.f;
	private final static int playerNumberOffset = 10;
	double halfViewAngle = Math.toRadians(31);
	protected int walkingToX, walkingToY, playerNumber, ballX, ballY;
	public float x,y,h, radius, xDist, yDist,kickDestX, kickDestY;
	public boolean shouldFlip_; 
	public boolean in_kicking_state = false; 
	public boolean seeing_ball = false;
	public boolean fallen = false;
	protected Color naoColor;
	float TO_RAD = (float) (Math.PI/180);

	public NaoRobot() { 
		x = 0.0f;
		y = 0.0f;
		setRadiusAndColor(); 
	}

	NaoRobot(float xCoord, float yCoord) {
		x = xCoord;
		y = yCoord;
		h = 0.0f;
		setRadiusAndColor();
	}

	NaoRobot(float xCoord, float yCoord, float heading) {
		x = xCoord;
		y = yCoord;
		h = -heading;
		setRadiusAndColor();
	}

	public void setRadiusAndColor() {
		radius = naoRobotSize;
		naoColor = Color.cyan;
	}

	public void drawNao(Graphics2D g2, boolean shouldFlip) {
		shouldFlip_ = shouldFlip;
		if(shouldFlip_) {
			x = FieldConstants.FIELD_WIDTH-x;
			h += (float)Math.PI;
			kickDestX = (int) (FieldConstants.FIELD_WIDTH-kickDestX);
			walkingToX = (int) (FieldConstants.FIELD_WIDTH-walkingToX);
			ballX = (int) (FieldConstants.FIELD_WIDTH-ballX);
		} else {
			y = FieldConstants.FIELD_HEIGHT-y;
			kickDestY = (int) FieldConstants.FIELD_HEIGHT-kickDestY;
			walkingToY = (int) FieldConstants.FIELD_HEIGHT-walkingToY;
			ballY = (int) FieldConstants.FIELD_HEIGHT-ballY;
		}

		g2.setColor(naoColor);
		g2.fill(new Ellipse2D.Float(x-7.5f, y-7.5f, 15.f, 15.f));

		//heading
		g2.setColor(Color.black);
		g2.setStroke(new BasicStroke(3));
		xDist = (float)(20 * Math.cos(TO_RAD*h)+x);
		yDist = (float)(20 * Math.sin(TO_RAD*h)+y);
		g2.draw(new Line2D.Float(x, y, xDist, yDist));

		//field of view
		g2.setColor(Color.red);
		g2.setStroke(new BasicStroke(2));
		//right
		xDist = (float)(20 * Math.cos(TO_RAD*h+halfViewAngle)+x);
		yDist = (float)(20 * Math.sin(TO_RAD*h+halfViewAngle)+y);
		g2.draw(new Line2D.Float(x, y, xDist, yDist));
		
		//left
		xDist = (float)(20 * Math.cos(TO_RAD*h-halfViewAngle)+x);
		yDist = (float)(20 * Math.sin(TO_RAD*h-halfViewAngle)+y);
		g2.draw(new Line2D.Float(x, y, xDist, yDist));
		
		//player number
		g2.setColor(naoColor);
		g2.drawString(Integer.toString(playerNumber), x+playerNumberOffset, y-playerNumberOffset);
		
		//where am i kicking
		g2.setColor(Color.blue);
		if(!in_kicking_state) {
			kickDestX = (int)x;
			kickDestY = (int)y;
		}
		g2.draw(new Line2D.Float(x,y,kickDestX,kickDestY));
		
		//where am i walking
		g2.setColor(Color.red);
		if(!fallen) {
			walkingToX = (int)x;
			walkingToY = (int)y;
		}
		g2.drawLine((int)x,(int)y,walkingToX,walkingToY);
		
		//where is my ball
		g2.setColor(Color.orange);
		if(seeing_ball) {
			g2.fill(new Ellipse2D.Double(ballX-4, ballY-4, 8, 8));
			g2.drawString(Integer.toString(playerNumber), ballX+1, ballY-1);	
		}
		
	}
	
	public void wvNao(TeamBroadcast tb) {
		x = tb.dataWorldModel.getMyX();
		y = tb.dataWorldModel.getMyY();
		h = -tb.dataWorldModel.getMyH();
		playerNumber = tb.dataTeamPacket.getPlayerNumber();
		
		if(tb.dataWorldModel.getFallen()) {
			naoColor = Color.red;
		} else {
			naoColor = Color.cyan;
		}
		
		if(tb.dataWorldModel.getInKickingState()) {
			in_kicking_state = true;
			kickDestX = (int)tb.dataWorldModel.getKickingToX();
			kickDestY = (int)tb.dataWorldModel.getKickingToY();
		}
		
		if(!tb.dataWorldModel.getFallen()) {
			fallen = true;
			walkingToX = (int)tb.dataWorldModel.getWalkingToX();
			walkingToY = (int)tb.dataWorldModel.getWalkingToY();
		}
		
		if(tb.dataWorldModel.getBallOn()) {
			seeing_ball = true;
			Point ballCenter = new Point((int)(tb.dataWorldModel.getMyX()+tb.dataWorldModel.getBallDist()
					*Math.cos(TO_RAD*tb.dataWorldModel.getMyH()+TO_RAD*tb.dataWorldModel.getBallBearing())),
					(int)(tb.dataWorldModel.getMyY()+tb.dataWorldModel.getBallDist()
					*Math.sin(TO_RAD*tb.dataWorldModel.getMyH()+tb.dataWorldModel.getBallBearing())));
			
			ballX = ballCenter.x;
			ballY = ballCenter.y;

		}
	}

	public void moveTo(float xCoord, float yCoord, float heading) {
		x = xCoord;
		y = yCoord;
		h = -heading;
	}

	public float getX() { return x; }
	public float getY() { return y; }
	public float getH() { return h; }
}