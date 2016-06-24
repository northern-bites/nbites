package nbtool.gui.utilitypanes;

import java.awt.Color;
import java.awt.Dimension;

import javax.swing.JFrame;

import nbtool.data.log.Log;
import nbtool.data.log.LogReference;

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

		final JFrame outerThis = this;

		Display() {
			super("testing: " + purpose());
			this.setMinimumSize(new Dimension(300,300));

			outerThis.setBackground(Color.BLACK);

			final AutomaticCrossTester.TestInterface<K, R> wrapped =
					getInterface();

			final AutomaticCrossTester.TestInterface<K, R> wrapper =
					new AutomaticCrossTester.TestInterface<K, R>(){

						@Override
						public void notifyTestStart() {
							wrapped.notifyTestStart();
							outerThis.setBackground(Color.YELLOW);
						}

						@Override
						public String testName() {
							return wrapped.testName();
						}

						@Override
						public K qualifiesForTesting(LogReference logr) {
							return wrapped.qualifiesForTesting(logr);
						}

						@Override
						public String crossFunctionName() {
							return wrapped.crossFunctionName();
						}

						@Override
						public void finishAnnotations(K key, Log log) {
							wrapped.finishAnnotations(key, log);
						}

						@Override
						public R getResult(LogReference from, K key, Log... out) {
							return wrapped.getResult(from, key, out);
						}

						@Override
						public boolean createGroupForResult(R resultType) {
							return wrapped.createGroupForResult(resultType);
						}

						@Override
						public String groupNameFor(R resultType) {
							return wrapped.groupNameFor(resultType);
						}

						@Override
						public void notifyTestFinished() {
							wrapped.notifyTestFinished();
							outerThis.setBackground(Color.GREEN);
						}
			};

			AutomaticCrossTester<K,R> tester = new AutomaticCrossTester<>(wrapper);
			tester.runTests();
		}
	}
}
