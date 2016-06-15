package nbtool.gui.utilitypanes;

import java.awt.Dimension;

import javax.swing.JFrame;

public abstract class AutomaticTestUtility <K, R> extends UtilityParent {

	private Display display = null;

	@Override
	public JFrame supplyDisplay() {
		if (display != null) {
			return display;
		} else {
			return (display = new Display());
		}
	}

	@Override
	public abstract String purpose();

	@Override
	public abstract char preferredMemnonic();

	public abstract AutomaticCrossTester.TestInterface<K, R> getInterface();

	private class Display extends JFrame {
		Display() {
			super("testing: " + purpose());
			this.setMinimumSize(new Dimension(300,300));

			AutomaticCrossTester<K,R> tester = new AutomaticCrossTester<>(getInterface());
			tester.runTests();
		}
	}
}
