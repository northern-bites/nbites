package nbtool.gui.logviews.sound2;

public class Target {
	
	public static double TAU = Math.PI * 2;
	
	public double radians_per_frame;
	public double amplitude, phase;
	
	public Target(double framesPerPeriod, double amplitude,
			double phaseOffset) {
		this.amplitude = amplitude;
		this.phase = phaseOffset;
		this.radians_per_frame = TAU / framesPerPeriod;
	}
	
	private double angle(int frame) {
		return frame * radians_per_frame + phase;
	}

	public double sin(int frame) {
		return Math.sin(angle(frame)) * amplitude;
	}
	
	public double cos(int frame) {
		return Math.cos(angle(frame)) * amplitude;
	}
	
	public String toString() {
		return String.format("Target(%f rpf, %f amp, %f phs)", 
				radians_per_frame, amplitude, phase);
	}
}
