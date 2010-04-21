package TOOL.Vision;

//visual corner
//pretty simple ; needs more stuff from the robot side

public class VisualCorner{

	public static enum shape {INNER_L,
			OUTER_L,
			T,
			CIRCLE,
			UNKNOWN};


    private int x, y;
	private float distance, bearing;
	private shape cornerType;

    public VisualCorner(int x, int y,
						float distance,
						float bearing,
						int cornerType) {
	this.x = x;
	this.y = y;
	this.distance = distance;
	this.bearing = bearing;
	this.cornerType = shape.values()[cornerType];

    }

    //getters+setters
    public void setX() {this.x = x;}
    public void setY() {this.y = y;}

    public int getX() {return x;}
    public int getY() {return y;}

	public float getDistance() {return distance;}
	public float getBearing() {return bearing;}

	public shape getShape() {return cornerType;}
}