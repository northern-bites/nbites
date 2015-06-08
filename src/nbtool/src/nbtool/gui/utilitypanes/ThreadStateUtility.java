package nbtool.gui.utilitypanes;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.DefaultListModel;
import javax.swing.JList;
import javax.swing.Timer;

import nbtool.util.Logger;

public class ThreadStateUtility extends UtilityParent implements ActionListener {
	private JList<String> display;
	public ThreadStateUtility() {
		super();
		
		this.setTitle("thread state");
		this.setBounds(0, 0, 600, 400);
		
		display = new JList<String>(new DefaultListModel<String>());
		display.setEnabled(false);
		this.setContentPane(display);
		
		Timer swingTimer = new Timer(1000, this);
		swingTimer.setInitialDelay(0);
		swingTimer.start();
	}

	@Override
	public Object getCurrentValue() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void actionPerformed(ActionEvent e) {
		//Logger.log(Logger.INFO, " not updating thread pane...");
				
		if (this.isFocused()) {
			Logger.log(Logger.INFO, "updating thread pane...");
			Thread[] threads = new Thread[Thread.activeCount()];
			Thread.enumerate(threads);
			DefaultListModel<String> dlm = new DefaultListModel<String>();
			
			for (Thread t : threads) {
				dlm.addElement( String.format("%s active=%b state=%s", t.getName(),
						t.isAlive(), t.getState()));
			}
			
			display.setModel(dlm);
		}
		
	}

}
