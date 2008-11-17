package edu.bowdoin.robocup.TOOL.WorldController;

public class AngleUtilities {

    // Return the angle identitical to the input angle that is between -180 and 180 degrees
    public static double sub180Angle(double angle) {
        while (angle > 180.) {
            angle = angle - 360.;
        }
        while (angle < -180.) {
            angle = angle + 360.;
        }
        return angle;
    }

    // Return the angle indetical to the first angle such that it has a Euclidian distance from from_angle of less that 180 degrees.  
    public static double sub180Diff(double angle, double from_angle) {
        while (Math.abs(angle - from_angle) > 180.) {
            if (angle > 0) {
                angle = angle - 360.;
            }
            else {
                angle = angle + 360.;
            }
        }
        return angle;
    }

}
