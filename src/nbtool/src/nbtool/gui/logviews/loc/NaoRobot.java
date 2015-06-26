package nbtool.gui.logviews.loc;

import java.awt.*;
import java.awt.geom.*;
import java.awt.event.*;
import javax.swing.*;

public class NaoRobot {
	private final static float naoRobotSize = 15.f;
	double halfViewAngle = Math.toRadians(31);
	protected int xDist, yDist;
	public float x,y,h, radius;
	protected Color naoColor;

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
		if(shouldFlip) {
			x = FieldConstants.FIELD_WIDTH-x;
			h += (float)Math.PI;
		} else {
			y = FieldConstants.FIELD_HEIGHT-y;
		}

		g2.setColor(naoColor);
		g2.fill(new Ellipse2D.Float(x-7.5f, y-7.5f, 15.f, 15.f));

		//heading
		g2.setColor(Color.black);
		g2.setStroke(new BasicStroke(3));
		xDist = (int)(20 * Math.cos(h)+x);
		yDist = (int)(20 * Math.sin(h)+y);
		g2.drawLine((int)x,(int)(y),xDist,yDist);

		//field of view
		g2.setColor(Color.red);
		g2.setStroke(new BasicStroke(2));
		//right
		xDist = (int)(20 * Math.cos(h+halfViewAngle)+x);
		yDist = (int)(20 * Math.sin(h+halfViewAngle)+y);
		g2.drawLine((int)x,(int)y,xDist,yDist);
		//left
		xDist = (int)(20 * Math.cos(h-halfViewAngle)+x);
		yDist = (int)(20 * Math.sin(h-halfViewAngle)+y);
		g2.drawLine((int)x,(int)y,xDist,yDist);
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