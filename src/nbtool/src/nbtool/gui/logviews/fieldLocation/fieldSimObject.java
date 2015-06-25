package nbtool.gui.logviews.fieldLocation;

import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.geom.Ellipse2D;

public abstract class fieldSimObject {
	protected float x,y,radius;
	protected Color color;
	
	public fieldSimObject() {
		x = 0.0f;
		y = 0.0f;
		setRadiusAndColor();
	}

	public fieldSimObject(float xCoord, float yCoord) {
		x = xCoord;
		y = yCoord;
		setRadiusAndColor();
	}
	
	protected void setRadiusAndColor() {
		radius = 40.0f;
		color = Color.CYAN;
	}
	
	public void moveTo(float xCoord, float yCoord) {
		x = xCoord;
		y = yCoord;
	}
	
	public void draw(Graphics2D g2) {
		g2.setColor(color);
		g2.fill(new Ellipse2D.Float(FieldConstants.GREEN_PAD_X+x-20, FieldConstants.GREEN_PAD_Y+y-20, 40.0f, 40.0f));
	}
	
	public float getX() { return x; }
	public float getY() { return y; }
}
