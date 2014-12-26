package nbclient.gui;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;

import javax.swing.DefaultComboBoxModel;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;

import nbclient.data.SessionHandler;
import nbclient.data.SessionHandler.STATUS;
import nbclient.data.OpaqueLog;
import nbclient.util.N;
import nbclient.util.N.EVENT;
import nbclient.util.N.NListener;
import nbclient.util.P;
import nbclient.util.U;

public class ControlPanel extends JPanel implements ActionListener, NListener {
	private static final long serialVersionUID = -3237233372276579379L;
	protected ControlPanel(SessionHandler ndh, LogDisplayPanel dp) {
		super();
		dh = ndh;
		ldp = dp;
		setLayout(null);
		addComponentListener(new ComponentAdapter() {
			public void componentResized(ComponentEvent e) {
				useSize(e.getComponent().getSize());
			}
		});
		
		N.listen(EVENT.LOGS_ADDED, this);
		N.listen(EVENT.SELECTION, this);
		N.listen(EVENT.STATUS, this);
		
		type = new JComboBox<String>(mode_strings);
		type.setSelectedIndex(0);
		type.addActionListener(this);
		
		settings_primary = new JComboBox<String>(P.getPrimaryValues());
		settings_secondary = new JComboBox<String>(P.getSecondaryValues());
		
		settings_primary.setToolTipText("<log directory>");
		settings_secondary.setToolTipText("address:port");

		settings_primary.setEditable(true);
		settings_secondary.setEditable(true);
		
		status = new JLabel("");
		status.setForeground(Color.RED);
		status.setAlignmentX(0);
		
		startb = new JButton("start");
		startb.setAlignmentX(0);
		startb.addActionListener(this);
		
		stream_desc = new JLabel("Stream");
		stream_box = new JCheckBox("objs w/ desc:");
		stream_field = new JTextField(15);

		stats = new StatsPanel(dh);
		
		add(type);
		add(settings_primary); add(settings_secondary);
		add(status); add(startb);
		
		add(stream_box); add(stream_desc); add(stream_field);
		
		add(stats);
		
		modeChangeAction();
		setStatusLabel();
		
		setVisible(true);
	}
	
	public void useSize(Dimension size) {
		int y_offset = 0;
		
		Dimension td = type.getPreferredSize();
		type.setBounds(0, y_offset, td.width, td.height);
		y_offset += td.height;
		
		Dimension d = settings_primary.getPreferredSize();
		settings_primary.setBounds(0, y_offset, size.width, d.height);
		y_offset += d.height;
		
		d = settings_secondary.getPreferredSize();
		settings_secondary.setBounds(0, y_offset, size.width, d.height);
		y_offset += d.height;
		
		d = status.getPreferredSize();
		status.setBounds(0, y_offset, size.width, d.height);
		y_offset += d.height;
		
		d = startb.getPreferredSize();
		startb.setBounds(0, y_offset, d.width, d.height);
		y_offset += d.height;
		y_offset += 30;
		
		int x_offset = 0;
		d = stream_desc.getPreferredSize();
		stream_desc.setBounds(0, y_offset + 4, d.width, d.height);
		x_offset += d.width;
		d = stream_box.getPreferredSize();
		stream_box.setBounds(x_offset, y_offset, d.width, d.height);
		y_offset += d.height;
		d = stream_field.getPreferredSize();
		stream_field.setBounds(x_offset, y_offset, d.width, d.height);
		y_offset += d.height + 20;
		
		stats.setBounds(0, y_offset, size.width, size.height - y_offset);
	}
	
	public void modelReturnAction() {
		ActionEvent e = new ActionEvent(startb, ActionEvent.ACTION_PERFORMED, "start/stop from keyPress");
		this.actionPerformed(e);
	}
	
	public void actionPerformed(ActionEvent e) {
		
		if (e.getSource() == startb) {
			if (dh.status == STATUS.IDLE) {
				startAction();
			}
			else if (dh.status == STATUS.RUNNING) {
				stopAction();
			}
			else {
				U.w("StatusPanel: DataHandler's mode not condusive to change.");
			}
			
		} else if (e.getSource() == type) {
			modeChangeAction();
		} else {
			U.w("ERROR: UNKNOWN EVENT: " + e);
		}
		
	}
	
	private void startAction() {
		String ptext = (String) settings_primary.getSelectedItem();
		String stext = (String) settings_secondary.getSelectedItem();
		
		if (ptext == null)
			ptext = "";
		if (stext == null)
			stext = "";
		
		settings_primary.setModel(new DefaultComboBoxModel<String>( P.putPrimaryValue(ptext) ));
		settings_secondary.setModel(new DefaultComboBoxModel<String>( P.putSecondValue(stext) ));
		
		switch(type.getSelectedIndex()) {
		case 0: {
			//network, saved
			SessionHandler.MODE m = SessionHandler.MODE.NETWORK_SAVING;			
			dh.start(m, ptext, stext);
		} break;
		case 1: {
			//network, unsaved
			SessionHandler.MODE m = SessionHandler.MODE.NETWORK_NOSAVE;
			dh.start(m, "",  stext);

		} break;
		case 2: {
			//from filesystem
			SessionHandler.MODE m = SessionHandler.MODE.FILESYSTEM;
			dh.start(m, ptext, "");
		} break;
		default: {
			U.w("ERROR: Unknown mode type in " + type.toString());
			return;
		}
		}
	}
	
	private void stopAction() {
		dh.stop();
	}
	
	private void modeChangeAction() {
		switch(type.getSelectedIndex()) {
		case 0: {
			//network, saved
			settings_primary.setEnabled(true);
			settings_secondary.setEnabled(true);
		} break;
		case 1: {
			//network, unsaved
			settings_primary.setEnabled(false);
			settings_secondary.setEnabled(true);
			
		} break;
		case 2: {
			//from fs
			settings_primary.setEnabled(true);
			settings_secondary.setEnabled(false);
		} break;
		default: {
			U.w("ERROR: Unknown mode type in: " + type.toString());
		}
		}
	}
	
	public void setStatusLabel() {
		String news = String.format("status: [%s, %s]", status_strings[dh.status.index], mode_strings[dh.mode.index]);
		status.setText(news);
	}
	
	public void setStatus(SessionHandler.STATUS s) {
		switch(s) {
		case IDLE:
			startb.setText("start");
			startb.setEnabled(true);
			break;
		case STARTING:
			startb.setEnabled(false);
			break;
		case RUNNING:
			startb.setText("stop");
			startb.setEnabled(true);
			break;
		case STOPPING:
			startb.setEnabled(false);
			break;
		}
		
		setStatusLabel();
	}
	
	public void notified(EVENT e, Object src, Object... args) {
		switch(e) {
		case STATUS:
			setStatus((STATUS) args[0]);
			break;
		case SELECTION:
			stream_box.setSelected(false);
		case LOGS_ADDED:
			if (stream_box.isSelected()) {
				OpaqueLog lg = (OpaqueLog) args[0];
				if (lg.description.contains(stream_field.getText())) {
					ldp.setContents(lg);
				}
			}
			
			break;
		default:
			break;
		}
		
	}
	
	private String[] status_strings = {"idle", "starting", "running", "stopping"};
	private String[] mode_strings = {"from network to filesystem", "from network, unsaved", "from filesystem", "NONE"};
	private JComboBox<String> type;
	private JButton startb;
	private JComboBox<String> settings_primary;
	private JComboBox<String> settings_secondary;
	private JLabel status;
	
	private JCheckBox stream_box;
	private JLabel stream_desc;
	private JTextField stream_field;
	
	private StatsPanel stats;
	
	private SessionHandler dh;
	private LogDisplayPanel ldp;
}
