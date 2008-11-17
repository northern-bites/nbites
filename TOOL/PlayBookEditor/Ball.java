package edu.bowdoin.robocup.TOOL.PlayBookEditor;

import java.awt.Point;

/**
 * Class to hold ball atributes for the PlayBook editor
 */
public class Ball
{
    // Class variables
    private boolean visible;
    private int x,y;
    // Constructors
    /**
     * Main constructor for the ball
     *
     * @param x Ball x location
     * @param y Ball y location
     */
    public Ball(int x, int y)
    {
	this.x = x;
	this.y = y;
    }

    public int getX() {return x;}
    public int getY() {return y;}
    public void setX(int newX) { x = newX;}
    public void setY(int newY) { y = newY;}
}