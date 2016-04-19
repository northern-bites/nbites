package nbtool.gui.logviews.sound2;

public class ModMath {
	public static void test() {
		double a = 1.0 % 2.0;
	}
	
	/*
	 * return the smallest possible amount left is ahead of right by (may be negative)
	 * expects 0-TAU
	 * */
	public static double diff(double left, double right) {
		
		assert(left >= 0 && left <= 2*Math.PI);
		assert(right >= 0 && right <= 2*Math.PI);
		
		double center = left - right;
		double wrapped;
		
		if (left > right) {
			//wrap must be 
			//		left--->
			//<--rght
			
			wrapped = (left - 2 * Math.PI) - right;
		} else {
			//wrap must be 
			//<--left
			//		rght--->
			wrapped = left + (2 * Math.PI - right);
		}
		
		return Math.abs(center) < Math.abs(wrapped) ? center : wrapped;
	}
}
