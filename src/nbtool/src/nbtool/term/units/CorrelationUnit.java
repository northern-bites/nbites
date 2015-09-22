package nbtool.term.units;

import nbtool.gui.logviews.sound2.Correlator;
import nbtool.gui.logviews.sound2.Target;
import nbtool.util.Logger;

public class CorrelationUnit extends UnitParent {

	@Override
	public boolean test(String absPathToResources) 
			throws UnitFailedException {
		
		Target zero = new Target(4.0, 1000.0, 0.0);
		Target piAhd = new Target(4.0, 1000.0, Math.PI);
		//Logger.printf("Correlation targets:\n%s\n%s\n", 
		//		zero, piAhd);
		
		Correlator cor = new Correlator(2, 4.0, 1.0);
		
		for (int i = 0; i < 1024; ++i) {
			cor.correlate(0, i, zero.sin(i));
			cor.correlate(1, i, piAhd.sin(i));
		}
		
		require( dequal(Math.abs(cor.offset(1)), Math.PI) , 
				"pi ahead test: " + cor.toString());
		
		Target one = new Target(4.0, 1000.0, 1.0);
		Correlator cor2 = new Correlator(2, 4.0, 1.0);
		for (int i = 0; i < 4096; ++i) {
			cor2.correlate(0, i, zero.sin(i));
			cor2.correlate(1, i, one.sin(i));
		}
		
		require( dequal(cor2.offset(1), 1.0),
				"1.0 offset: " + cor2.toString());
		
		require( dequal(cor2.magnitude(0), cor2.magnitude(1)),
				"magnitude with phase offset: " + cor2.toString() );
		
		return true;
	}

	@Override
	public String toString() {
		return "Correlation Unit";
	}
	
	private final double epsilon = 1e-6;
	private boolean dequal(double a, double b) {
		return Math.abs(a - b) < epsilon;
	}

}
