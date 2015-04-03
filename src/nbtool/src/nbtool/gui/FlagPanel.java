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

import nbtool.io.CommandIO;
import nbtool.util.U;

public class FlagPanel extends JPanel implements ActionListener {
	private static final long serialVersionUID = 1L;
	private JRadioButton jrb[] = new JRadioButton[3];
	private ButtonGroup bg;
	private String flag_name;
	private int index;
	
	public FlagPanel(String name, int i) {
		this.flag_name = name;
		this.index = i;
		this.setLayout(new BoxLayout(this, BoxLayout.LINE_AXIS));
		
		JLabel lbl = new JLabel(this.flag_name);
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
		if (e.getSource() == jrb[0]) {
			boolean success =
					CommandIO.tryAddSetFlag(index, false);
			U.wf("FlagPanel[%s] CommandIO.tryAddSetFlag(%d, false) returned %B\n", flag_name, index, success);
			
		} else if (e.getSource() == jrb[1]) {
			U.w("ERROR: FlagPanel " + flag_name + " got action from MIDDLE switch!");
		} else if (e.getSource() == jrb[2]) {
			System.out.println("2 " + jrb[2].isSelected());
			boolean success =
					CommandIO.tryAddSetFlag(index, true);
			U.wf("FlagPanel[%s] CommandIO.tryAddSetFlag(%d, true) returned %B\n", flag_name, index, success);
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
}
