package nbtool.gui.logviews.sound2;

import java.util.Arrays;

public class Correlator {
	
	int used[];
	public double[] tsin, tcos;
	
	public Target target;
	
	public Correlator(int nc,
			double framesPerTargetPeriod,
			double targetAmplitude) {
		tsin = new double[nc];
		tcos = new double[nc];
		Arrays.fill(tsin, 0d);
		Arrays.fill(tcos, 0d);
		used = new int[nc];
		Arrays.fill(used, 0);
		
		target = new Target(framesPerTargetPeriod,
				targetAmplitude, 0.0);
	}
	
	public void correlate(int channel, int frame, int value) {
		this.correlate(channel, frame, (double) value);
	}
	
	public void correlate(int channel, int frame, double value) {
		tsin[channel] += target.sin(frame) * value;
		tcos[channel] += target.cos(frame) * value;
		
		++used[channel];
	}
	
	public double offset(int channel) {
		return Math.atan2(tcos[channel], tsin[channel]);
	}
	
	public double magnitude(int channel) {
		double sum = tcos[channel] * tcos[channel] + 
				tsin[channel] * tsin[channel];
		return Math.sqrt(sum);
	}
	
	public int nchannels() {return tsin.length;}
	
	public String toString() {
		StringBuilder sb = new StringBuilder(100);
		
		sb.append(String.format("Correlator(nc=%d, %s){\n",
				nchannels(), target.toString()
				));
		for (int i = 0; i < nchannels(); ++i) {
			sb.append(String.format("\tc%d: used %d offset %f mag %f\n", 
					i, used[i], offset(i), magnitude(i)));
		}
		sb.append("}");
		
		
		return sb.toString();
	}
}
