package nbtool.gui;

import java.awt.Dimension;
import java.awt.Font;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.nio.file.FileSystem;
import java.util.Map.Entry;

import javax.swing.DefaultComboBoxModel;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextField;
import javax.swing.SwingConstants;

import nbtool.data.BotStats;
import nbtool.data.Log;
import nbtool.data.SessionMaster;
import nbtool.io.CommandIO;
import nbtool.util.N;
import nbtool.util.N.NListener;
import nbtool.util.NBConstants;
import nbtool.util.N.EVENT;
import nbtool.util.NBConstants.FlagPair;
import nbtool.util.NBConstants.MODE;
import nbtool.util.NBConstants.STATUS;
import nbtool.util.P;
import nbtool.util.U;

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
		test.setEnabled(false);
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
		
		faddr = new JComboBox<String>(P.getAddrs());
		faddr.setToolTipText("robot address");
		faddr.setEditable(true);
		
		fpath = new JComboBox<String>(P.getPaths());
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
		
		modes.setSelectedIndex(P.getLastMode());
		for (FlagPanel fp : flags)
			fp.setUnknown();
		
		add(sp);
		
		N.listen(EVENT.LOG_FOUND, this);
		N.listen(EVENT.LOG_SELECTION, this);
		N.listen(EVENT.SES_SELECTION, this);
		N.listen(EVENT.STATUS, this);
		
		N.listen(EVENT.CNC_CONNECTION, this);
		N.listen(EVENT.REL_BOTSTAT, this);
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
		faddr.setBounds(d1.width + 3, y, s.width - d1.width - 10, d2.height);
		y += (d1.height > d2.height ? d1.height : d2.height) + 3;
		
		d2 = fpath.getPreferredSize();
		path.setBounds(0, y, d1.width, d1.height);
		fpath.setBounds(d1.width + 3, y, s.width - d1.width - 10, d2.height);
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

	public void model_returnKeypress() {
		ActionEvent e = new ActionEvent(go, ActionEvent.ACTION_PERFORMED, "start/stop from keyPress");
		this.actionPerformed(e);
	}
	
	public void actionPerformed(ActionEvent e) {
		if (e.getSource() == go) {
			
			if (SessionMaster.INST.isIdle()) {
				tryStart();
			} else {
				SessionMaster.INST.stopSession();
			}
			
		} else if (e.getSource() == test) {
			boolean success = CommandIO.tryAddTest();
			U.w("ControlPanel: CommandIO.tryAddTest() returned " + success);
		} else if (e.getSource() == modes) {
			int si = modes.getSelectedIndex();
			if (si < 0) return;
			
			useMode(NBConstants.MODE.values()[si]);
		} else {
			
		}
	}
	
	public void notified(EVENT e, Object src, Object... args) {
		switch (e) {
		case REL_BOTSTAT:
			BotStats bs= (BotStats) args[0];
			if (connected) {
				for (int i = 0; i < flags.length; ++i) {
					boolean val = bs.flags.bFlags[i + 2]; //2 for connection flags
					
					flags[i].setKnown(val);
				}
			}
			
			break;
		case CNC_CONNECTION:
			Boolean c = (Boolean) args[0];
			
			if (c) {
				connected = true;
				test.setEnabled(true);
			} else {
				
				connected = false;
				for (FlagPanel fp : flags)
					fp.setUnknown();
				
				test.setEnabled(false);
			}
			
			break;
		case LOG_FOUND:
			if (bstream.isSelected()) {
				Log l = (Log) args[0];
				if (l.description.contains(fstream.getText()))
					N.notifyEDT(EVENT.LOG_SELECTION, this, l);
			}
			break;
		case LOG_SELECTION:
		case SES_SELECTION:
			if (src != this)
				bstream.setSelected(false);
			break;
		case STATUS:
			STATUS s = (STATUS) args[0];
			useStatus(s);
			break;
		}
		
	}
	
	private void tryStart() {
		String fs_text = (String) fpath.getSelectedItem();
		String addr_text = (String) faddr.getSelectedItem();
		
		if (fs_text == null)
			fs_text = "";
		if (addr_text == null)
			addr_text = "";
		
		int cs = modes.getSelectedIndex();
		P.putLastMode(cs);
		
		if (cs < 0) 
			return;
		MODE m = NBConstants.MODE.values()[cs];
		
		switch(m) {
		case FILESYSTEM:
			fpath.setModel(new DefaultComboBoxModel<String>( P.putPaths(fs_text) ));
			break;
		case NETWORK_NOSAVE:
			faddr.setModel(new DefaultComboBoxModel<String>( P.putAddrs(addr_text) ));
			break;
		case NETWORK_SAVING:
			fpath.setModel(new DefaultComboBoxModel<String>( P.putPaths(fs_text) ));
			faddr.setModel(new DefaultComboBoxModel<String>( P.putAddrs(addr_text) ));
			break;
		case NONE:
			U.w("ControlPanel: cannot start that mode.");
			return;
		default:
			break;
		}
		
		SessionMaster.INST.startSession(m, fs_text, addr_text);
	}
	
	private void useMode(MODE m) {
		switch(m) {
		case FILESYSTEM:
			faddr.setEnabled(false);
			fpath.setEnabled(true);
			break;
		case NETWORK_NOSAVE:
			faddr.setEnabled(true);
			fpath.setEnabled(false);
			break;
		case NETWORK_SAVING:
			faddr.setEnabled(true);
			fpath.setEnabled(true);
			break;
		case NONE:
			faddr.setEnabled(false);
			fpath.setEnabled(false);
			break;
		default:
			break;
		
		}
	}
	
	private void useStatus(STATUS s) {
		switch (s) {
		case IDLE:
			go.setText("go");
			go.setEnabled(true);
			break;
		case RUNNING:
			go.setText("stop");
			go.setEnabled(true);
			break;
		case STARTING:
			go.setEnabled(false);
			break;
		case STOPPING:
			go.setEnabled(false);
			break;
		default:
			break;
		
		}
	}
	
	/*
	 * CNC
	 * */
	
	private boolean connected = false;
}
