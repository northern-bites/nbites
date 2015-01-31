package nbtool.gui.logviews.misc;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.io.File;
import java.io.IOException;
import java.util.Map;
import java.util.jar.Attributes;

import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JFileChooser;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;

import nbtool.data.Log;
import nbtool.io.FileIO;
import nbtool.util.U;

public class DefaultView extends ViewParent implements ActionListener {
	private static final long serialVersionUID = 1L;
	private JLabel size;
	private JTextField desc;
	private JTextArea data;
	private JButton save;
	
	public Log getLog() {return log;}
	public void setLog(Log newlog) {
		assert(newlog.bytes != null);
		this.log = newlog;
		desc.setText(log.description);
		size.setText("BYTES: " +String.valueOf(log.bytes.length));
		
		if (log.bytes.length < 1000)
			data.setText(U.bytesToHexString(log.bytes));
		else data.setText(U.bytesToHexString(U.subArray(log.bytes, 0, 1000)) + "...");
		
		if (log.getAttributes().get("checksum") != null) {
			int recv_checksum = log.checksum();
			int checksum = 0;
			for (byte b : log.bytes)
				checksum += (b & 0xFF); //NEED AND.  Forces java to treat the byte as an UNSIGNED value.
			
			if (recv_checksum != checksum) {
				JOptionPane.showMessageDialog(null, "log checksum did not match, expected "
						+ recv_checksum + " but calculated " + checksum + ".");
			}
		}
		
		this.repaint();
	}
	
	private void useSize(Dimension s) {
		int y_offset = 0;
		Dimension d = desc.getPreferredSize();
		desc.setBounds(0, 0, d.width, d.height);
		y_offset += d.height;
		
		d = size.getPreferredSize();
		size.setBounds(0, y_offset, d.width, d.height);
		Dimension sd = save.getPreferredSize();
		save.setBounds(s.width - sd.width, y_offset, sd.width, sd.height);
		
		y_offset += (sd.height > d.height) ? sd.height : d.height;
		
		data.setBounds(0, y_offset, s.width, (s.height - y_offset) / 2);
	}
	
	public DefaultView() {
		super();
		
		addComponentListener(new ComponentAdapter() {
			public void componentResized(ComponentEvent e) {
				useSize(e.getComponent().getSize());
			}
		});
		setLayout(null);
		
		desc = new JTextField("desc");
		desc.addActionListener(this);
		size = new JLabel("size");
		
		data = new JTextArea("data in hex");
		data.setEditable(false);
		data.setLineWrap(true);
		data.setBorder(BorderFactory.createLineBorder(Color.BLACK));
		
		save = new JButton("save");
		save.addActionListener(this);
		
		add(desc); add(size); add(data); add(save);
	}
	
	public void actionPerformed(ActionEvent e) {
		if (e.getSource() == desc && log != null) {
			String newdesc = desc.getText();
			
			try {
				Map<String, String> a = U.attributes(newdesc);

				if (a == null || a.size() < 2) {
					U.w("Cannot use new description: " + newdesc);
					desc.setText(log.description);
				} else {
					log.dumpAttributes();
					log.description = newdesc;
					log.getAttributes(); 
				} 
			} catch(Exception ex) {
				U.w("Cannot use new description: " + newdesc);
				desc.setText(log.description);
			}
		} else if (e.getSource() == save) {
			int rVal = FileIO.chooser.showSaveDialog(this);
			
			if (rVal == JFileChooser.APPROVE_OPTION) {
				File f = FileIO.chooser.getSelectedFile();
				if (f.isDirectory()) {
					U.w("Cannot overwrite directory with log.");
				}
				
				String aPath = f.getAbsolutePath();
				if (!aPath.endsWith(".nblog"))
					aPath = aPath + ".nblog";
				
				U.w("Writing log to: " + aPath);
				
				try {
					FileIO.writeLogToPath(log, aPath);
				} catch (IOException e1) {
					e1.printStackTrace();
				}
			}
		}
	}	
}
