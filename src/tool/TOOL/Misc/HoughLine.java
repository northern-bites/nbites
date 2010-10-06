package TOOL.Misc;

import java.awt.geom.Point2D;
  // **********
  // *        *
  // *  Line  *
  // *        *
  // **********
  //
  // A geometric line in polar coords so all lines can be represented in 2 degrees of freedom.
public class HoughLine {

    public double r, t; // Radius and Theta coords of line
    public int rIndex, tIndex;  // Indices of line in Hough Space
    public int score;           // Hough accumulator count at point

    public HoughLine(int rIndex, int tIndex, double r, double t, int score){
        this.rIndex = rIndex;
        this.tIndex = tIndex & 0xFF;
        this.r = r;
        int n = (int)Math.floor(t / (2.0 * Math.PI)); // make 0 <= T < 2 PI
        this.t = t - n * 2.0 * Math.PI;
        this.score = score;
    }

    public Point2D.Double intersect(HoughLine other){
        Point2D.Double pt;

        double cs1 = Math.cos(t);
        double sn1 = Math.sin(t);
        double cs2 = Math.cos(other.t);
        double sn2 = Math.sin(other.t);
        double g = cs1 * sn2 - sn1 * cs2;
        if (g == 0)
            {
                pt = new Point2D.Double(0.0,0.0);
                return pt;
            }
        pt = new Point2D.Double(( sn2 * r - sn1 * other.r) / g,
                                (-cs2 * r + cs1 * other.r) / g);
        return pt;

    }

    // returns  true iff this line and the other line intersect
    // effect   set px, py to the intersection of this line and the other line
    public boolean intersect(HoughLine other, double rx, double ry)
    {
        double px, py;
        double cs1 = Math.cos(t);
        double sn1 = Math.sin(t);
        double cs2 = Math.cos(other.t);
        double sn2 = Math.sin(other.t);
        double g = cs1 * sn2 - sn1 * cs2;
        if (g == 0)
            {
                px = py = 0;
                return false;
            }

        px = ( sn2 * r - sn1 * other.r) / g;
        py = (-cs2 * r + cs1 * other.r) / g;
        return (Math.abs(px) <= rx && Math.abs(py) <= ry);
    }

}