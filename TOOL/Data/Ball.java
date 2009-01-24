package TOOL.Data;

public interface Ball {

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
}
