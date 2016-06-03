//package nbtool.gui.utilitypanes;
//
//import java.awt.event.ActionEvent;
//import java.awt.event.ActionListener;
//import java.io.IOException;
//
//import javax.swing.JFrame;
//import javax.swing.JOptionPane;
//import javax.swing.Timer;
//
//import nbtool.gui.logdnd.LogDND;
//import nbtool.gui.logdnd.LogDND.LogDNDTarget;
//import nbtool.util.Debug;
//
//public class ReplayUtility extends UtilityParent {
//	
//	private RU_Frame display = null;
//
//	@Override
//	public JFrame supplyDisplay() {
//		if (display == null) {
//			return (display = new RU_Frame());
//		}
//		
//		return display;
//	}
//
//	@Override
//	public String purpose() {
//		return "Loop through a set of logs.";
//	}
//
//	@Override
//	public char preferredMemnonic() {
//		// TODO Auto-generated method stub
//		return 'o';
//	}
//	
//	public class RU_Frame extends javax.swing.JFrame {
//		
//		private class Loop {
//			
//			boolean valid = false, repeat = false;
//			int first = -1;
//			int len = -1;
//			
//			int index = -1;
//			
//			Loop(Session s, _Log from, _Log to, boolean repeat) {
//				int fi = s.logs_DO.indexOf(from);
//				int ti = s.logs_DO.indexOf(to);
//				
//				if (!(fi < ti)) {
//					return;
//				}
//				
//				this.session = s;
//				this.valid = true;
//				this.repeat = repeat;
//				this.first = fi;
//				this.len = ti - fi + 1;
//				this.index = 0;
//				
//				Debug.info("Loop(%d, %d)", first, len);
//			}
//			
//			Session session;
//			_Log getNext() {
//				if (index == len && !repeat)
//					return null;
//				
//				int offset = index++ % len;
//				_Log l = session.logs_DO.get(first + offset);
//				if (l.bytes == null) {
//					try {
//						FileIO.loadLog(l, l.parent.directoryFrom);
//					} catch (IOException e) {
//						e.printStackTrace();
//						return null;
//					}
//				}
//				
//				return l;
//			}
//			
//			boolean isValid() {
//				return valid;
//			}
//		}
//		
//		private _Log fromPlaced = null;
//		private _Log toPlaced = null;
//		private Timer timer = null;
//
//	    public RU_Frame() {
//	        initComponents();
//	        
//	        LogDND.makeComponentTarget(logFromLabel, new LogDNDTarget(){
//				@Override
//				public void takeLogsFromDrop(_Log[] log) {
//					if (log.length < 1) return;
//					fromPlaced = log[0];
//					logFromLabel.setText(fromPlaced.description(50));
//				}
//	        });
//	        
//	        LogDND.makeComponentTarget(logToLabel, new LogDNDTarget(){
//				@Override
//				public void takeLogsFromDrop(_Log[] log) {
//					if (log.length < 1) return;
//					toPlaced = log[0];
//					logToLabel.setText(toPlaced.description(50));
//				}
//	        });
//	        
//	        toggle.addActionListener(new ActionListener(){
//
//				@Override
//				public void actionPerformed(ActionEvent e) {
//					toggleAction();
//				}
//	        	
//	        });
//	    }
//	    
//	    private void toggleAction() {
//	    	if (timer != null && timer.isRunning()) {
//	    		timer.stop();
//	    		timer = null;
//	    		toggle.setText("start");
//	    		return;
//	    	}
//	    	
//	    	Debug.info("starting replay...");
//	    	if (fromPlaced == null || toPlaced == null ) {
//	    		JOptionPane.showMessageDialog(this, "set both logs");
//	    		return;
//	    	}
//	    	
//	    	if (fromPlaced.parent != toPlaced.parent) {
//	    		JOptionPane.showMessageDialog(this, "logs must be from same session");
//	    		return;
//	    	}
//	    	
//	    	int di = displayBox.getSelectedIndex();
//	    	if (di < 0) return;
//	    	
//	    	LogDisplayPanel _ldp = null; 
//	    	if (di == 0) {
//	    		_ldp = LogDisplayPanel.main;
//	    	} else {
//	    		_ldp = LogDisplayPanel.extern[di - 1];
//	    	}
//	    	
//	    	if (_ldp == null) {
//	    		JOptionPane.showMessageDialog(this, "bad display");
//	    		return;
//	    	}
//	    	
//	    	final LogDisplayPanel ldp = _ldp;
//
//	        final int delay = (Integer) startDelaySpinner.getValue();
//	        final int itr = (Integer) itrDelaySpinner.getValue();
//	        
//	        final boolean looping = loopBox.isSelected();
//	        final Loop loopo = new Loop(fromPlaced.parent, fromPlaced, toPlaced, looping);
//	       
//	        
//	        if (!loopo.isValid()) {
//	        	JOptionPane.showMessageDialog(this, "bad log order");
//	    		return;
//	        }
//	        
//	        timer = new Timer(delay, new ActionListener() {
//	        	
//	        	private final Loop lp = loopo;
//	        	private final Timer caller = timer;
//	        	private final LogDisplayPanel display = ldp;
//
//				@Override
//				public void actionPerformed(ActionEvent e) {
//					_Log l = lp.getNext();
//					Debug.print("loop chose: %s", l);
//					
//					if (l == null) {
//						caller.stop();
//						return;
//					}
//					
//					display.takeLogsFromDrop(new _Log[]{
//						l
//					});
//				}
//	        	
//	        });
//	        
//	        timer.setRepeats(true);
//	        timer.setDelay(itr);
//	        toggle.setText("stop");
//	        timer.start();
//	    }
//
//	    /**
//	     * This method is called from within the constructor to initialize the form.
//	     * WARNING: Do NOT modify this code. The content of this method is always
//	     * regenerated by the Form Editor.
//	     */
//	    @SuppressWarnings("unchecked")
//	    // <editor-fold defaultstate="collapsed" desc="Generated Code">                          
//	    private void initComponents() {
//	        displayBox = new javax.swing.JComboBox<>();
//	        logFromLabel = new javax.swing.JLabel();
//	        logToLabel = new javax.swing.JLabel();
//	        loopBox = new javax.swing.JCheckBox();
//	        toggle = new javax.swing.JButton();
//	        jLabel1 = new javax.swing.JLabel();
//	        startDelaySpinner = new javax.swing.JSpinner();
//	        itrDelaySpinner = new javax.swing.JSpinner();
//	        jLabel2 = new javax.swing.JLabel();
//	        jLabel3 = new javax.swing.JLabel();
//
//	        displayBox.setModel(new javax.swing.DefaultComboBoxModel<String>(new String[] { "MAIN", "1", "2", "3", "4", "5", "6", "7", "8", "9" }));
//
//	        logFromLabel.setText("<drag the first log of a sequence here!>");
//
//	        logToLabel.setText("<drag the last log of a sequence here!>");
//
//	        loopBox.setText("loop");
//
//	        toggle.setText("start");
//
//	        jLabel1.setText("display:");
//
//	        startDelaySpinner.setModel(new javax.swing.SpinnerNumberModel(Integer.valueOf(10), null, Integer.valueOf(5000), Integer.valueOf(100)));
//
//	        itrDelaySpinner.setModel(new javax.swing.SpinnerNumberModel(Integer.valueOf(10), null, Integer.valueOf(5000), Integer.valueOf(100)));
//
//	        jLabel2.setText("sdelay");
//
//	        jLabel3.setText("idelay");
//
//	        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
//	        getContentPane().setLayout(layout);
//	        layout.setHorizontalGroup(
//	            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
//	            .addGroup(layout.createSequentialGroup()
//	                .addContainerGap()
//	                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
//	                    .addComponent(logToLabel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
//	                    .addComponent(logFromLabel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
//	                    .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
//	                        .addComponent(jLabel1)
//	                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
//	                        .addComponent(displayBox, 0, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)))
//	                .addContainerGap())
//	            .addGroup(layout.createSequentialGroup()
//	                .addComponent(loopBox)
//	                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 14, Short.MAX_VALUE)
//	                .addComponent(jLabel2)
//	                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
//	                .addComponent(startDelaySpinner, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
//	                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
//	                .addComponent(jLabel3)
//	                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
//	                .addComponent(itrDelaySpinner, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
//	                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
//	                .addComponent(toggle)
//	                .addGap(87, 87, 87))
//	        );
//	        layout.setVerticalGroup(
//	            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
//	            .addGroup(layout.createSequentialGroup()
//	                .addContainerGap()
//	                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
//	                    .addComponent(displayBox, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
//	                    .addComponent(jLabel1))
//	                .addGap(18, 18, 18)
//	                .addComponent(logFromLabel)
//	                .addGap(18, 18, 18)
//	                .addComponent(logToLabel)
//	                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
//	                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
//	                    .addComponent(loopBox)
//	                    .addComponent(toggle)
//	                    .addComponent(startDelaySpinner, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
//	                    .addComponent(itrDelaySpinner, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
//	                    .addComponent(jLabel2)
//	                    .addComponent(jLabel3))
//	                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
//	        );
//
//	        pack();
//	    }// </editor-fold>                        
//
//	    // Variables declaration - do not modify                     
//	    private javax.swing.JComboBox<String> displayBox;
//	    private javax.swing.JSpinner itrDelaySpinner;
//	    private javax.swing.JLabel jLabel1;
//	    private javax.swing.JLabel jLabel2;
//	    private javax.swing.JLabel jLabel3;
//	    private javax.swing.JLabel logFromLabel;
//	    private javax.swing.JLabel logToLabel;
//	    private javax.swing.JCheckBox loopBox;
//	    private javax.swing.JSpinner startDelaySpinner;
//	    private javax.swing.JButton toggle;
//	    // End of variables declaration                   
//	}
//
//}
