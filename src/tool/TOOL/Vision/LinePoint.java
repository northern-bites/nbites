package TOOL.Vision;

/**
 * class LinePoint - used to store a point on a visual line
 * 
 * @see VisualLine.java
 * @author Octavian Neamtu 2009
 *
 */

public class LinePoint{

    public static final int HORIZONTAL = 1;
    public static final int VERTICAL = 2;
    //variables
    private int x, y;
    private double lineWidth;
    private int foundWithScan;

    //just a default constructor
    public LinePoint(int x, int y, double lineWidth, int foundWithScan) {
	this.x = x;
	this.y = y;
	this.lineWidth = lineWidth;
	this.foundWithScan = foundWithScan;
    }
    
    //setters + getters
    public int getX() { return x;}
    public int getY() { return y;}
    public double getLineWidth() { return lineWidth;}
    public int foundWithScan() { return foundWithScan;}

    public void setX(int x) { this.x = x;}
    public void setY(int y) { this.y = y;}
    public void setLineWidth(double lineWidth) { this.lineWidth = lineWidth;}
    public void setFoundWithScan(int fws) { this.foundWithScan = fws;}
}
