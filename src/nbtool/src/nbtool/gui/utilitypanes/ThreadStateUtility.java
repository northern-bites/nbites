package nbtool.gui.utilitypanes;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.DefaultListModel;
import javax.swing.JFrame;
import javax.swing.JList;
import javax.swing.Timer;

import nbtool.data.Log;
import nbtool.gui.logdnd.LogDND;
import nbtool.gui.logdnd.LogDND.LogDNDTarget;
import nbtool.util.Logger;

public class ThreadStateUtility extends UtilityParent {
	

	private class TSU_Frame extends JFrame  implements ActionListener  {
		private JList<String> listDisplay;
		public TSU_Frame() {
			super();
			
			this.setTitle("thread state");
			this.setBounds(0, 0, 600, 400);
			
			listDisplay = new JList<String>(new DefaultListModel<String>());
			listDisplay.setEnabled(false);
			this.setContentPane(listDisplay);
			
			Timer swingTimer = new Timer(1000, this);
			swingTimer.setInitialDelay(0);
			swingTimer.start();
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
				
				listDisplay.setModel(dlm);
			}
			
		}
	}
	
	private TSU_Frame display = null;


	@Override
	public JFrame supplyDisplay() {
		if (display == null) {
			return (display = new TSU_Frame());
		}
		return display;
	}

	@Override
	public String purpose() {
		return "List active threads (including IO) in tool.";
	}

	@Override
	public char preferredMemnonic() {
		return 'k';
	}
}
