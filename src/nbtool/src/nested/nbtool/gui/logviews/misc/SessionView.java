package nbtool.gui.logviews.misc;

import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.io.IOException;
import java.util.LinkedList;

import javax.swing.JButton;
import javax.swing.JFileChooser;
import javax.swing.JPanel;
import javax.swing.JTextField;

import nbtool.data.Session;
import nbtool.data._log._Log;
import nbtool.io.FileIO;
import nbtool.term.NBLOG_pack;
import nbtool.util.Debug;

public class SessionView extends JPanel implements ActionListener {
	
	private Session session = null;
	
	public SessionView(Session s) {
		super();
		
		this.session = s;
		
		f1 = new JTextField("Session:: " + s.name);
		f2 = new JTextField(String.format("showing %d logs of %d total.", 
				s.logs_DO.size(), s.logs_ALL.size()));
		
		setLayout(new GridLayout(4, 1));
		
		add(f1);
		add(f2);
		
		save = new JButton("save session");
		save.addActionListener(this);
		pack = new JButton("pack session");
		pack.addActionListener(this);
		
		add(save);
		add(pack);
	}
	
	private JTextField f1, f2;
	private JButton save, pack;
	
	@Override
	public void actionPerformed(ActionEvent e) {
		
		if (e.getSource() == save) {
			int ret = FileIO.dirChooser.showSaveDialog(this);
			
			if (ret == JFileChooser.APPROVE_OPTION) {
				String path = FileIO.dirChooser.getSelectedFile().getAbsolutePath();
				
				for (_Log l : session.logs_ALL) {
					if (l.name == null)
						l.setNameFromDesc();
					String fname = path + File.separatorChar + l.name ;
					
					if (l.bytes == null) {
						Debug.info( "... loading bytes for save op.");
						try {
							FileIO.loadLog(l, session.directoryFrom);
						} catch (IOException e1) {
							e1.printStackTrace();
						}
					}
					
					try {
						Debug.info( "saving log to " + fname);
						FileIO.writeLogToPath(l, fname);
					} catch (IOException e1) {
						e1.printStackTrace();
					}
				}
			}
			
			return;
		}
		
		if (e.getSource() == pack) {
			
			LinkedList<_Log> accepted = new LinkedList<_Log>();
			
			for (_Log lg : session.logs_ALL) {
				if (lg.primaryType().equals("YUVImage")) {
					accepted.add(lg);
					
					if (lg.bytes == null) {
						Debug.info( "... loading bytes for pack op.");
						try {
							FileIO.loadLog(lg, session.directoryFrom);
						} catch (IOException e1) {
							e1.printStackTrace();
						}
					}
				}
			}
			
			int ret = FileIO.dirChooser.showOpenDialog(this);
			
			if (ret == JFileChooser.APPROVE_OPTION) {
				String path = FileIO.dirChooser.getSelectedFile().getAbsolutePath();
				
				try {
					NBLOG_pack.packTo(path, accepted);
				} catch (IOException e1) {
					e1.printStackTrace();
				}
			}
		
			return;
		}
		
	}
}
