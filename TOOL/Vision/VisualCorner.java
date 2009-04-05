package TOOL.Vision;

//visual corner
//pretty simple ; needs more stuff from the robot side 

public class VisualCorner{
    
    private int x, y;

    public VisualCorner(int x, int y) {
	this.x = x;
	this.y = y;
    }

    //getters+setters
    public void setX() {this.x = x;}
    public void setY() {this.y = y;}
    
    public int getX() {return x;}
    public int getY() {return y;}
}