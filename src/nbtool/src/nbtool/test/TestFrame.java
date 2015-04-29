package nbtool.test;

import java.awt.Color;

import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JSplitPane;

public class TestFrame extends JFrame {
	public static void main(String[] args) {
		new TestFrame();
	}
	
	public TestFrame() {
		super("test");
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setSize(400,400);
		
		JPanel p1 = new JPanel();
		p1.setBackground(Color.RED);
		JPanel p2 = new JPanel();
		p2.setBackground(Color.ORANGE);
		JPanel p3 = new JPanel();
		p3.setBackground(Color.YELLOW);
		
		JSplitPane split1 = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT, false, p1, p2);
		JSplitPane split2 = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT, false, split1, p3);
		
		add(split2);
		//split1.setDividerLocation(.5);
		//split2.setDividerLocation(.5);
		split1.setResizeWeight(.2);
		split2.setResizeWeight(.85);
		
		setVisible(true);
	}
}
