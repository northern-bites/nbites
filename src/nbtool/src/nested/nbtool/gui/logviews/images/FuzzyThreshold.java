package nbtool.gui.logviews.images;

public class FuzzyThreshold {
	double x0;
	double x1;
	double m;
	
	public FuzzyThreshold(double x0_, double x1_) {
		x0 = x0_;
		x1 = x1_;
		m = 1 / (x1 - x0);
	}
	
	public double f(double x) {
		if (Double.isNaN(x) || x < x0)
			return 0;
		else if (x > x1)
			return 1;
		else
			return m*(x-x0);
	}
}