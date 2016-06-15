package nbtool.gui.utilitypanes;

import javax.swing.JFrame;

public abstract class AutomaticTestUtility <K, R> extends UtilityParent {

	@Override
	public final JFrame supplyDisplay() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public abstract String purpose();

	@Override
	public abstract char preferredMemnonic();

	public abstract AutomaticCrossTester.TestInterface<K, R> getInterface();

	private class TestFrame extends JFrame {
		TestFrame() {
			super("testing: " + purpose());
		}
	}
}
