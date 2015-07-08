package nbtool.gui;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.Font;
import java.awt.GridLayout;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.lang.management.ManagementFactory;
import java.util.HashSet;

import javax.swing.BoxLayout;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;
import javax.swing.text.DefaultCaret;

import nbtool.data.RobotStats;
import nbtool.data.Log;
import nbtool.data.SessionMaster;
import nbtool.data.ToolStats;
import nbtool.io.ControlIO.ControlInstance;
import nbtool.io.CrossIO.CrossInstance;
import nbtool.io.FileIO.FileInstance;
import nbtool.io.StreamIO.StreamInstance;
import nbtool.util.Center;
import nbtool.util.Events;
import nbtool.util.Logger;
import nbtool.util.NBConstants;
import nbtool.util.NBConstants.STATUS;
import nbtool.util.Utility;

public class StatusPanel extends JPanel implements Events.ControlStatus, Events.CrossStatus, Events.FileIOStatus,
	Events.StreamIOStatus, Events.LogsFound, Events.ToolStats, Events.ToolStatus, Events.RelevantRobotStats{

	public StatusPanel() {
		super();
		setLayout(null);
		addComponentListener(new ComponentAdapter() {
			public void componentResized(ComponentEvent e) {
				useSize(e.getComponent().getSize());
			}
		});
		
		types = new HashSet<String>();
		
		canvas = new JPanel();
		canvas.setLayout(null);
		
		Font msf = new Font("monospaced", Font.PLAIN, 14);
		Font sf = new Font("monospaced", Font.PLAIN, 12);
		Font bsf = msf.deriveFont(Font.BOLD, 15);
		
		serv = new JLabel("[serv]");
		serv.setFont(bsf);
		cnc = new JLabel("[cnc]");
		cnc.setFont(bsf);
		cpp = new JLabel("[cpp]");
		cpp.setFont(bsf);
		fst = new JLabel("[fst]");
		fst.setFont(bsf);
		
		serv.setForeground(Color.GRAY);
		cnc.setForeground(Color.GRAY);
		cpp.setForeground(Color.GRAY);
		fst.setForeground(Color.GRAY);
		
		up_container = lineAxisContainer();
		up_container.add(serv);
		up_container.add(cnc);
		up_container.add(cpp);
		up_container.add(fst);
		
		mode_status = new JLabel(":");
		mode_status.setFont(msf);
		canvas.add(mode_status);
		
		jvm_heap = new JLabel();
		jvm_heap.setFont(msf);
		jvm_max = new JLabel();
		jvm_max.setFont(msf);
		setJVM_labels();
		
		l_found = new JLabel();
		l_found.setFont(msf);
		canvas.add(l_found);
		
		s_found = new JLabel();
		s_found.setFont(msf);
		canvas.add(s_found);
		
		db_found = new JLabel();
		db_found.setFont(msf);
		canvas.add(db_found);
		
		db_cur = new JLabel();
		db_cur.setFont(msf);
		canvas.add(db_cur);
		
		db_dropped = new JLabel();
		db_dropped.setFont(msf);
		canvas.add(db_dropped);
		
		typeArea = new JTextArea();
		typeArea.setColumns(100);
		typeArea.setFont(msf);
		typeArea.setEditable(false);
		
		typeArea.setOpaque(false);
		
		canvas.add(typeArea);
		
		botStatA = new JTextArea();
		botStatA.setFont(sf);
		botStatA.setEditable(false);
		botStatA.setAutoscrolls(false);
		botStatA.setText("<no STATS logs found>");
		
		DefaultCaret caret = (DefaultCaret)botStatA.getCaret();
		caret.setUpdatePolicy(DefaultCaret.NEVER_UPDATE);
		
		canvas.add(botStatA);
		
		canvas.add(up_container);
		canvas.add(jvm_heap);
		canvas.add(jvm_max);
		
		set();
		
		sp = new JScrollPane();
		sp.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED);
		sp.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
		sp.setViewportView(canvas);
		
		canvas.setAutoscrolls(false);
		
		add(sp);
		
		Center.listen(Events.ControlStatus.class, this, true);
		Center.listen(Events.CrossStatus.class, this, true);
		Center.listen(Events.FileIOStatus.class, this, true);
		Center.listen(Events.StreamIOStatus.class, this, true);
		Center.listen(Events.LogsFound.class, this, true);
		Center.listen(Events.ToolStatus.class, this, true);
		Center.listen(Events.ToolStats.class, this, true);
		Center.listen(Events.RelevantRobotStats.class, this, true);
	}
	
	private void useSize(Dimension s) {
		sp.setBounds(0, 0, s.width, s.height);
		
		Dimension d1, d2, d3;
		int x = 0;
		int y = 3;
		
		d1 = up_container.getPreferredSize();
		up_container.setBounds(x, y, s.width, d1.height);
		
		y += d1.height + 5;
		
		d1 = mode_status.getPreferredSize();
		mode_status.setBounds(x, y, d1.width, d1.height);
		y += d1.height + 5;
		
		d1 = jvm_heap.getPreferredSize();
		jvm_heap.setBounds(x, y, d1.width, d1.height);
		y += d1.height;
		
		d1 = jvm_max.getPreferredSize();
		jvm_max.setBounds(x, y, d1.width, d1.height);
		y += d1.height + 5;
		
		
		d1 = l_found.getPreferredSize();
		l_found.setBounds(x, y, d1.width, d1.height);
		y += d1.height;
		
		d1 = s_found.getPreferredSize();
		s_found.setBounds(x, y, d1.width, d1.height);
		y += d1.height;
		
		d1 = db_found.getPreferredSize();
		db_found.setBounds(x, y, d1.width, d1.height);
		y += d1.height;
		
		d1 = db_cur.getPreferredSize();
		db_cur.setBounds(x, y, d1.width, d1.height);
		y += d1.height;
		
		d1 = db_dropped.getPreferredSize();
		db_dropped.setBounds(x, y, d1.width, d1.height);
		y += d1.height + 5;
		
		d1 = typeArea.getPreferredSize();
		typeArea.setBounds(x, y, d1.width, d1.height);
		y += d1.height + 10;
		
		d1 = botStatA.getPreferredSize();
		botStatA.setBounds(x, y, d1.width, d1.height);
		y += d1.height;

		
		canvas.setPreferredSize(
				new Dimension(d1.width > 250 ? d1.width : 250, y));
	}
	
	private void setJVM_labels() {
		long used = ManagementFactory.getMemoryMXBean().getHeapMemoryUsage().getUsed();
	    long max = ManagementFactory.getMemoryMXBean().getHeapMemoryUsage().getMax();
	    
	    //jvm_heap.setText(String.format("JVM heap using  ~%d bytes.", used));
	    //jvm_max.setText(String.format("JVM heap max is ~%d bytes.", max));
	    jvm_heap.setText(String.format("JVM heap using  ~%s.",
	    		Utility.byteString(used, true, true, true, false)));
	    jvm_max.setText(String.format("JVM heap max  ~%s.",
	    		Utility.byteString(max, true, true, true, false)));
	    //Logger.println("" + max);
	}
	
	private void set() {
		setJVM_labels();
		
		l_found.setText("# logs found: " + ToolStats.INST.l_found);
		s_found.setText("# sessions: " + SessionMaster.INST.sessions.size());
		db_found.setText("Total bytes of data found: " + 
				Utility.byteString(ToolStats.INST.db_found, true, true, true, true));
		db_cur.setText("Current data bytes retained: " + ToolStats.INST.db_cur);
		db_dropped.setText("Total released data: " + ToolStats.INST.db_dropped);
		
		String ts = "Log types found: ";
		for (String t : types) {
			ts += t + " ";
		}
		
		typeArea.setText(ts);
	}
	
	public static JPanel lineAxisContainer() {
		JPanel ret = new JPanel();
		//ret.setLayout(new GridLayout(1, 0));
		
		FlowLayout fl = new FlowLayout(FlowLayout.LEFT);
		ret.setLayout(fl);
		
		return ret;
	}
	
	private JPanel canvas;
	private JScrollPane sp;
	
	private JLabel serv, cnc, cpp, fst;
	private JPanel up_container;
	
	private JLabel mode_status;
	
	private JLabel jvm_heap, jvm_max;
	private JLabel l_found, s_found, db_found;
	private JLabel db_cur, db_dropped;
	
	private HashSet<String> types;
	private JTextArea typeArea;
	
	private JTextArea botStatA;

	@Override
	public void relRobotStats(Object source, RobotStats bs) {
		botStatA.setText(bs.toString());
		useSize(this.getSize());
	}

	@Override
	public void toolStatus(Object source, STATUS s, String desc) {
			
		mode_status.setText(
				String.format("[Status] %s: %s", 
						NBConstants.STATUS_STRINGS[s.index],
						desc));
		
		set();
		useSize(this.getSize());
	}

	@Override
	public void toolStats(Object source, ToolStats s) {
		set();
		useSize(this.getSize());
	}

	@Override
	public void logsFound(Object source, Log... found) {
		for (Object o : found) {
			Log l = (Log) o;
			types.add(l.primaryType());
			
			set();
		}
		useSize(this.getSize());
	}

	@Override
	public void streamStatus(StreamInstance inst, boolean up) {		
		Color c = up ? Color.GREEN : Color.RED;
		serv.setForeground(c);
		useSize(this.getSize());
	}

	@Override
	public void fileioStatus(FileInstance fi, boolean up) {
		Color c = up ? Color.GREEN : Color.RED;
		fst.setForeground(c);
		useSize(this.getSize());
	}

	@Override
	public void nbCrossFound(CrossInstance inst, boolean up) {		
		Color c = up ? Color.GREEN : Color.RED;
		cpp.setForeground(c);
		useSize(this.getSize());
	}

	@Override
	public void controlStatus(ControlInstance inst, boolean up) {
		Color c = up ? Color.GREEN : Color.RED;
		cnc.setForeground(c);
		useSize(this.getSize());
	}
}
