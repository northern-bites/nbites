package nbclient.gui;

import java.awt.Dimension;
import java.awt.Font;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.util.Map.Entry;

import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextField;
import javax.swing.SwingConstants;

import nbclient.util.N;
import nbclient.util.N.NListener;
import nbclient.util.NBConstants;
import nbclient.util.N.EVENT;
import nbclient.util.NBConstants.FlagPair;
import nbclient.util.P;

public class ControlPanel extends JPanel implements ActionListener, NListener {
	private static final long serialVersionUID = 1L;

	public ControlPanel() {
		super();
		setLayout(null);
		addComponentListener(new ComponentAdapter() {
			public void componentResized(ComponentEvent e) {
				useSize(e.getComponent().getSize());
			}
		});
		canvas = new JPanel();
		canvas.setLayout(null);
		
		go = new JButton("start");
		go.addActionListener(this);
		canvas.add(go);
		
		test = new JButton("test");
		test.addActionListener(this);
		canvas.add(test);
		
		modes = new JComboBox<String>(NBConstants.mode_strings);
		modes.setSelectedIndex(0);
		modes.addActionListener(this);
		canvas.add(modes);
		
		addr = new JLabel(" address:");
		path = new JLabel(" path:");
		stream = new JLabel(" stream");
		cnc = new JLabel("CNC:");
		cnc.setFont(cnc.getFont().deriveFont(Font.BOLD));
		
		faddr = new JComboBox<String>(P.getPrimaries());
		faddr.setToolTipText("robot address");
		faddr.setEditable(true);
		
		fpath = new JComboBox<String>(P.getSecondaries());
		fpath.setToolTipText("directory path");
		fpath.setEditable(true);
		
		canvas.add(addr);
		canvas.add(path);
		canvas.add(stream);
		canvas.add(cnc);
		canvas.add(faddr);
		canvas.add(fpath);
		
		fstream = new JTextField(15);
		canvas.add(fstream);
		bstream = new JCheckBox("objects w/ desc:");
		canvas.add(bstream);
		
		flags = new FlagPanel[NBConstants.flags.size()];
		int i = 0;
		for (FlagPair f : NBConstants.flags) {
			FlagPanel fp = new FlagPanel(f.name, f.index);
			flags[i++] = fp;
			canvas.add(fp);
		}
		
		sp = new JScrollPane();
		sp.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED);
		sp.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
		sp.setViewportView(canvas);
		
		add(sp);
		
		N.listen(EVENT.LOG_FOUND, this);
		N.listen(EVENT.LOG_SELECTION, this);
		N.listen(EVENT.SES_SELECTION, this);
		N.listen(EVENT.STATUS, this);
	}
	
	public void actionPerformed(ActionEvent e) {
		
		
	}

	
	private void useSize(Dimension s) {
		sp.setBounds(0, 0, s.width, s.height);
		
		//update component sizes...
		Dimension d1, d2, d3;
		int y = 0;
		int max_x = 260;
		
		d1 = go.getPreferredSize();
		go.setBounds(0, y, d1.width, d1.height);
		d2 = modes.getPreferredSize();
		modes.setBounds(d1.width + 3, y, d2.width, d2.height);
		y += (d1.height > d2.height ? d1.height : d2.height) + 3;
		
		
		d2 = faddr.getPreferredSize();
		addr.setBounds(0, y, d1.width, d1.height);
		faddr.setBounds(d1.width + 3, y, d2.width, d2.height);
		y += (d1.height > d2.height ? d1.height : d2.height) + 3;
		
		d2 = fpath.getPreferredSize();
		path.setBounds(0, y, d1.width, d1.height);
		fpath.setBounds(d1.width + 3, y, d2.width, d2.height);
		y += (d1.height > d2.height ? d1.height : d2.height) + 5;
		
		
		d2 = stream.getPreferredSize();
		stream.setBounds(0, y + 3, d2.width, d2.height);
		d3 = bstream.getPreferredSize();
		bstream.setBounds(d2.width, y, d3.width, d3.height);
		y += d3.height;
		d3 = fstream.getPreferredSize();
		fstream.setBounds(d2.width, y, d3.width, d3.height);
		y += d3.height + 5;
		
		cnc.setBounds(0, y, d1.width, d1.height);
		d2 = test.getPreferredSize();
		test.setBounds(d1.width, y, d2.width, d2.height);
		y += d2.height;
		
		for (int i = 0; i < flags.length; ++i) {
			d1 = flags[i].getPreferredSize();
			flags[i].setBounds(0, y, d1.width, d1.height);
			y += d1.height;
		}
		
		canvas.setPreferredSize(new Dimension(max_x, y));
	}
	
	private JScrollPane sp;
	private JPanel canvas;
	
	private JButton go, test;
	private JComboBox<String> modes, faddr, fpath;
	private JLabel addr, path, stream, cnc;
	
	private JTextField fstream;
	private JCheckBox bstream;
	
	private FlagPanel[] flags;

	
	public void notified(EVENT e, Object src, Object... args) {
		switch (e) {
		case LOG_FOUND:
			break;
		case LOG_SELECTION:
		case SES_SELECTION:
			break;
		case STATUS:
			break;
		}
		
	}
	
	private void startAction() {
		
	}
	
	private void stopAction() {
		
	}
	
	private boolean starting;
	private void setStartBText() {
		go.setText(starting ? "go" : "stop");
	}
}
