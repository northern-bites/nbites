package nbtool.gui.logviews.misc;

import java.awt.Dimension;
import java.awt.Font;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.io.IOException;

import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.JTextArea;

import nbtool.data.RobotStats;
import nbtool.data.Log;
import nbtool.util.Utility;

public class BotStatsView extends ViewParent {

	@Override
	public void setLog(Log newlog) {
		RobotStats bs = new RobotStats(newlog);
		
		if (area != null) {
			sp.remove(area);
		}
		
		area = new JTextArea(bs.toString());
		Font f = new Font("monospaced", Font.PLAIN, 14);
		area.setFont(f);
		
		sp.setViewportView(area);
	}

	private void useSize(Dimension s) {
		// TODO Auto-generated method stub
		sp.setBounds(0, 0, s.width, s.height);
	}
	
	public BotStatsView() {
		super();
		
		addComponentListener(new ComponentAdapter() {
			public void componentResized(ComponentEvent e) {
				useSize(e.getComponent().getSize());
			}
		});
		setLayout(null);
		
		sp = new JScrollPane();
		sp.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
		sp.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);
		
		add(sp);
		
		area = null;
	}

	JScrollPane sp;
	JTextArea area;
}
