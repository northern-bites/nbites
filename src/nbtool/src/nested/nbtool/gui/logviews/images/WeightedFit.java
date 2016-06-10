package nbtool.gui.logviews.images;

public class WeightedFit {
	double sumW;
	double sumX, sumY, sumXY, sumX2, sumY2;

	boolean solved;

	double pLen1, pLen2;         // principal lengths, pLen1 >= pLen2
	double uPAI, vPAI;           // components of unit vector along 1st principal axis
	double ssError;

	public WeightedFit() {
		clear();
	}

	public void clear() {
		sumW = sumX = sumY = sumXY = sumX2 = sumY2 = 0;
		solved = false;
	}

	public double getArea() {
		return sumW;
	}
	
	public double getCenterX() {
		return sumX / sumW;
	}

	public double getCenterY() {
		return sumY / sumW;
	}
	
	public double getFirstPrincipalLength() {
		solve(); return pLen1;
	}
	
	public double getSecondPrincipalLength() {
		solve(); return pLen2;
	}

	public boolean hasPrincipalAxes() {
		solve(); return (uPAI != 0 || vPAI != 0);
	}
	
	public double getFirstPrincipalAxisU() {
		solve(); return uPAI;
	}
	
	public double getFirstPrincipalAxisV() {
		solve(); return vPAI;
	}
	
	public double getSecondPrincipalAxisU() {
		solve(); return -vPAI;
	}
	
	public double getSecondPrincipalAxisV() {
		solve(); return uPAI;
	}

	public double getFirstPrincipalAngle() {
		solve(); return Math.atan2(vPAI, uPAI);
	}
	
	public double getSecondPrincipalAngle() {
		solve(); return Math.atan2(uPAI, -vPAI);
	}
	
	public double getSumSquaredError() {
		solve(); return ssError;
	}
	
	public double getRmsError() {
		return Math.sqrt(getSumSquaredError() / sumW);
	}

	public void add(double w, double x, double y) {
		sumW  += w;;
	    sumX  += w * x;
	    sumY  += w * y;
	    sumX2 += w * x * x;
	    sumY2 += w * y * y;
	    sumXY += w * x * y;
	    solved = false;
	}

	public void add(double x, double y)
	{
		add(1.0, x, y);
	}

	public void sub(double w, double x, double y) {
		sumW  -= w;;
	    sumX  -= w * x;
	    sumY  -= w * y;
	    sumX2 -= w * x * x;
	    sumY2 -= w * y * y;
	    sumXY -= w * x * y;
	    solved = false;
   }

	public void sub(double x, double y) {
		sub(1.0, x, y);
	}

	void solve() {
		if (solved)
			return;

		double a = sumW * sumX2 - sumX * sumX;
	    double b = sumW * sumY2 - sumY * sumY;
	    double c = 2.0 * ((double)sumW * sumXY - (double)sumX * sumY);
	    double d = Math.sqrt(c * c + (a - b) * (a - b));

	    pLen1 = Math.sqrt(1.5 * (a + b + d)) / sumW;
	    pLen2 = Math.sqrt(1.5 * (a + b - d)) / sumW;

	    if (d == 0.0)
	    	uPAI = vPAI = 0.0;
	    else {
	    	uPAI = Math.sqrt(((a - b) / d + 1) / 2);
	    	vPAI = Math.sqrt((1 - (a - b) / d) / 2);
	    	if (c < 0)
	    		uPAI = -uPAI;
	    }
	    solved = true;
	}
}