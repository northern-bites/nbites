package nbtool.gui;

import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentListener;
import java.io.File;
import java.util.ArrayList;
import java.util.Hashtable;
import java.util.LinkedList;
import java.util.Vector;

import javax.swing.BoxLayout;
import javax.swing.DefaultComboBoxModel;
import javax.swing.JFileChooser;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import nbtool.data.Log;
import nbtool.data.RobotStats;
import nbtool.data.RobotStats.Flag;
import nbtool.data.Session;
import nbtool.data.SessionMaster;
import nbtool.io.ControlIO;
import nbtool.io.ControlIO.ControlInstance;
import nbtool.io.FileIO;
import nbtool.util.Center;
import nbtool.util.Events;
import nbtool.util.Logger;
import nbtool.util.NBConstants.STATUS;
import nbtool.util.Prefs;
import nbtool.util.Utility;

public class ControlPanel extends JPanel implements Events.LogsFound, Events.LogSelected,
	Events.SessionSelected, Events.ToolStatus, Events.ControlStatus, Events.RelevantRobotStats {
	
	private Vector<String> updateList(LinkedList<String> pref, String nv) {
		if (nv != null) {
			String put = nv.trim();
			if (pref.contains(put)) {
				pref.remove(put);
			}
			
			pref.addFirst(put);
		}
		
		return new Vector<String>(pref);
	}

	public ControlPanel() {
		super();
		initComponents();

		loadButton.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				loadButtonActionPerformed(evt);
			}
		});

		chooseButton.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				chooseButtonActionPerformed(evt);
			}
		});

		clearButton.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				clearButtonActionPerformed(evt);
			}
		});

		connectButton.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				connectButtonActionPerformed(evt);
			}
		});

		controlTestButton.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				testButtonActionPerformed(evt);
			}
		});

		controlExitButton.addActionListener(new java.awt.event.ActionListener() {
			public void actionPerformed(java.awt.event.ActionEvent evt) {
				exitButtonActionPerformed(evt);
			}
		});
		
		writeBox.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				writeBoxActionPerformed();
			}
		});
		writeSlider.setEnabled(false);

		this.controlStatus(null, false);

		dirBox.setModel(new DefaultComboBoxModel<String>( updateList(Prefs.filepaths, null)));
		addrBox.setModel(new DefaultComboBoxModel<String>( updateList(Prefs.addresses, null)));

		Hashtable<Integer, JLabel> labelTable = new Hashtable<>();
		labelTable.put( new Integer( 0 ), new JLabel("0.0") );
		labelTable.put( new Integer( 1 ), new JLabel("0.01") );
		labelTable.put( new Integer( 2 ), new JLabel("0.1") );
		labelTable.put( new Integer( 3 ), new JLabel("0.2") );
		labelTable.put( new Integer( 4 ), new JLabel("0.5") );
		labelTable.put( new Integer( 5 ), new JLabel("1.0") );

		writeSlider.setSnapToTicks(true);
		writeSlider.setMinimum(0);
		writeSlider.setMaximum(5);
		writeSlider.setLabelTable( labelTable );
		writeSlider.setPaintLabels(true);
		writeSlider.setPaintTicks(true);
		writeSlider.setValue(5);

		writeSlider.addChangeListener(new ChangeListener() {
			@Override
			public void stateChanged(ChangeEvent e) {
				if (writeSlider.getValueIsAdjusting())
					return;

				int val = writeSlider.getValue();
				switch(val) {
				case 0: SessionMaster.saveMod = 0; break;
				case 1: SessionMaster.saveMod = 100; break;
				case 2: SessionMaster.saveMod = 10; break;
				case 3: SessionMaster.saveMod = 5; break;
				case 4: SessionMaster.saveMod = 2; break;
				case 5: SessionMaster.saveMod = 1; break;
				default:
					Logger.log(Logger.ERROR, "bad slider value in ControlPanel! " + val);
				}
			}
		});

		keepSlider.setSnapToTicks(true);
		keepSlider.setMinimum(0);
		keepSlider.setMaximum(5);
		keepSlider.setLabelTable( labelTable );
		keepSlider.setPaintLabels(true);
		keepSlider.setPaintTicks(true);
		keepSlider.setValue(5);

		keepSlider.addChangeListener(new ChangeListener() {
			@Override
			public void stateChanged(ChangeEvent e) {
				if (keepSlider.getValueIsAdjusting())
					return;

				int val = keepSlider.getValue();
				switch(val) {
				case 0: SessionMaster.keepMod = 0; break;
				case 1: SessionMaster.keepMod = 100; break;
				case 2: SessionMaster.keepMod = 10; break;
				case 3: SessionMaster.keepMod = 5; break;
				case 4: SessionMaster.keepMod = 2; break;
				case 5: SessionMaster.keepMod = 1; break;
				default:
					Logger.log(Logger.ERROR, "bad slider value in ControlPanel! " + val);
				}
			}
		});

		/* dirBox prevents hotkeys.  Try to keep it from getting default focus */
		this.addComponentListener(new ComponentAdapter(){
			@Override
			public void componentShown(ComponentEvent e) {
				//Logger.println("SHOWN");
				loadButton.requestFocus();
			}

		});

		//Events.LogsFound, Events.LogSelected, Events.SessionSelected,
		//Events.ToolStatus, Events.ControlStatus, Events.RelevantRobotStats
		Center.listen(Events.LogsFound.class, this, true);
		Center.listen(Events.LogSelected.class, this, true);
		Center.listen(Events.SessionSelected.class, this, true);
		Center.listen(Events.ToolStatus.class, this, true);
		Center.listen(Events.ControlStatus.class, this, true);
		Center.listen(Events.RelevantRobotStats.class, this, true);
	}
	
	private void writeBoxActionPerformed() {
		boolean set = writeBox.isSelected();
		if (set) {
			writeSlider.setEnabled(true);
			if (SessionMaster.get().isIdle()) return;
			
			//Not idle, try to latestart
			String dpath = (String) dirBox.getSelectedItem();
			if (dpath == null || dpath.trim().isEmpty()) {
				Logger.warnf("Cannot late start FileInstance with path: %s", dpath);
				return;
			}
			
			String absolute = Utility.localizePath(dpath.trim()) + File.separator;
			if (FileIO.checkLogFolder(absolute)) {
				Logger.warnf("ControlPanel trying to late start a FileInstance...");
				SessionMaster.get().lateStartFileWriting(absolute);
			} else {
				Logger.warnf("Cannot late start FileInstance with path: %s", absolute);
				return;
			}
		} else {
			writeSlider.setEnabled(false);
			if (!SessionMaster.get().isIdle()) {
				Logger.warnf("ControlPanel trying to early stop a FileInstance...");
				SessionMaster.get().earlyStopFileWriting();
			}
		}
	}

	private void loadButtonActionPerformed(java.awt.event.ActionEvent evt) {    		
		if (SessionMaster.get().isIdle()) {
			String dpath = (String) dirBox.getSelectedItem();

			if (dpath == null || dpath.trim().isEmpty()) {
				JOptionPane.showMessageDialog(this, String.format("empty path"));
				return;
			}

			String absolute = Utility.localizePath(dpath.trim()) + File.separator;

			if (FileIO.checkLogFolder(absolute)) {
				Vector<String> pathes = updateList(Prefs.filepaths, dpath);
				dirBox.setModel(new DefaultComboBoxModel<String>(pathes));
				dirBox.setSelectedIndex(pathes.indexOf(dpath));
				SessionMaster.get().loadSession(absolute);
			} else {
				JOptionPane.showMessageDialog(this, String.format("bad path: {%s}", absolute));
			}

		} else {
			JOptionPane.showMessageDialog(this, "cannot load logs while not idle");
		}
	}       

	private void chooseButtonActionPerformed(java.awt.event.ActionEvent evt) {                                           
		int ret = FileIO.dirChooser.showOpenDialog(this);
		if (ret == JFileChooser.APPROVE_OPTION) {
			dirBox.setSelectedItem( FileIO.dirChooser.getSelectedFile().getAbsolutePath());
		}
	}       

	private void clearButtonActionPerformed(java.awt.event.ActionEvent evt) {                                           
		dirBox.setSelectedItem("");
	}       

	private void connectButtonActionPerformed(java.awt.event.ActionEvent evt) {                                           
		if (connectButton.getText().equals("connect")) {
			Logger.log(Logger.INFO, "ControlPanel: connect action.");

			String filepath = null;
			String address = ((String) addrBox.getSelectedItem()).trim();

			if (address == null || address.isEmpty()) {
				Logger.log(Logger.INFO, "ControlPanel: cannot use address: " + address);
				JOptionPane.showMessageDialog(this, String.format("bad address: %s", address));
				return;
			} else {
				Vector<String> addrs =  updateList(Prefs.addresses, address);
				addrBox.setModel(new DefaultComboBoxModel<String>(addrs));
				addrBox.setSelectedIndex(addrs.indexOf(address));
				Logger.log(Logger.INFO, "ControlPanel: using address " + address);
			}

			String dpath = (String) dirBox.getSelectedItem();
			if (!writeBox.isSelected() || dpath == null || dpath.trim().isEmpty()) {
				Logger.log(Logger.INFO, "ControlPanel: not using file writer.");
			} else {
				String absolute = Utility.localizePath(dpath.trim()) + File.separator;
				
				if (!FileIO.checkLogFolder(absolute)) {
					
					int ret = JOptionPane.showConfirmDialog(this,
							"would you like to create it?",
							String.format("bad path {%s}", absolute),
							JOptionPane.YES_NO_OPTION);
					
					if (ret == JOptionPane.YES_OPTION) {
						File pathCreator = new File(absolute);
						try {
							pathCreator.mkdirs();
							pathCreator.setReadable(true);
							pathCreator.setWritable(true);
						} catch (Exception e) {
							Logger.logf(Logger.ERROR, "could not make full path: %s", absolute);
							return;
						}
						
						if (!FileIO.checkLogFolder(absolute)) {
							Logger.logf(Logger.ERROR, "could not verify path: %s", absolute);
							return;
						}
						
					} else {
						return;
					}
				}
				
				assert(FileIO.checkLogFolder(absolute));
				Vector<String> pathes = updateList(Prefs.filepaths, dpath);
				dirBox.setModel(new DefaultComboBoxModel<String>(pathes));
				dirBox.setSelectedIndex(pathes.indexOf(dpath));
				Logger.log(Logger.INFO, "ControlPanel: using directory " + absolute);
				filepath = absolute;
			}

			SessionMaster.get().streamSession(address, filepath);

		} else {
			Logger.log(Logger.INFO, "ControlPanel: stop action.");
			if (!SessionMaster.get().isIdle()) {
				SessionMaster.get().stopWorkingSession();
			}
		}
	}       

	private void testButtonActionPerformed(java.awt.event.ActionEvent evt) {                                           
		ControlInstance inst = ControlIO.getByIndex(0);
		if (inst == null)
			return;

		Logger.log(Logger.INFO, "ControlPane: sending test cmnd");
		inst.tryAddCmnd(ControlIO.createCmndTest());
	}       

	private void exitButtonActionPerformed(java.awt.event.ActionEvent evt) {                                           
		ControlInstance inst = ControlIO.getByIndex(0);
		if (inst == null)
			return;

		Logger.log(Logger.INFO, "ControlPane: sending exit cmnd");
		inst.tryAddCmnd(ControlIO.createCmndExit());
	}        

	/*INTERFACES*/

	private FlagPanel[] flags = null;

	private boolean controlling = false;
	@Override
	public void relRobotStats(Object source, RobotStats bs) {
		if (!controlling)
			return;

		flags = new FlagPanel[bs.flags.size() - 2];
		for (int i = 0; i < flags.length; ++i) {
			Flag f = bs.flags.get(i + 2);	//Skip two for the connected flags.

			flags[i] = new FlagPanel();
			flags[i].setInfo(f.name, f.index);
			flags[i].setKnown(f.value);
		}

		JPanel container = new JPanel();
		container.setLayout(new GridLayout(flags.length, 1));
		for (FlagPanel fp : flags) {
			container.add(fp);
		}

		container.setMinimumSize(container.getPreferredSize());

		flagScrollPanel.setViewportView(container);
	}

	@Override
	public void controlStatus(ControlInstance inst, boolean up) {
		if(up) {
			controlling = true;
			controlTestButton.setEnabled(true);
			controlExitButton.setEnabled(true);
		} else {
			controlling = false;
			controlTestButton.setEnabled(false);
			controlExitButton.setEnabled(false);

			flagScrollPanel.setViewportView(new JLabel("no control instance connected"));
		}
	}

	@Override
	public void toolStatus(Object source, STATUS s, String desc) {
		switch (s) {
		case IDLE:
			connectButton.setEnabled(true);
			connectButton.setText("connect");
			loadButton.setEnabled(true);
			break;
		case RUNNING:
			connectButton.setEnabled(true);
			connectButton.setText("stop");
			loadButton.setEnabled(false);
			break;
		case STARTING:
			connectButton.setEnabled(false);
			connectButton.setText("stop");
			loadButton.setEnabled(false);
			break;
		case STOPPING:
			connectButton.setEnabled(false);
			connectButton.setText("stop");
			loadButton.setEnabled(false);
			break;
		default:
			break;

		}
	}

	@Override
	public void sessionSelected(Object source, Session s) {
//		if (source != this)
//			streamCB.setSelected(false);
	}

	@Override
	public void logSelected(Object source, Log first,
			ArrayList<Log> alsoSelected) {
		/*
		if (source != this)
			streamCB.setSelected(false); */
	}

	@Override
	public void logsFound(Object source, Log... found) {
		/*
		if (streamCB.isSelected()) {
			Log streamLog = null;

			for (Log l : found) {
				if (l.description().contains(streamField.getText()) &&
						l.source == Log.SOURCE.NETWORK) {
					streamLog = l;
					break;
				}
			}
			if (streamLog != null)
				Events.GLogSelected.generate(this, streamLog, new ArrayList<Log>());
		} */
	}

	/*GENERATED CODE*/
	
	private void initComponents() {

        jScrollPane1 = new javax.swing.JScrollPane();
        jTextArea1 = new javax.swing.JTextArea();
        jLabel3 = new javax.swing.JLabel();
        dirpanel = new javax.swing.JPanel();
        dirBox = new javax.swing.JComboBox<>();
        loadButton = new javax.swing.JButton();
        chooseButton = new javax.swing.JButton();
        clearButton = new javax.swing.JButton();
        robotPanel = new javax.swing.JPanel();
        addrBox = new javax.swing.JComboBox<>();
        connectButton = new javax.swing.JButton();
        writeSlider = new javax.swing.JSlider();
        keepSlider = new javax.swing.JSlider();
        jLabel1 = new javax.swing.JLabel();
        jLabel2 = new javax.swing.JLabel();
        writeBox = new javax.swing.JCheckBox();
        controlPanel = new javax.swing.JPanel();
        controlTestButton = new javax.swing.JButton();
        controlExitButton = new javax.swing.JButton();
        flagScrollPanel = new javax.swing.JScrollPane();

        jTextArea1.setColumns(20);
        jTextArea1.setRows(5);
        jScrollPane1.setViewportView(jTextArea1);

        jLabel3.setText("jLabel3");

        dirpanel.setBorder(javax.swing.BorderFactory.createTitledBorder(javax.swing.BorderFactory.createBevelBorder(javax.swing.border.BevelBorder.RAISED), "directory"));

        dirBox.setEditable(true);

        loadButton.setText("load");

        chooseButton.setText("choose");

        clearButton.setText("clear");

        javax.swing.GroupLayout dirpanelLayout = new javax.swing.GroupLayout(dirpanel);
        dirpanel.setLayout(dirpanelLayout);
        dirpanelLayout.setHorizontalGroup(
            dirpanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(dirpanelLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(dirpanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(dirBox, 0, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addGroup(dirpanelLayout.createSequentialGroup()
                        .addComponent(loadButton)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                        .addComponent(chooseButton)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                        .addComponent(clearButton)))
                .addContainerGap())
        );
        dirpanelLayout.setVerticalGroup(
            dirpanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(dirpanelLayout.createSequentialGroup()
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addComponent(dirBox, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(dirpanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(loadButton)
                    .addComponent(chooseButton)
                    .addComponent(clearButton)))
        );

        robotPanel.setBorder(javax.swing.BorderFactory.createTitledBorder(javax.swing.BorderFactory.createBevelBorder(javax.swing.border.BevelBorder.RAISED), "robot"));

        addrBox.setEditable(true);

        connectButton.setText("connect");

        writeSlider.setMaximum(5);
        writeSlider.setPaintLabels(true);
        writeSlider.setPaintTicks(true);
        writeSlider.setSnapToTicks(true);

        keepSlider.setMaximum(5);
        keepSlider.setPaintLabels(true);
        keepSlider.setPaintTicks(true);
        keepSlider.setSnapToTicks(true);

        jLabel1.setText("write fraction");

        jLabel2.setText("keep fraction");

        writeBox.setText("save streamed logs to disk");

        javax.swing.GroupLayout robotPanelLayout = new javax.swing.GroupLayout(robotPanel);
        robotPanel.setLayout(robotPanelLayout);
        robotPanelLayout.setHorizontalGroup(
            robotPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(robotPanelLayout.createSequentialGroup()
                .addGroup(robotPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(robotPanelLayout.createSequentialGroup()
                        .addComponent(connectButton)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(addrBox, 0, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                    .addGroup(robotPanelLayout.createSequentialGroup()
                        .addContainerGap()
                        .addGroup(robotPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                            .addComponent(jLabel1, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                            .addComponent(jLabel2, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addGroup(robotPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(keepSlider, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                            .addComponent(writeSlider, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                        .addGap(0, 0, Short.MAX_VALUE)))
                .addContainerGap())
            .addGroup(robotPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(writeBox)
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );
        robotPanelLayout.setVerticalGroup(
            robotPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(robotPanelLayout.createSequentialGroup()
                .addGroup(robotPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(addrBox, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(connectButton))
                .addGap(0, 0, 0)
                .addComponent(writeBox)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(robotPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                    .addComponent(writeSlider, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(jLabel1, javax.swing.GroupLayout.PREFERRED_SIZE, 38, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(robotPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(keepSlider, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(jLabel2, javax.swing.GroupLayout.PREFERRED_SIZE, 27, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        controlPanel.setBorder(javax.swing.BorderFactory.createTitledBorder(javax.swing.BorderFactory.createBevelBorder(javax.swing.border.BevelBorder.RAISED), "control"));

        controlTestButton.setText("test");

        controlExitButton.setText("exit");

        javax.swing.GroupLayout controlPanelLayout = new javax.swing.GroupLayout(controlPanel);
        controlPanel.setLayout(controlPanelLayout);
        controlPanelLayout.setHorizontalGroup(
            controlPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(controlPanelLayout.createSequentialGroup()
                .addComponent(controlTestButton)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(controlExitButton)
                .addGap(0, 0, Short.MAX_VALUE))
            .addGroup(controlPanelLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(flagScrollPanel)
                .addContainerGap())
        );
        controlPanelLayout.setVerticalGroup(
            controlPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(controlPanelLayout.createSequentialGroup()
                .addGroup(controlPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(controlTestButton)
                    .addComponent(controlExitButton))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(flagScrollPanel, javax.swing.GroupLayout.DEFAULT_SIZE, 165, Short.MAX_VALUE)
                .addContainerGap())
        );

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                    .addComponent(controlPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(dirpanel, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(robotPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)))
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(dirpanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(robotPanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(controlPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addContainerGap())
        );
    }// </editor-fold>                        
                              

    // Variables declaration - do not modify                     
    private javax.swing.JComboBox<String> addrBox;
    private javax.swing.JButton chooseButton;
    private javax.swing.JButton clearButton;
    private javax.swing.JButton connectButton;
    private javax.swing.JButton controlExitButton;
    private javax.swing.JPanel controlPanel;
    private javax.swing.JButton controlTestButton;
    private javax.swing.JComboBox<String> dirBox;
    private javax.swing.JPanel dirpanel;
    private javax.swing.JScrollPane flagScrollPanel;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JLabel jLabel2;
    private javax.swing.JLabel jLabel3;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JTextArea jTextArea1;
    private javax.swing.JSlider keepSlider;
    private javax.swing.JButton loadButton;
    private javax.swing.JPanel robotPanel;
    private javax.swing.JCheckBox writeBox;
    private javax.swing.JSlider writeSlider;
    // End of variables declaration                               
}

