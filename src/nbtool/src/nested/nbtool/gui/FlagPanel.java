package nbtool.gui;

import java.awt.ComponentOrientation;
import java.awt.Font;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.BoxLayout;
import javax.swing.ButtonGroup;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JRadioButton;

import nbtool.data.json.JsonObject;
import nbtool.data.log.Log;
import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOInstance;
import nbtool.nio.LogRPC;
import nbtool.nio.RobotConnection;
import nbtool.nio.RobotConnection.RobotFlag;
import nbtool.util.Debug;

public class FlagPanel extends JPanel implements ActionListener {
	private static final long serialVersionUID = 1L;
	private JRadioButton jrb[] = new JRadioButton[3];
	private JLabel lbl;
	
	private ButtonGroup bg;
	public String flag_name;
	public int index;
	public RobotConnection robot;
	
	private final int TOTAL_NAME_LENGTH = 40;
	
	public FlagPanel(RobotConnection robot, RobotFlag flag) {
		String name = flag.name;
		int index = flag.index;
		boolean value = flag.value;	
		
		this.robot = robot;
		
		this.flag_name = name;
		if (name.length() > TOTAL_NAME_LENGTH) {
			Debug.error("flag name %s is longer than %d chars, this will mess up the GUI!", name,
					TOTAL_NAME_LENGTH);
		} else {
			int nspace = TOTAL_NAME_LENGTH - name.length();
			//even out lengths.
			this.flag_name += new String(new char[nspace]).replace("\0", " ");
		}
		
		this.index = index;
		
		this.setLayout(new BoxLayout(this, BoxLayout.LINE_AXIS));
		
		lbl = new JLabel(this.flag_name);
		lbl.setFont(new Font("monospaced", Font.PLAIN, 12));
		add(lbl);
		bg = new ButtonGroup();
		jrb[0] = new JRadioButton("F");
		jrb[0].applyComponentOrientation(ComponentOrientation.RIGHT_TO_LEFT);
		bg.add(jrb[0]);
		add(jrb[0]);
		jrb[1] = new JRadioButton("?");
		jrb[1].applyComponentOrientation(ComponentOrientation.RIGHT_TO_LEFT);
		bg.add(jrb[1]);
		add(jrb[1]);
		jrb[2] = new JRadioButton("T");
		jrb[2].applyComponentOrientation(ComponentOrientation.RIGHT_TO_LEFT);
		bg.add(jrb[2]);
		add(jrb[2]);
		
		jrb[0].addActionListener(this);
		jrb[1].addActionListener(this);
		jrb[2].addActionListener(this);
		
		jrb[1].setEnabled(false);
		lbl.setText(flag_name);
		
		setKnown(value);		
	}

	public void actionPerformed(ActionEvent e) {
		if (robot == null || robot.finished()) {
			Debug.error( "FlagPanel clicked while no RobotConnection available!");
			this.setUnknown();
			return;
		}
		
		if (e.getSource() == jrb[0]) {
			LogRPC.setFlag(falseR, robot, index, false);
			Debug.info( "FlagPanel[%s] on %s LogRPC.setFlag(%d, false)\n", flag_name, robot, index);
		} else if (e.getSource() == jrb[1]) {
			Debug.error( "ERROR: FlagPanel " + flag_name + " got action from MIDDLE switch!");
		} else if (e.getSource() == jrb[2]) {
			LogRPC.setFlag(trueR, robot, index, true);
			Debug.info( "FlagPanel[%s] on %s LogRPC.setFlag(%d, true)\n", flag_name, robot, index);
		} else {
			Debug.error( "ERROR: FlagPanel " + flag_name + " got action from UNKNOWN!");
		}
		
		this.setUnknown();
	}
	
	private final IOFirstResponder falseR = new IOFirstResponder(){
		@Override
		public void ioFinished(IOInstance instance) {}

		@Override
		public void ioReceived(IOInstance inst, int ret, Log... out) {
			assert(inst == robot);
			setKnown(false);
		}

		@Override
		public boolean ioMayRespondOnCenterThread(IOInstance inst) {
			return false;
		}
	};
	
	private final IOFirstResponder trueR = new IOFirstResponder(){
		@Override
		public void ioFinished(IOInstance instance) {}

		@Override
		public void ioReceived(IOInstance inst, int ret, Log... out) {
			assert(inst == robot);
			setKnown(true);
		}

		@Override
		public boolean ioMayRespondOnCenterThread(IOInstance inst) {
			return false;
		}
	};
	
	public void setUnknown() {
		jrb[0].setEnabled(false);
		jrb[1].setSelected(true);
		jrb[2].setEnabled(false);
	}
	
	public void setKnown(boolean v) {
		jrb[v ? 2 : 0].setSelected(true);
		jrb[0].setEnabled(true);
		jrb[2].setEnabled(true);
	}
}
