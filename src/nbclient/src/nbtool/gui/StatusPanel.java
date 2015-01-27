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

import nbtool.data.BotStats;
import nbtool.data.Log;
import nbtool.data.SessionMaster;
import nbtool.data.Stats;
import nbtool.util.N;
import nbtool.util.N.EVENT;
import nbtool.util.N.NListener;
import nbtool.util.NBConstants;
import nbtool.util.NBConstants.MODE;
import nbtool.util.NBConstants.STATUS;

public class StatusPanel extends JPanel implements NListener{

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
		
		N.listen(EVENT.CNC_CONNECTION, this);
		N.listen(EVENT.CPP_CONNECTION, this);
		N.listen(EVENT.FIO_THREAD, this);
		N.listen(EVENT.SIO_THREAD, this);
		N.listen(EVENT.LOG_FOUND, this);
		N.listen(EVENT.STATS, this);
		N.listen(EVENT.REL_BOTSTAT, this);
		N.listen(EVENT.STATUS, this);
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
	    
	    jvm_heap.setText(String.format("JVM heap using  ~%d bytes.", used));
	    jvm_max.setText(String.format("JVM heap max is ~%d bytes.", max));
	}
	
	private void set() {
		setJVM_labels();
		
		l_found.setText("# logs found: " + Stats.INST.l_found);
		s_found.setText("# sessions: " + SessionMaster.INST.sessions.size());
		db_found.setText("Total bytes of data found: " + Stats.INST.db_found);
		db_cur.setText("Current data bytes retained: " + Stats.INST.db_cur);
		db_dropped.setText("Total released data: " + Stats.INST.db_dropped);
		
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

	
	public void notified(EVENT e, Object src, Object... args) {
		switch(e) {
		case CNC_CONNECTION:
			Boolean b = (Boolean) args[0];
			
			Color c = b ? Color.GREEN : Color.RED;
			
			cnc.setForeground(c);
			break;
		case CPP_CONNECTION:
			b = (Boolean) args[0];
			
			c = b ? Color.GREEN : Color.RED;
			
			cpp.setForeground(c);
			break;
		case FIO_THREAD:
			b = (Boolean) args[0];
			
			c = b ? Color.GREEN : Color.RED;
			
			fst.setForeground(c);
			break;
		case LOG_FOUND:
			for (Object o : args) {
				Log l = (Log) o;
				types.add(l.type());
				
				set();
			}
			break;
		case REL_BOTSTAT:
			BotStats a = (BotStats) args[0];
			botStatA.setText(a.toString());
			
			break;
		case SIO_THREAD:
			b = (Boolean) args[0];
			
			c = b ? Color.GREEN : Color.RED;
			
			serv.setForeground(c);
			break;
		case STATS:
			
			set();
			
			break;
		case STATUS:
			STATUS s = (STATUS) args[0];
			MODE m = (MODE) args[1];
			
			mode_status.setText(
					String.format("[M/S] %s: %s", 
							NBConstants.mode_strings[m.index],
							NBConstants.status_strings[s.index]));
			
			set();
			
			break;
		default:
			break;
		 
		}
		
		useSize(this.getSize());
	}
}
