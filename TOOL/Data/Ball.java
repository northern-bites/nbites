package TOOL.Data;

public class Ball {

    //some constants
    public final static int DEF_H = 0, DEF_W = 0, DEF_X = 0, DEF_Y = 0;
    //variables
    private int x, y;
    private double h, w;

    //default constructor, just sets everything to 0
    public Ball() {
	this.x = DEF_X;
	this.y = DEF_Y;
	this.w = DEF_W;
	this.h = DEF_H;
    }

    //standard constructor
    public Ball(int x, int y, double width, double height) {
	this.x = x;
	this.y = y;
	this.w = width;
	this.h = height;
    }
    
    //setters
    public void setWidth(double w) {this.w = w;}
    public void setHeight(double h) {this.h = h;}
    public void setX(int x) {this.x = x;}
    public void setY(int y) {this.y = y;}

    //getters
    public double getWidth() {return w;}
    public double getHeight() {return h;}
    public int getX() {return x;}
    public int getY() {return y;}

    //Keeping this here as a reminder of stuff to implement
    /*
    public void setWidth(double w);
    public void setHeight(double h);
    public void setRadius(double r);
    public void setCenterX(int cx);
    public void setCenterY(int cy);
    public void setAngleX(double aX);
    public void setAngleY(double aY);
    public void setBearing(double b);
    public void setElevation(double e);
    public void setConfidence(int c);
    public void setX(int x1);
    public void setY(int y1);

    public void setDistance();
    public void setFocalDistance();
    public void findAngles();

    public int getX();
    public int getY();
    public double getWidth();
    public double getHeight();
    public double getRadius();
    public int getCenterX();
    public int getCenterY();
    public double getAngleX();
    public double getAngleY();
    public double getFocDist();
    public double getDist();
    public double getBearing();
    public double getElevation();
    public int getConfidence();
    */
}
