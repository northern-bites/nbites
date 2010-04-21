package TOOL.Vision;

import java.util.Vector;

import TOOL.Vision.LinePoint;

/**
 * class VisualLine - should somewhat reflect VisualLine from man/vision/VisualLine
 * in data structuring
 * it represents a field line that the bot recognizes
 *
 * @author Octavian Neamtu 2009
 */

public class VisualLine {

    //variables
    //beginx beginy endx endy
    private int bx, by, ex, ey;
    private Vector<LinePoint> points;

    public VisualLine(int bx,int by,int ex,int ey, 
		      Vector<LinePoint> pts) {
	this.bx = bx;
	this.by = by;
	this.ex = ex;
	this.ey = ey;
	this.points = pts;
    }

    //getters and setters
    public int getBeginX(){ return bx;}
    public int getBeginY(){ return by;}
    public int getEndX(){ return ex;}
    public int getEndY(){ return ey;}
    public Vector<LinePoint> getLinePoints(){ return points;}

    public void setBeginX(int bx){ this.bx = bx;}
    public void setBeginY(int by){ this.by = by;}
    public void setEndX(int ex){ this.ex = ex;}
    public void setEndY(int ey){ this.ey = ey;}
    public void setLinePoints(Vector <LinePoint> pts){ this.points = pts;}
}
