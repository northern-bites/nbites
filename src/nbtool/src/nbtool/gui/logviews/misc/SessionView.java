package nbtool.gui.logviews.misc;

import java.awt.GridLayout;

import javax.swing.JPanel;
import javax.swing.JTextField;

import nbtool.data.Session;

public class SessionView extends JPanel {
	
	public SessionView(Session s) {
		super();
		
		f1 = new JTextField("Session:: " + s.name);
		f2 = new JTextField(String.format("showing %d logs of %d total.", 
				s.logs_DO.size(), s.logs_ALL.size()));
		
		setLayout(new GridLayout(2, 1));
		
		add(f1);
		add(f2);
	}
	
	private JTextField f1, f2;
}
