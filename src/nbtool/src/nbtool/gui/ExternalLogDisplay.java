package nbtool.gui;

import java.awt.Dimension;

import javax.swing.JFrame;

public class ExternalLogDisplay extends JFrame {
	//Somewhat arbitrary limit, the tool currently links one display with the number keys [1-9]
	public static final int MAX_EXTERNAL = 9;
	public static ExternalLogDisplay[] displays = new ExternalLogDisplay[9];
	
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
		
		ldp = new LogDisplayPanel(false);
		this.setContentPane(ldp);
		this.setVisible(true);
	}

	private LogDisplayPanel ldp;
	
	private final Dimension DEFAULT_SIZE = new Dimension(600, 600);
}
