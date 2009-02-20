package TOOL.Vision;

//simple class, sotres coordinates for the horizon line
//@author Octavian

public class Horizon{
    
    private int lx, ly, rx, ry;
    
    Horizon(int lx,int ly,int rx,int ry){
	this.lx = lx;
	this.ly = ly;
	this.rx = rx;
	this.ry = ry;
    }
    
    //getters + setters
    public void setLeftX(int lx){ this.lx = lx;}
    public void setLeftY(int ly){ this.ly = ly;}
    public void setRightX(int rx){ this.rx = rx;}
    public void setRightY(int ry){ this.ry = ry;}
    
    public int getLeftX(){ return lx;}
    public int getLeftY(){ return ly;}
    public int getRightX(){ return rx;}
    public int getRightY(){ return ry;}
}