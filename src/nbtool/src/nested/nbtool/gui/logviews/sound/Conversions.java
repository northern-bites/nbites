package nbtool.gui.logviews.sound;

import nbtool.util.Debug;
import nbtool.util.test.TestBase;
import nbtool.util.test.Tests;

public class Conversions {
	
	//rate is frames / second
	
	//PERIOD is what I traditionally term buffer
	
	//1 cycle == 2PI radians...?
	
	public static double cyclesPerFrame(double rate, double fhertz) {
		return fhertz / rate;
	}
	
	public static double framesPerCycle(double rate, double fhertz) {
		return rate / fhertz;
	}
	
	public static double cyclesPerPeriod(double rate, double hertz, double frames) {
		return cyclesPerFrame(rate,hertz) * frames;
	}
	
	public static double bytesPerSecond(double rate, int channels, int sampleSize) {
		return rate * channels * sampleSize;
	}
	
	public static final int BYTES_PER_SHORT = 2;
	public static final int BYTES_PER_INT	= 4;

	public static void _NBL_ADD_TESTS_() {
		Tests.add("sound conversions", new TestBase("rate/hertz"){

			@Override
			public boolean testBody() throws Exception {
				
				double hertz = 1000;
				double rate = 500;
				
				assert(cyclesPerFrame(rate,hertz) == 2);
				
				return true;
			}
			
		});
	}
	
	public static void main(String[] args) {
		int rate = 96000;
		int targ = 1000;
		
		Debug.print("%d %d %f", rate, targ, framesPerCycle(rate, targ));
	}
	
}
