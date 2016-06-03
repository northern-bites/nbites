package nbtool.gui.logviews.fieldLocation;

import java.awt.*;

public class Player extends fieldSimObject {
	private final static float playerSize = 40.f;
    float playerX;
    float playerY;
    
    public Player() { super(); }
    
    Player(float xCoord, float yCoord) { 
    	super(xCoord,yCoord);
    }
    
    public void setRadiusAndColor()
    {
        radius = playerSize;
        color = Color.cyan;
    }
    
    public void moveTo(float xCoord, float yCoord) {
    	super.moveTo(xCoord, yCoord);
    }
}