package nbtool.gui.logviews.sound3;

public class SoundMath {
	
	public static double bearingFromOffset( double mOffset ) {
		return (Math.PI / 2) - Math.acos(mOffset / Constants.EAR_TO_EAR_SEPARATION);
	}
	
	public static double offsetDistanceFromOffsetPhase( double rOffset, int frequency ) {
		double sWavelength = 1d / frequency;
		double fracPeriod = rOffset / Constants.TAU;
		
		double mPeriod = (sWavelength * Constants.SOUND_MPS);
		return mPeriod * fracPeriod;
	}
	
	

}
