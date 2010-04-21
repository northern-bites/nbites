package TOOL.Misc;
// This is a placeholder for the C++ estimate we receive out of pixEstimate.
// Everything is public for convenience. There no need for setters and getters
// ever

public class Estimate {
    public double dist;
    public double elevation;
    public double bearing;
    public double x;
    public double y;

    public Estimate(double _dist, double _elevation, double _bearing,
                    double _x, double _y) {
        dist = _dist;
        elevation = _dist;
        bearing = _bearing;
        x = _x;
        y = _y;
    }
}