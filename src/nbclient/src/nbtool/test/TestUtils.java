package nbtool.test;

import java.awt.Dimension;

import javax.swing.JFrame;
import javax.swing.JPanel;

public class TestUtils {
	
	public static void frameForPanel(JPanel p) {
		JFrame frame = new JFrame("test");
		frame.setLayout(null);
		frame.add(p);
		
		Dimension dp = p.getPreferredSize();
		frame.setSize(dp);
		p.setBounds(0, 0, dp.width, dp.height);
		
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.setVisible(true);
	}

}
