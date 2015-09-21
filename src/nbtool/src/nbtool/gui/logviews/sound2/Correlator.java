package nbtool.gui.logviews.sound2;

import java.util.Arrays;

public class Correlator {
	
	private static final double TAU = Math.PI * 2;

	public int nchannels;
	public double[] tsin, tcos;
	
	public double rstep;
	public double target_amp;
	
	public Correlator(int nc,
			double framesPerTargetPeriod,
			double targetAmplitude) {
		tsin = new double[nc];
		tcos = new double[nc];
		Arrays.fill(tsin, 0d);
		Arrays.fill(tcos, 0d);

		nchannels = nc;
		
		rstep = TAU / framesPerTargetPeriod;
		target_amp = targetAmplitude;
	}
	
	public void correlate(int channel, int frame, int value) {
		this.correlate(channel, frame, (double) value);
	}
	
	public void correlate(int channel, int frame, double value) {
		double angle = frame * rstep;
		tsin[channel] += Math.sin(angle) * target_amp * value;
		tcos[channel] += Math.cos(angle) * target_amp * value;
	}
	
	public double offset(int channel) {
		return Math.atan2(tcos[channel], tsin[channel]);
	}
	
	public double magnitude(int channel) {
		double sum = tcos[channel] * tcos[channel] + 
				tsin[channel] * tsin[channel];
		return Math.sqrt(sum);
	}
}
