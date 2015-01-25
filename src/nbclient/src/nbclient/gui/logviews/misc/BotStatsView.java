package nbclient.gui.logviews.misc;

import java.awt.Dimension;
import java.awt.Font;
import java.io.IOException;

import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.JTextArea;

import nbclient.data.BotStats;
import nbclient.data.Log;
import nbclient.util.U;

public class BotStatsView extends ViewParent {

	@Override
	public void setLog(Log newlog) {
		this.log = newlog;
		BotStats bs = null;
		
		try {
			bs = new BotStats(newlog);
		} catch (IOException e) {
			e.printStackTrace();
			return;
		}
		
		if (area != null) {
			sp.remove(area);
		}
		
		area = new JTextArea(bs.toString());
		Font f = new Font("monospaced", Font.PLAIN, 14);
		area.setFont(f);
		
		sp.setViewportView(area);
	}

	@Override
	protected void useSize(Dimension s) {
		// TODO Auto-generated method stub
		sp.setBounds(0, 0, s.width, s.height);
	}
	
	public BotStatsView() {
		super();
		
		sp = new JScrollPane();
		sp.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
		sp.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);
		
		add(sp);
		
		area = null;
	}

	JScrollPane sp;
	JTextArea area;
}
