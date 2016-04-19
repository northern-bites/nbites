package nbtool.gui;

import java.awt.ComponentOrientation;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.BoxLayout;
import javax.swing.ButtonGroup;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JRadioButton;

import nbtool.io.ControlIO;
import nbtool.io.ControlIO.ControlInstance;
import nbtool.util.Debug;
import nbtool.util.Utility;

public class FlagPanel extends JPanel implements ActionListener {
	private static final long serialVersionUID = 1L;
	private JRadioButton jrb[] = new JRadioButton[3];
	private JLabel lbl;
	
	private ButtonGroup bg;
	public String flag_name;
	public int index;

	
	public FlagPanel() {
		this.flag_name = "null";
		int nspace = 20 - flag_name.length();
		//even out lengths.
		this.flag_name += new String(new char[nspace]).replace("\0", " ");
		this.index = -1;
		
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
	}

	public void actionPerformed(ActionEvent e) {
		
		ControlInstance first = ControlIO.getByIndex(0);
		if (first == null) {
			Debug.log(Debug.WARN, "FlagPanel clicked while no ControlInstance available");
			this.setUnknown();
			return;
		}
		
		if (e.getSource() == jrb[0]) {
			boolean success = first.tryAddCmnd(ControlIO.createCmndSetFlag(index, false));
			Debug.logf(Debug.INFO, "FlagPanel[%s] CommandIO.tryAddSetFlag(%d, false) returned %B\n", flag_name, index, success);
		} else if (e.getSource() == jrb[1]) {
			Debug.logf(Debug.ERROR, "ERROR: FlagPanel " + flag_name + " got action from MIDDLE switch!");
		} else if (e.getSource() == jrb[2]) {
			boolean success = first.tryAddCmnd(ControlIO.createCmndSetFlag(index, true));
			Debug.logf(Debug.INFO, "FlagPanel[%s] CommandIO.tryAddSetFlag(%d, true) returned %B\n", flag_name, index, success);
		} else {}
		
		this.setUnknown();
	}
	
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
	
	public void setInfo(String name, int index) {
		this.flag_name = name;
		int nspace = 20 - name.length();
		//even out lengths.
		this.flag_name += new String(new char[nspace]).replace("\0", " ");
		this.index = index;
		
		lbl.setText(flag_name);
	}
}
