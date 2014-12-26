package nbclient.gui;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Insets;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;

import javax.swing.BorderFactory;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.border.Border;

import nbclient.data.SessionHandler;
import nbclient.data.Stats;
import nbclient.util.N;
import nbclient.util.N.EVENT;
import nbclient.util.N.NListener;

public class StatsPanel extends JPanel implements NListener {
	private static final long serialVersionUID = -3792669341719678442L;
	public StatsPanel(SessionHandler dh) {
		super();
		setLayout(null);
		addComponentListener(new ComponentAdapter() {
			public void componentResized(ComponentEvent e) {
				useSize(e.getComponent().getSize());
			}
		});
		
		Border b = BorderFactory.createLineBorder(Color.BLACK);
		setBorder(BorderFactory.createTitledBorder(b, "STATS"));
	
		nlField = new JLabel("num logs: 0");
		tbField = new JLabel("total bytes: 0");
		types_label = new JLabel("found log types:");
		tList = new JList<String>(
				(String[]) dh.stats.types.toArray(new String[dh.stats.types.size()]));
		
		add(nlField);
		add(tbField); add(types_label); add(tList);
		N.listen(EVENT.STATS, this);
	}
	
	public void useSize(Dimension size) {
		Insets is = this.getInsets();
		
		int y_offset = is.top;
		Dimension d;
		int x_offset = is.left;
		
		d = nlField.getPreferredSize();
		nlField.setBounds(x_offset, y_offset, d.width, d.height);
		y_offset += d.height;
		
		d = tbField.getPreferredSize();
		tbField.setBounds(x_offset, y_offset, d.width, d.height);
		y_offset += d.height;
		
		d= types_label.getPreferredSize();
		types_label.setBounds(x_offset, y_offset, d.width, d.height);
		y_offset += d.height;
		
		d = tList.getPreferredScrollableViewportSize();
		tList.setBounds(x_offset, y_offset, d.width - 40, d.height);
	}
	
	public void statsUpdate(Stats s) {
		nlField.setText("num logs: " + s.numlogs);
		tbField.setText("total bytes: " + s.totalbytes);
		
		tList.setListData((String[]) s.types.toArray(new String[s.types.size()]));
		tList.setVisibleRowCount(s.types.size());
		
		useSize(this.getSize());
	}
	
	public void notified(EVENT e, Object src, Object... args) {
		if (e == EVENT.STATS) {
			statsUpdate((Stats) src);
		}
	}
	
	private JLabel nlField;
	private JLabel tbField;
	private JLabel types_label;
	private JList<String> tList;
}
