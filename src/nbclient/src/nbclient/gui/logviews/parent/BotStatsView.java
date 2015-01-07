package nbclient.gui.logviews.parent;

import java.awt.Dimension;
import java.io.IOException;

import javax.swing.JPanel;
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
		
		area = new JTextArea(bs.toString());
		add(area);
		Dimension d = area.getPreferredSize();
		area.setBounds(0, 0, d.width, d.height);
	}

	@Override
	protected void useSize(Dimension s) {
		// TODO Auto-generated method stub
		
	}
	
	public BotStatsView() {
		super();
	}

	JTextArea area;
}
