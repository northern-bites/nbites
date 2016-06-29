package nbtool.gui.utilitypanes;

import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JFrame;

import nbtool.data.log.Log;
import nbtool.nio.LogRPC;
import nbtool.nio.RobotConnection;
import nbtool.util.Debug;
import nbtool.util.SharedConstants;

public class NowifiTestUtility extends UtilityParent {

	private Display display = null;

	@Override
	public JFrame supplyDisplay() {
		if (display == null) {
			return display = new Display();
		} else {
			return display;
		}
	}

	@Override
	public String purpose() {
		return "test Nowifi";
	}

	@Override
	public char preferredMemnonic() {
		return 'p';
	}

	private void doSendAction(NowifiTestPane pane) {
		String toSend = pane.thetaField.getText().trim();

		Debug.print("sending: %s", pane.thetaField.getText());

		RobotConnection robot = RobotConnection.getByIndex(0);

		if (robot != null) {
			Log empty = Log.emptyLog();
			empty.logClass = SharedConstants.LogClass_Null();
			empty.topLevelDictionary.put("AngleEnv", toSend);

			robot.addControlCall(LogRPC.NULL_RESPONDER, "SetAngleEnv", empty);
		}
	}

	private class Display extends JFrame {
		public NowifiTestPane testPane = new NowifiTestPane();

		public Display() {
			this.setContentPane(testPane);
			this.setMinimumSize(new Dimension(400,400));

			testPane.sendButton.addActionListener(new ActionListener(){
				@Override
				public void actionPerformed(ActionEvent e) {
					doSendAction(testPane);
				}
			});
		}
	}


}
