package nbtool.gui.utilitypanes;

import java.awt.Dimension;
import java.awt.Font;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.LinkedList;
import java.util.List;

import javax.swing.JFrame;

import nbtool.data.SExpr;
import nbtool.data.group.AllGroups;
import nbtool.data.group.Group;
import nbtool.data.json.JsonArray;
import nbtool.data.log.Block;
import nbtool.data.log.Log;
import nbtool.data.log.LogReference;
import nbtool.gui.logviews.images.BallTestView;
import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOInstance;
import nbtool.nio.CrossServer;
import nbtool.nio.CrossServer.CrossInstance;
import nbtool.util.Debug;
import nbtool.util.Events;
import nbtool.util.Robots;
import nbtool.util.SharedConstants;


public class BallTestUtility extends UtilityParent {

	private static Display display = null;

	@Override
	public JFrame supplyDisplay() {
		if (display != null) {
			return display;
		} else {
			return (display = new Display());
		}
	}

	@Override
	public String purpose() {
		return "run all ball ground truth logs and detect errors";
	}

	@Override
	public char preferredMemnonic() {
		return 't';
	}

	private static Group topFalsePos = null;
	private static Group topFalseNeg = null;
	private static Group botFalsePos = null;
	private static Group botFalseNeg = null;

	private static int outstanding = 0;
	
	private static class ImageBall {
		int x, y;
		ImageBall(int _x, int _y) {this.x = _x; this.y = _y;}
	}

	private static void analyze(Log out, boolean top) {
		assert(out.logClass.equals("VisionReturn"));
		assert(out.blocks.size() > 3);

		Block found_block = null;
		for (Block block : out.blocks ) {
			if (block.type.equals(SharedConstants.SexprType_DEFAULT()) &&
					block.whereFrom.equals("nbcross-Vision-ball")) {
				found_block = block; break;
			}
		}

		if (found_block == null) {
			Debug.error("could not find ball block in VisionReturn!");
			return;
		}
		
		JsonArray actual = out.topLevelDictionary.get(BallTestView.BALLS_KEY).asArray();
		List<ImageBall> found = new LinkedList<>();
		
		SExpr found_s = found_block.parseAsSExpr();
		for (int i = 0; found_s.find("ball" + i).exists(); ++i) {
			SExpr thisBall = found_s.find("ball" + i);
			found.add(new ImageBall(0,0));
		}
		
		if (actual.size() > found.size()) {
			Debug.warn("false negative in %s!", top ? "top" : "bot");
		}
		
		if (actual.size() < found.size()) {
			Debug.warn("false positive in %s!", top ? "top" : "bot");
		}
	}
	
	private static String textFrom(Group g) {
		return String.format("%s: %d", g.getGroupInfo(), g.logs.size());
	}

	private static void finished() {
		if (display == null) {
			Debug.error("test finished but no display!");
			return;
		}
		
		if (topFalsePos != null) {
			display.panel.topFalsePosLabel.setText(textFrom(topFalsePos));
			Events.GGroupAdded.generate(display, topFalsePos);
		}

		if (topFalseNeg != null) {
			display.panel.topFalseNegLabel.setText(textFrom(topFalseNeg));
			Events.GGroupAdded.generate(display, topFalseNeg);
		}

		if (botFalsePos != null) {
			display.panel.botFalsePosLabel.setText(textFrom(botFalsePos));
			Events.GGroupAdded.generate(display, botFalsePos);
		}

		if (botFalseNeg != null) {
			display.panel.botFalseNegLabel.setText(textFrom(botFalseNeg));
			Events.GGroupAdded.generate(display, botFalseNeg);
		}
	}

	private static class TestResponder implements IOFirstResponder {

		boolean top;
		TestResponder(boolean t) { this.top = t; }

		@Override
		public void ioFinished(IOInstance instance) {}

		@Override
		public void ioReceived(IOInstance inst, int ret, Log... out) {
			--outstanding;
			analyze(out[0], top);

			if (outstanding == 0) finished();
		}

		@Override
		public boolean ioMayRespondOnCenterThread(IOInstance inst) {
			return false;
		}

		static TestResponder get(boolean top) { return new TestResponder(top); }
	}

	private static void runTest(Log log, boolean top) {
		CrossInstance ci = CrossServer.instanceByIndex(0);
		if (ci == null) {
			Debug.error("nbcross crashed during ball test!");
			return;
		}

		++outstanding;
		ci.tryAddCall(TestResponder.get(top), "Vision", log);
	}

	private static void runTests() {
		List<LogReference> found_top = new LinkedList<>();
		List<LogReference> found_bot = new LinkedList<>();
		int tripoint = 0;

		for (Group group : AllGroups.allGroups) {
			for (LogReference ref : group.logs) {

				if (ref.logClass.equals(
						SharedConstants.LogClass_Tripoint()) ) {
					++tripoint;
					
					if (ref.description.contains(BallTestView.BALLS_KEY)) 
					{	
						if (ref.description.contains("camera_TOP")) {
							found_top.add(ref);
						} else {
							found_bot.add(ref);
						}

					}
				}
			}
		}

		Debug.info("found %d testable top logs.", found_top.size());
		Debug.info("found %d testable bot logs.", found_bot.size());
		CrossInstance ci = CrossServer.instanceByIndex(0);

		if (display != null)
			display.panel.usedLabel.setText(
					String.format("used %d top, %d bot logs of %d total tripoint", 
							found_top.size(), found_bot.size(), tripoint)
					);

		if (ci == null) {
			Debug.error("cannot test ball detection without nbcross.");
			return;
		}

		assert(outstanding == 0);
		for (LogReference ref : found_top) runTest(ref.get(), true);
		for (LogReference ref : found_bot) runTest(ref.get(), false);
	}

	private static class Display extends JFrame {

		final BallTestPanel panel = new BallTestPanel();

		Display() {
			super("ball tester");
			this.setContentPane(panel);
			this.setMinimumSize(new Dimension(600,300));

			this.panel.goButton.addActionListener(new ActionListener(){
				@Override
				public void actionPerformed(ActionEvent e) {
					runTests();
				}	
			});

			panel.usedLabel.setFont(panel.usedLabel.getFont().deriveFont(Font.BOLD));
		}
	}
}
