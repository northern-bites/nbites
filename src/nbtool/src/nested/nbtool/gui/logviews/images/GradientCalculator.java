package nbtool.gui.logviews.images;

public class GradientCalculator {
	public static double[] calculateGrad(int[] input) {
		double[] grad = new double[2];
    	grad[0] = -1*input[0] + 1*input[2] - 2*input[3] + 2*input[5] - 1*input[6] + 1*input[8]; // X
		grad[1] = -1*input[0] - 2*input[1] - 1*input[2] + 1*input[6] + 2*input[7] + 1*input[8]; // Y
	
		return grad;
	}
	
	public static double calculateGradDir(double[] grad) {
    	double gradX = grad[0];
    	double gradY = grad[1];
    	
    	if (gradX == 0 & gradY == 0)
    		return Double.NaN;
    	if (gradX == 0)
    		return (gradY > 0 ? 90.0 : -90.0);
    	
		double angle = java.lang.Math.toDegrees(java.lang.Math.atan(java.lang.Math.abs(gradY / gradX)));
    	if (gradX >= 0 && gradY >= 0)
    		return angle;
    	else if (gradX <= 0 && gradY <= 0)
    		return angle + 180;
    	else if (gradX <= 0)
    		return 180 - angle;
    	else
    		return -1*angle;
    }
	
	public static double calculateGradMagn(double[] grad) {
    	double gradX = grad[0];
    	double gradY = grad[1];
    	
    	return java.lang.Math.sqrt(gradX*gradX + gradY*gradY);
	}
}
