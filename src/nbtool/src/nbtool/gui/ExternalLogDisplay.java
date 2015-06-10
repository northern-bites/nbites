package nbtool.gui;

import java.awt.Dimension;

import javax.swing.JFrame;

public class ExternalLogDisplay extends JFrame {
	
	public static void request() {
		if (external == null) {
			external = new ExternalLogDisplay();
		} else {
			external.instanceRequest();
		}
	}
	
	private static ExternalLogDisplay external = null;
	
	private void instanceRequest() {
		ldp.setEnabled(true);
		this.setVisible(true);
		this.toFront();
	}
	
	private ExternalLogDisplay() {
		super("nbtool – external log display");
		this.setDefaultCloseOperation(JFrame.HIDE_ON_CLOSE);
		this.setSize(DEFAULT_SIZE);
		this.setMinimumSize(DEFAULT_SIZE);
		
		ldp = new LogDisplayPanel();
		this.setContentPane(ldp);
		this.setVisible(true);
	}

	private LogDisplayPanel ldp;
	
	private final Dimension DEFAULT_SIZE = new Dimension(600, 600);
}
