package nbtool.gui.logviews.misc;

import java.awt.Dimension;

import javax.swing.JTextField;

import nbtool.data.Log;

public class ErrorView extends ViewParent {
	
	public ErrorView() {
		super();
		
		efield = new JTextField();
		add(efield);
	}

	
	public void setLog(Log newlog) {
		efield.setText("ERROR: Could not load data bytes of log. "
				+ " Check log directory or reduce data usage.");
	}

	
	protected void useSize(Dimension s) {
		Dimension d = efield.getPreferredSize();
		efield.setBounds(0, 0, d.width, d.height);
	}

	private JTextField efield;
}
