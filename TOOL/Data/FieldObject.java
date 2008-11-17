package edu.bowdoin.robocup.TOOL.Data;

public interface FieldObject {

    // SETTERS
    public void setWidth(double w1);
    public void setHeight(double h1);
    public void setX(int x1);
    public void setY(int y1);
    public void setAngleX(double aX);
    public void setAngleY(double aY);
    public void setBearing(double b);
    public void setElevation(double e);
    public void setFocDist(double fd);
    public void setDist(double d);
    public void setCertainty(int c);
    public void setDistCertainty(int c);
    public void setShoot(boolean s1);
    public void setBackLeft(int x1);
    public void setBackRight(int y1);
    public void setBackDir(int x1);
    public void setLeftOpening(int op);
    public void setRightOpening(int op);

    // GETTERS
    public int getX();
    public int getY();
    public double getWidth();
    public double getHeight();
    public int getLeftTopX();
    public int getLeftTopY();
    public int getRightTopX();
    public int getRightTopY();
    public int getLeftBottomX();
    public int getLeftBottomY();
    public int getRightBottomX();
    public int getRightBottomY();
    public int getCenterX();
    public int getCenterY();
    public int getCertainty();
    public int getDistCertainty();
    public double getAngleX();
    public double getAngleY();
    public double getFocDist();
    public double getDist();
    public double getBearing();
    public double getElevation();
    public int getShootLeft();
    public int getShootRight();
    public int getBackDir();
    public int getLeftOpening();
    public int getRightOpening();
    public boolean shotAvailable();
}
