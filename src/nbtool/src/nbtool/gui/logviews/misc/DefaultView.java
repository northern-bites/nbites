package nbtool.gui.logviews.misc;

import java.awt.Color;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;

import javax.swing.BorderFactory;
import javax.swing.JFileChooser;
import javax.swing.JFrame;

import nbtool.data.Log;
import nbtool.data.SExpr;
import nbtool.io.FileIO;
import nbtool.util.Logger;
import nbtool.util.Utility;

public class DefaultView extends ViewParent implements ActionListener {
		
	public DefaultView() {
		initComponents();
		
		saveButton.addActionListener(this);
		saveButton.setText("save");
		setButton.addActionListener(this);
		setButton.setText("set");
		dataArea.setEditable(false);
		dataArea.setLineWrap(true);
		dataArea.setBorder(BorderFactory.createLineBorder(Color.BLACK));
		
		descArea.setLineWrap(false);
	}
	
	//From netbeans!
	private void initComponents() {

        jScrollPane4 = new javax.swing.JScrollPane();
        descArea = new javax.swing.JTextArea();
        jScrollPane5 = new javax.swing.JScrollPane();
        dataArea = new javax.swing.JTextArea();
        setButton = new javax.swing.JButton();
        id = new javax.swing.JLabel();
        source = new javax.swing.JLabel();
        saveButton = new javax.swing.JButton();
        dataSize = new javax.swing.JLabel();
        descSize = new javax.swing.JLabel();

        descArea.setColumns(20);
        descArea.setRows(5);
        jScrollPane4.setViewportView(descArea);

        dataArea.setColumns(20);
        dataArea.setRows(5);
        jScrollPane5.setViewportView(dataArea);

        setButton.setText("jButton2");

        id.setText("jLabel3");

        source.setText("jLabel4");

        saveButton.setText("jButton1");

        dataSize.setText("jLabel1");

        descSize.setText("jLabel2");

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(jScrollPane5, javax.swing.GroupLayout.DEFAULT_SIZE, 823, Short.MAX_VALUE)
                    .addGroup(layout.createSequentialGroup()
                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                                .addComponent(setButton)
                                .addComponent(saveButton)
                                .addComponent(source, javax.swing.GroupLayout.DEFAULT_SIZE, 137, Short.MAX_VALUE)
                                .addComponent(id, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                            .addComponent(dataSize)
                            .addComponent(descSize))
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                        .addComponent(jScrollPane4)))
                .addContainerGap())
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(jScrollPane4, javax.swing.GroupLayout.PREFERRED_SIZE, 420, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addGroup(layout.createSequentialGroup()
                        .addComponent(setButton)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(saveButton)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(id)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(source)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(dataSize)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(descSize)))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jScrollPane5, javax.swing.GroupLayout.DEFAULT_SIZE, 152, Short.MAX_VALUE)
                .addContainerGap())
        );
    }

	@Override
	public void setLog(Log newlog) {
		assert(newlog.bytes != null);
				
		descArea.setText(log.tree().print());		
		if (log.bytes.length < 1000)
			dataArea.setText(Utility.bytesToHexString(log.bytes));
		else dataArea.setText(Utility.bytesToHexString(Utility.subArray(log.bytes, 0, 1000)) + "...");
		
		if (log.checksum() != null) {
			int recv_checksum = log.checksum();
			int checksum = Utility.checksum(log.bytes);
			
			if (recv_checksum != checksum) {
				/*
				JOptionPane.showMessageDialog(null, "log checksum did not match, expected "
						+ recv_checksum + " but calculated " + checksum + "."); */
				
				Logger.logf(Logger.WARN, "\n\nWARNING: log checksum DID NOT match!\n\texpected %d\n\tgot %d\n",
						recv_checksum, checksum);
			}
		}
		
		id.setText("id: " + log.unique_id);
		source.setText("src: " + log.source);
		dataSize.setText("data b: " + log.bytes.length);
		descSize.setText("desc c: " + log.description().length());
	}

	// Variables declaration - do not modify                     
    private javax.swing.JButton saveButton;
    private javax.swing.JButton setButton;
    private javax.swing.JLabel dataSize;
    private javax.swing.JLabel descSize;
    private javax.swing.JLabel id;
    private javax.swing.JLabel source;
    private javax.swing.JScrollPane jScrollPane4;
    private javax.swing.JScrollPane jScrollPane5;
    private javax.swing.JTextArea descArea;
    private javax.swing.JTextArea dataArea;
    // End of variables declaration
    
    public static void main(String args[]) {
        JFrame frame = new JFrame("tester");
        frame.setSize(400, 400);
        frame.setContentPane(new DefaultView());
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setVisible(true);
    }

	@Override
	public void actionPerformed(ActionEvent e) {
		if (e.getSource() == setButton && log != null) {
			String newdesc = descArea.getText();
			try {
				SExpr s = SExpr.deserializeFrom(newdesc);
				if (s == null || s.count() < 1) {
					Logger.log(Logger.INFO, "Cannot use new description: " + newdesc);
					descArea.setText(log.description());
				} else {
					log.setTree(s);
				}
				
			} catch(Exception ex) {
				Logger.log(Logger.INFO, "Cannot use new description: " + newdesc);
				descArea.setText(log.description());
			}
		} else if (e.getSource() == saveButton) {
			int rVal = FileIO.fileChooser.showSaveDialog(this);
			
			if (rVal == JFileChooser.APPROVE_OPTION) {
				File f = FileIO.fileChooser.getSelectedFile();
				if (f.isDirectory()) {
					Logger.log(Logger.INFO, "Cannot overwrite directory with log.");
					return;
				}
				
				String aPath = f.getAbsolutePath();
				if (!aPath.endsWith(".nblog"))
					aPath = aPath + ".nblog";
				
				Logger.log(Logger.INFO, "Writing log to: " + aPath);
				
				try {
					FileIO.writeLogToPath(log, aPath);
				} catch (IOException e1) {
					e1.printStackTrace();
				}
			}
		}
		
	}
	
	@Override
	public void alsoSelected(ArrayList<Log> also) {
		Logger.log(Logger.INFO, "DefaultView sees also selected:");
		for (Log a: also) {
			Logger.logf(Logger.INFO, "\t%s", a.toString());
		}
	}
}
