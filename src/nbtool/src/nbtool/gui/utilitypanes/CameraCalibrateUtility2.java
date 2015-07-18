package nbtool.gui.utilitypanes;

import java.awt.Container;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.JPanel;

import nbtool.data.Log;
import nbtool.data.SExpr;
import nbtool.data.Session;
import nbtool.data.SessionMaster;
import nbtool.io.CrossIO;
import nbtool.io.CrossIO.CrossCall;
import nbtool.io.CrossIO.CrossFunc;
import nbtool.io.CrossIO.CrossInstance;
import nbtool.io.FileIO;
import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.io.CommonIO.IOInstance;
import nbtool.util.Center;
import nbtool.util.Events;
import nbtool.util.Logger;

public class CameraCalibrateUtility2 extends UtilityParent {
	
	private CCU_Frame display = null;

    @Override
    public JFrame supplyDisplay() {
        if (display == null) {
            return (display = new CCU_Frame());
        }
        return display;
    }

    @Override
    public String purpose() {
        return "Calibrate camera from logs of star mat";
    }

	@Override
	public char preferredMemnonic() {
		return 'c';
	}
	
	public class CCU_Frame extends javax.swing.JFrame implements IOFirstResponder, ActionListener, Events.SessionSelected {
		
		class Params {
			double rollOffset, tiltOffset;
			String camera;
			String robotName;
			
			Params(String c, String r) {
				camera = c;
				robotName = r;
				rollOffset = Double.NaN;
				tiltOffset = Double.NaN;
			}
		}
		
		Params lastCalculated;
		Session using;
		
	    public CCU_Frame() {
	        initComponents();
	        cameraBox.setSelectedIndex(0);
	        writeButton.setEnabled(false);
	        
	        writeButton.addActionListener(this);
	        sendButton.addActionListener(this);
	        callButton.addActionListener(this);
	        
	        using = SessionMaster.get().getLatestSession();
	        if (using != null)
	        	updateSessionLabels();
	        
	        lastCalculated = null;
	        Center.listen(Events.SessionSelected.class, this, true);
	    }
	    
	    @Override
		public void sessionSelected(Object source, Session s) {
			using = s;
			updateSessionLabels();
		}

		@Override
		public void actionPerformed(ActionEvent e) {
			if (e.getSource() == writeButton) {
				if (lastCalculated != null && lastCalculated.rollOffset != Double.NaN) {
					Logger.println("saving parameters...");
					String filePath = System.getenv().get("NBITES_DIR");
		            filePath += "/src/man/config/calibrationParams.txt";

		            String lisp = "(" + lastCalculated.camera + " ";
		            lisp += Double.toString(lastCalculated.rollOffset)
		            		+ " " + Double.toString(lastCalculated.tiltOffset) + ")";
		            
		            String text = "";
		            try {
		                FileReader fr = new FileReader(filePath);
		                BufferedReader bf = new BufferedReader(fr);
		                String line;
		                while ((line = bf.readLine()) != null) {
		                    text += line;
		                }
		                SExpr saved = SExpr.deserializeFrom(text);
		                SExpr bot = saved.get(1).find(lastCalculated.robotName);
		                if (!bot.exists()) {
		                    System.out.printf("Invalid robot name! Could not find exiting params for \"%s\"\n",
		                    		lastCalculated.robotName);
		                } else {
		                    if (lastCalculated.camera.equals("TOP")) {
		                        bot.setList(bot.get(0), SExpr.deserializeFrom(lisp), bot.get(2));
		                    } else {
		                        bot.setList(bot.get(0), bot.get(1), SExpr.deserializeFrom(lisp));
		                    }
		                }

		                // Write out to file
		                FileOutputStream fos = new FileOutputStream(filePath, false);
		                byte[] data = saved.print().getBytes();
		                fos.write(data);

		                fr.close();
		                bf.close();
		                fos.close();

		            } catch (FileNotFoundException e1) {
		                e1.printStackTrace();
		            } catch (IOException e1) {
		                e1.printStackTrace();
		            }
				} else {
					JOptionPane.showMessageDialog(this, "successfully call calibrate first");
				}
			} else if (e.getSource() == callButton) {
				if (using == null) {
					JOptionPane.showMessageDialog(this, "no selected session");
					return;
				}
				List<Log> accepted = new LinkedList<Log>();
				String cameraString = (String) cameraBox.getSelectedItem();
				String searchText = String.format("(from camera_%s)", cameraString);
				
				for (Log log : using.logs_ALL) {
					if (log.description().indexOf(searchText) >= 0) {
						accepted.add(log);
					}
				}
				
				if (accepted.size() < 7) {
					JOptionPane.showMessageDialog(this, "not enough logs from " + cameraString);
					return;
				}
				
				for (Log log : accepted) {
					if (log.bytes == null) {
						try {
							FileIO.loadLog(log, log.parent.directoryFrom);
						} catch (IOException e1) {
							e1.printStackTrace();
							return;
						}
					}
				}
				
				// Fetch name of robot
                SExpr name = accepted.get(0).tree().find("from_address");
                if (!name.exists()) {
                    System.out.printf("COULD NOT LOAD ROBOT NAME. ABORTING.\n");
                } else {
                    String rname = name.get(1).value();
                    int iloc = rname.indexOf(".local");
                 
                    if (iloc > 0) {
                    	rname = rname.substring(0, iloc);
                    }

                    if (rname.equals("she-hulk"))
                    	rname = "shehulk";
                    
                    lastCalculated = new Params(cameraString, rname);

                    // Call calibrate nbfunc with the 7 logs
                    Logger.printf("calibrating for camera{%s} robot{%s} session{%s}",
                    		cameraString, rname, using.name);
                    CrossInstance ci = CrossIO.instanceByIndex(0);
                    if (ci == null) {
                    	JOptionPane.showMessageDialog(this, "no nbcross instance connected");
                    	return;
                    }
                    
                    CrossFunc func = ci.functionWithName("CameraCalibration");
                    if (func == null) {
                    	Logger.errorf("COULD NOT GET CameraCalibration FUNCTION");
                    	return;
                    }
                    
                    Log[] seven = accepted.subList(0, 7).toArray(new Log[0]);
                    CrossCall call = new CrossCall(this, func, seven);
                    assert(ci.tryAddCall(call));
                }
				
			} else {
				Logger.errorf("UNKNOWN ACTION SOURCE IN CCU");
			}
		}

		@Override
		public void ioFinished(IOInstance instance) {}

		@Override
		public void ioReceived(IOInstance inst, int ret, Log... out) {
			System.out.println("CamCal ioReceived");
			assert(lastCalculated != null);
			
            SExpr atom = out[0].tree().find("roll");
            if (atom.exists()) {
            	lastCalculated.rollOffset = atom.get(1).valueAsDouble();
            } else {
                System.out.printf("IORecieved without roll offset!\n");
            }

            atom = out[0].tree().find("tilt");
            if (atom.exists()) {
            	lastCalculated.tiltOffset = atom.get(1).valueAsDouble();
            } else {
                System.out.printf("IORecieved without tilt offset!\n");
            }

            if (out[0].tree().find("failure").exists()) {
                JOptionPane.showMessageDialog(this, "Log calibreation failure! Take better logs.\n");
                return;
            } 
            
            calcLabel.setText("Calculated calibration offsets:");
            cameraLabel.setText("For camera: " + lastCalculated.camera);
            rollLabel.setText("Roll offset: " + Double.toString(lastCalculated.rollOffset) + " rad.");
            tiltLabel.setText("Tilt offset: " + Double.toString(lastCalculated.tiltOffset) + " rad.");
            robotLabel.setText("Of robot: " + lastCalculated.robotName);
            writeButton.setEnabled(true);
            sendButton.setEnabled(true);
		}

		@Override
		public boolean ioMayRespondOnCenterThread(IOInstance inst) {
			return false;
		}
		
		private void updateSessionLabels() {
			sessionNameLabel.setText("Session name: " + using.name);
			sessionSizeLabel.setText("Session size: " + using.logs_ALL.size());
		}

	    /**
	     * This method is called from within the constructor to initialize the form.
	     * WARNING: Do NOT modify this code. The content of this method is always
	     * regenerated by the Form Editor.
	     */
	    @SuppressWarnings("unchecked")
	    // <editor-fold defaultstate="collapsed" desc="Generated Code">                          
	    private void initComponents() {
	        cameraBox = new javax.swing.JComboBox<>();
	        callButton = new javax.swing.JButton();
	        sessionNameLabel = new javax.swing.JLabel();
	        sessionSizeLabel = new javax.swing.JLabel();
	        calcLabel = new javax.swing.JLabel();
	        rollLabel = new javax.swing.JLabel();
	        tiltLabel = new javax.swing.JLabel();
	        cameraLabel = new javax.swing.JLabel();
	        robotLabel = new javax.swing.JLabel();
	        
	        writeButton = new JButton();
	        sendButton = new JButton();
	        
	        saveContainer = new javax.swing.JPanel();
	        saveContainer.setLayout(new GridLayout(1,2));
	        saveContainer.add( writeButton );
	        saveContainer.add( sendButton );

	        setTitle("Cheddar's Cool Calibration Utility");
	        setMinimumSize(new java.awt.Dimension(485, 242));

	        cameraBox.setModel(new javax.swing.DefaultComboBoxModel<String>(new String[] { "TOP", "BOT" }));

	        callButton.setText("get calibration params");

	        sessionNameLabel.setText("Session name:");

	        sessionSizeLabel.setText("Session size:");

	        writeButton.setText("SAVE");
	        sendButton.setText("SEND");

	        calcLabel.setText("");

	        rollLabel.setText("");

	        tiltLabel.setText("");

	        cameraLabel.setText("");

	        robotLabel.setText("");

	        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
	        getContentPane().setLayout(layout);
	        layout.setHorizontalGroup(
	            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
	            .addGroup(layout.createSequentialGroup()
	                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
	                    .addGroup(layout.createSequentialGroup()
	                        .addContainerGap()
	                        .addComponent(callButton, javax.swing.GroupLayout.PREFERRED_SIZE, 218, javax.swing.GroupLayout.PREFERRED_SIZE)
	                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
	                        .addComponent(cameraBox, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
	                        .addGap(0, 168, Short.MAX_VALUE))
	                    .addGroup(layout.createSequentialGroup()
	                        .addGap(12, 12, 12)
	                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
	                            .addComponent(calcLabel, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
	                            .addComponent(sessionNameLabel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
	                            .addComponent(sessionSizeLabel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)))
	                    .addGroup(layout.createSequentialGroup()
	                        .addContainerGap()
	                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
	                            .addComponent(rollLabel, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
	                            .addComponent(robotLabel, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
	                            .addComponent(cameraLabel, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
	                            .addGroup(layout.createSequentialGroup()
	                                .addComponent(saveContainer, javax.swing.GroupLayout.PREFERRED_SIZE, 100, javax.swing.GroupLayout.PREFERRED_SIZE)
	                                .addGap(0, 0, Short.MAX_VALUE))
	                            .addComponent(tiltLabel, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))))
	                .addContainerGap())
	        );
	        layout.setVerticalGroup(
	            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
	            .addGroup(layout.createSequentialGroup()
	                .addContainerGap()
	                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
	                    .addComponent(callButton)
	                    .addComponent(cameraBox, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
	                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
	                .addComponent(sessionNameLabel)
	                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
	                .addComponent(sessionSizeLabel)
	                .addGap(18, 18, 18)
	                .addComponent(calcLabel)
	                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
	                .addComponent(rollLabel)
	                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
	                .addComponent(tiltLabel)
	                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
	                .addComponent(cameraLabel)
	                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
	                .addComponent(robotLabel)
	                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
	                .addComponent(saveContainer)
	                .addContainerGap())
	        );

	        pack();
	    }// </editor-fold>     
	    
	    /*
	    private void initComponents2() {
	    	cameraBox = new javax.swing.JComboBox<>();
	        callButton = new javax.swing.JButton();
	        sessionNameLabel = new javax.swing.JLabel();
	        sessionSizeLabel = new javax.swing.JLabel();
	        saveButton = new javax.swing.JButton();
	        calcLabel = new javax.swing.JLabel();
	        rollLabel = new javax.swing.JLabel();
	        tiltLabel = new javax.swing.JLabel();
	        cameraLabel = new javax.swing.JLabel();
	        robotLabel = new javax.swing.JLabel();

	        setTitle("Cheddar's Cool Calibration Utility");
	        setMinimumSize(new java.awt.Dimension(485, 242));

	        cameraBox.setModel(new javax.swing.DefaultComboBoxModel<String>(new String[] { "TOP", "BOT" }));

	        callButton.setText("get calibration params");

	        sessionNameLabel.setText("Session name:");

	        sessionSizeLabel.setText("Session size:");

	        saveButton.setText("SAVE");

	        calcLabel.setText("");

	        rollLabel.setText("");

	        tiltLabel.setText("");

	        cameraLabel.setText("");

	        robotLabel.setText("");
	        
	        GridLayout grid = new GridLayout(10,1);
	        Container content = this.getContentPane();
	        content.setLayout(grid);
	        content.add(sessionNameLabel);
	        content.add(sessionSizeLabel);
	        
	        content.add(callButton);
	        content.add(cameraBox);
	        
	        content.add(calcLabel);
	        content.add(robotLabel);
	   
	        content.add(cameraLabel);
	        content.add(rollLabel);
	        
	        content.add(tiltLabel);
	        content.add(saveButton);
	    } */

	    // Variables declaration - do not modify                     
	    private javax.swing.JButton callButton;
	    private javax.swing.JComboBox<String> cameraBox;
	    private javax.swing.JLabel cameraLabel;
	    private javax.swing.JLabel calcLabel;
	    private javax.swing.JLabel robotLabel;
	    private javax.swing.JLabel rollLabel;
	    private javax.swing.JLabel sessionNameLabel;
	    private javax.swing.JLabel sessionSizeLabel;
	    private javax.swing.JLabel tiltLabel;
	    
	    private javax.swing.JButton writeButton;
	    private javax.swing.JButton sendButton;
	    private javax.swing.JPanel  saveContainer;
	    // End of variables declaration                   
	}
}
