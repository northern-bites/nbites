package nbtool.gui.logviews.loc;

import java.awt.*;
import java.awt.geom.*;
import java.awt.event.*;
import javax.swing.*;

public class NaoParticle {
	private final static float particleSize = 2.f;
	protected Color naoColor;
	protected float x,y,radius;

	public NaoParticle() { 
		x = 0.0f;
		y = 0.0f;
		setRadiusAndColor(); 
	}

	NaoParticle(float xCoord, float yCoord) {
		x = xCoord;
		y = yCoord;
		setRadiusAndColor();
	}

	public void setRadiusAndColor() {
		radius = particleSize;
		naoColor = Color.red;
	}

	public void drawParticle(Graphics2D g2, float weight, boolean shouldFlip) {
		if(shouldFlip) {
			x = FieldConstants.FIELD_WIDTH-x;
		} else {
			y = FieldConstants.FIELD_HEIGHT-y;
		}
		if(weight > 0.002f) {
			g2.setColor(Color.magenta);
		} else if(weight > 0.004f) {
			g2.setColor(Color.orange);	
		} else {
			g2.setColor(Color.white);
		}
		g2.fill(new Ellipse2D.Float(x-1, y-1, 2.f, 2.f));
	}

	public void moveTo(float xCoord, float yCoord) {
		x = xCoord;
		y = yCoord;
	}

	public float getX() { return x; }
	public float getY() { return y; }
}