package edu.bowdoin.robocup.TOOL.WorldController;

public class LocalizationPacket {
    public boolean includes_uncert;
    public double x_est, y_est, heading_est, head_pan;
    public double x_uncert, y_uncert, heading_uncert;
    public double min_visible_distance, max_visible_distance;
    public double x_velocity, y_velocity;

    private LocalizationPacket (double at_x_est, double at_y_est, double at_heading_est, double at_head_pan,
                                double at_x_uncert, double at_y_uncert, double at_heading_uncert, boolean include_uncert,
                                double at_min_distance, double at_max_distance, double at_x_velocity, double at_y_velocity) {
        includes_uncert = include_uncert;
        x_est = at_x_est;
        y_est = at_y_est;
        heading_est = at_heading_est;
        head_pan = at_head_pan;
        x_uncert = at_x_uncert;
        y_uncert = at_y_uncert;
        heading_uncert = at_heading_uncert;
        min_visible_distance = at_min_distance;
        max_visible_distance = at_max_distance;
	x_velocity = at_x_velocity;
	y_velocity = at_y_velocity;
    }

    public static LocalizationPacket makeFieldPointLocation(double at_x, double at_y) {
        return new LocalizationPacket(at_x, at_y, 0., 0., 0., 0., 0., false, 0., 0., 0., 0.);
    }

    public static LocalizationPacket makeLandmarkLocation(double at_x, double at_y, double at_min_visible, double at_max_visible) {
        return new LocalizationPacket(at_x, at_y, 0., 0., 0., 0., 0., false, at_min_visible, at_max_visible, 0., 0.);
    }

    public static LocalizationPacket makeDogLocation(double at_x, double at_y, double at_heading, double at_pan) {
        return new LocalizationPacket(at_x, at_y, at_heading, at_pan, 0., 0., 0., false, 0., 0., 0., 0.);
    }

    public static LocalizationPacket makeEstimateOnlyPacket(double at_x, double at_y, double at_heading) {
        return new LocalizationPacket(at_x, at_y, at_heading, 0., 0., 0., 0., false, 0., 0., 0., 0.);
    }

    public static LocalizationPacket makeBallEstimateAndUncertPacket(double ball_x, double ball_y, double ball_x_uncert, double ball_y_uncert, double ball_x_velocity, double ball_y_velocity) {
	return new LocalizationPacket(ball_x, ball_y, 0.0, 0.0, ball_x_uncert, ball_y_uncert, 0.0, true, 0.0, 0.0, ball_x_velocity, ball_y_velocity); 
    }

    public static LocalizationPacket makeEstimateAndUncertPacket(double at_x_est, double at_y_est, double at_heading_est, double at_head_pan,double at_x_uncert, double at_y_uncert, double at_heading_uncert) {
        return new LocalizationPacket(at_x_est, at_y_est, at_heading_est, at_head_pan, at_x_uncert, at_y_uncert, at_heading_uncert, true, 0., 0., 0., 0.);
    }

    public double getXActual() {
        return x_est;
    }

    public double getYActual() {
        return y_est;
    }

    public double getHeadingActual() {
        return heading_est;
    }

    public double getHeadPanActual() {
        return head_pan;
    }

    public double getXEst() {
        return x_est;
    }

    public double getYEst() {
        return y_est;
    }

    public double getHeadingEst() {
        return heading_est;
    }

    public double getHeadPanEst() {
        return head_pan;
    }

    public double getXUncert() {
        return x_uncert;
    }

    public double getYUncert() {
        return y_uncert;
    }
    
    public double getHUncert() {
	return heading_uncert;
    }

    public double getXVelocity() {
	return x_velocity;
    }

    public double getYVelocity() {
	return y_velocity;
    }

    public double getMinVisibleDistance() {
        return min_visible_distance;
    }

    public double getMaxVisibleDistance() {
        return max_visible_distance;
    }

    public void incrementXBy(double by_x) {
        x_est += by_x;
    }

    public void incrementYBy(double by_y) {
        y_est += by_y;
    }

    public void incrementHeadingBy(double by_theta) {
        heading_est += by_theta;
    }

    public void setHeadPan(double to_pan) {
        head_pan = to_pan;
    }
}
