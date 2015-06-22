package nbtool.gui.utilitypanes;

import java.awt.Color;

import javax.swing.DefaultListModel;
import javax.swing.JFrame;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.Timer;

import nbtool.data.Log;
import nbtool.gui.logdnd.LogDND;
import nbtool.gui.logdnd.LogDND.LogDNDTarget;

public class TestUtility extends UtilityParent {
	
	private class TestFrame extends JFrame {
		public TestFrame() {
		super();
		
		this.setTitle("t");
		this.setBounds(0, 0, 600, 400);
		JPanel content = new JPanel();
		content.setBackground(Color.RED);
		this.setContentPane(content);
				
		LogDND.makeComponentTarget(content, new LogDNDTarget(){

			@Override
			public void takeLogsFromDrop(Log[] log) {
				System.out.println("dropped " + log.toString());
				for (Log l : log)
					System.out.println("\t" + l);
			}
			
		});
		}
	}

	@Override
	public JFrame supplyDisplay() {
		return new TestFrame();
	}

	@Override
	public String purpose() {
		return "none";
	}

	@Override
	public char preferredMemnonic() {
		// TODO Auto-generated method stub
		return 0;
	}

}
