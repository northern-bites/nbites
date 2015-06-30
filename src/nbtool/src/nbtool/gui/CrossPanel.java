package nbtool.gui;

import java.io.IOException;
import java.util.ArrayList;

import javax.swing.AbstractListModel;
import javax.swing.DefaultComboBoxModel;
import javax.swing.JList;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;
import javax.swing.tree.TreePath;

import nbtool.data.Log;
import nbtool.data.Session;
import nbtool.io.CrossIO;
import nbtool.io.CrossIO.CrossCall;
import nbtool.io.FileIO;
import nbtool.io.CommonIO.IOInstance;
import nbtool.io.CrossIO.CrossFunc;
import nbtool.io.CrossIO.CrossInstance;
import nbtool.io.CommonIO.IOFirstResponder;
import nbtool.util.Center;
import nbtool.util.Events;
import nbtool.util.Logger;

public class CrossPanel extends JPanel implements Events.CrossStatus, IOFirstResponder {
	
	private ArrayList<CrossInstance> recognized = new ArrayList<>();
	private CrossListModel<CrossInstance> recModel = new CrossListModel<>(recognized);
	
	private ArrayList<Log> output = new ArrayList<>();
	private CrossListModel<Log> outModel = new CrossListModel<>(output);
	
	private LogChooser chooser;
	
	private class CrossListModel<T> extends AbstractListModel<T> {
		
		private ArrayList<T> relevant;
		protected CrossListModel(ArrayList<T> rel) {
			super();
			this.relevant = rel;
		}

		@Override
		public int getSize() {
			return relevant.size();
		}

		@Override
		public T getElementAt(int index) {
			return relevant.get(index);
		}
		
		public void update() {
			this.fireContentsChanged(this, 0, relevant.size());
		}
	}
	/**
     * Creates new form CrossPane
     */
    public CrossPanel(LogChooser chooser) {
        initComponents();
        
        this.chooser = chooser;
        
        instanceList.setModel(recModel);
        instanceList.addListSelectionListener(new ListSelectionListener(){

			@Override
			public void valueChanged(ListSelectionEvent e) {
				if (e.getValueIsAdjusting())
					return;
				
				JList<CrossInstance> list = (JList<CrossInstance>) e.getSource();
				int index = list.getSelectedIndex();
				
				if (index < 0 || index >= recognized.size())
					return;
				Logger.log(Logger.INFO, "NBCrossPane instance selected.");
				
				CrossInstance ci = recognized.get(index);
				assert(ci != null);
				assert(ci.functions != null);
				
				functionBox.setModel(new DefaultComboBoxModel<CrossFunc>(ci.functions.toArray(new CrossFunc[0])));
				functionBox.setSelectedIndex(-1);
			}
        	
        });
        
        outList.setModel(outModel);
        outList.addListSelectionListener(new ListSelectionListener(){

			@Override
			public void valueChanged(ListSelectionEvent e) {
				if (e.getValueIsAdjusting())
					return;
				
				JList<Log> list = (JList<Log>) e.getSource();
				int index = list.getSelectedIndex();
				
				if (index < 0 || index >= output.size())
					return;
				Logger.log(Logger.INFO, "NBCrossPane output selected.");
				
				Log sel = output.get(index);
				Events.GLogSelected.generate(this, sel, new ArrayList<Log>());
			}
        	
        });
        
        clearOutButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
            	clearOutButtonActionPerformed(evt);
            }
        });
        
        callButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
            	callButtonActionPerformed(evt);
            }
        });
        
        Center.listen(Events.CrossStatus.class, this, true);
    }
    
    private void clearOutButtonActionPerformed(java.awt.event.ActionEvent evt) {  
    	Logger.log(Logger.INFO, "NBCrossPane clear button.");
    	output.clear();
    	outModel.update();
    }   
    
    private void callButtonActionPerformed(java.awt.event.ActionEvent evt) {                                                
    	int inst_i = instanceList.getSelectedIndex();
    	int func_i = functionBox.getSelectedIndex();
    	
    	if (inst_i < 0 || func_i < 0) {
    		JOptionPane.showMessageDialog(this, "must select instance and function");
    		return;
    	}
    	
    	Logger.log(Logger.INFO, "NBCrossPane call button.");
    	
    	CrossInstance ci = recognized.get(inst_i);
    	CrossFunc cf = (CrossFunc) functionBox.getSelectedItem();
    	
    	assert(ci != null && cf != null && cf == ci.functions.get(func_i));
    	
    	Log[] args = new Log[cf.args.length];
    	TreePath[] pathes = chooser.selection();
    	
    	if (pathes.length < args.length) {
    		JOptionPane.showMessageDialog(this, String.format("function requires %d arguments but found %d selections.",
    				args.length, pathes.length));
    		return;
    	}
    	
    	for (int i = 0; i < args.length; ++i) {
    		TreePath path = pathes[i];
    		if (path.getPathCount() != 3) {
    			JOptionPane.showMessageDialog(this, String.format("argument %d is session: no session arguments allowed.",
        				i));
        		return;
    		}
    		
    		Session ses = (Session)path.getPath()[1];
			Log sel = (Log) path.getPath()[2];
			
			if (sel.bytes == null) {
				assert(ses.directoryFrom != null && !ses.directoryFrom.isEmpty());
				try {
					FileIO.loadLog(sel, ses.directoryFrom);
				} catch (IOException e1) {
					Logger.logf(Logger.ERROR, "Could not load log data!");
					e1.printStackTrace();
					
					return;
				}
				Events.GLogLoaded.generate(this, sel);
			}
			
			Logger.logf(Logger.INFO, "NBCrossPane argument: %s", sel.description());
			args[i] = sel;
    	}
    	
    	CrossCall call = new CrossCall(this, cf, args);
    	if (!cf.accepts(call)) {
    		
    		JOptionPane.showMessageDialog(this, String.format("bad args, try %s",
    				cf.args.toString()));
    		return;
    	}
    	
    	ci.tryAddCall(call);
    }  
    
    @Override
	public void nbCrossFound(CrossInstance inst, boolean up) {
		if (up) {
			assert(!recognized.contains(inst));
			recognized.add(inst);
			recModel.update();
		} else {
			if (recognized.contains(inst)) {
				recognized.remove(inst);
				recModel.update();
				
				functionBox.setModel(new DefaultComboBoxModel<CrossFunc>());
				functionBox.setSelectedIndex(-1);
			}
		}
	}

	@Override
	public void ioFinished(IOInstance instance) {}

	@Override
	public void ioReceived(IOInstance inst, int ret, Log... out) {
		for (Log l : out) {
			Logger.logf(Logger.INFO, "NBCrossPane function returned: %s", l.description());
			assert(l != null);
			output.add(l);
			outModel.update();
		}
	}

	@Override
	public boolean ioMayRespondOnCenterThread(IOInstance inst) {
		return false;
	}

	/**
     * This method is called from within the constructor to initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is always
     * regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">                          
    private void initComponents() {
        jProgressBar1 = new javax.swing.JProgressBar();
        jScrollPane2 = new javax.swing.JScrollPane();
        outList = new javax.swing.JList<>();
        jScrollPane3 = new javax.swing.JScrollPane();
        instanceList = new javax.swing.JList<>();
        functionBox = new javax.swing.JComboBox<>();
        callButton = new javax.swing.JButton();
        clearOutButton = new javax.swing.JButton();

        outList.setBorder(javax.swing.BorderFactory.createTitledBorder("output"));
        
        jScrollPane2.setViewportView(outList);

        instanceList.setBorder(javax.swing.BorderFactory.createTitledBorder("cross instances"));
      
        jScrollPane3.setViewportView(instanceList);

        functionBox.setBorder(javax.swing.BorderFactory.createTitledBorder("functions"));
        functionBox.setMinimumSize(new java.awt.Dimension(108, 81));
        functionBox.setPreferredSize(new java.awt.Dimension(108, 81));

        callButton.setText("call");

        clearOutButton.setText("clear out");

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(functionBox, 0, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addGroup(layout.createSequentialGroup()
                        .addContainerGap()
                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(jScrollPane2)
                            .addGroup(layout.createSequentialGroup()
                                .addComponent(callButton)
                                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 208, Short.MAX_VALUE)
                                .addComponent(clearOutButton, javax.swing.GroupLayout.PREFERRED_SIZE, 105, javax.swing.GroupLayout.PREFERRED_SIZE))
                            .addComponent(jScrollPane3, javax.swing.GroupLayout.DEFAULT_SIZE, 388, Short.MAX_VALUE))))
                .addContainerGap())
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jScrollPane3, javax.swing.GroupLayout.PREFERRED_SIZE, 125, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(10, 10, 10)
                .addComponent(functionBox, javax.swing.GroupLayout.PREFERRED_SIZE, 83, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jScrollPane2, javax.swing.GroupLayout.PREFERRED_SIZE, 366, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(callButton)
                    .addComponent(clearOutButton)))
        );
    }// </editor-fold>                        

    // Variables declaration - do not modify                     
    private javax.swing.JButton callButton;
    private javax.swing.JButton clearOutButton;
    private javax.swing.JComboBox<CrossIO.CrossFunc> functionBox;
    private javax.swing.JList<CrossIO.CrossInstance> instanceList;
    private javax.swing.JProgressBar jProgressBar1;
    private javax.swing.JScrollPane jScrollPane2;
    private javax.swing.JScrollPane jScrollPane3;
    private javax.swing.JList<Log> outList;
    // End of variables declaration            
}
