package nbtool.gui;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.io.IOException;
import java.util.Arrays;

import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.DefaultComboBoxModel;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.border.Border;

import nbtool.data.SessionMaster;
import nbtool.gui.utilitypanes.UtilityManager;
import nbtool.gui.utilitypanes.UtilityParent;
import nbtool.util.Logger;
import nbtool.util.Logger.LogLevel;
import nbtool.util.Prefs;
import nbtool.util.Utility;


public class OptionsAndUtilities extends JPanel{
	private static final long serialVersionUID = 1L;
		
	protected OptionsAndUtilities() {
		super();
		
		initComponents();
				
		levelBox.setModel(new DefaultComboBoxModel<Logger.LogLevel>(Logger.LogLevel.values()));
		levelBox.setEditable(false);
		levelBox.setSelectedIndex(Arrays.asList(Logger.LogLevel.values()).indexOf(Logger.level));
		levelBox.addActionListener(new ActionListener(){

			@Override
			public void actionPerformed(ActionEvent e) {
				int sel = levelBox.getSelectedIndex();
				Logger.level = Logger.LogLevel.values()[sel];
				Prefs.logLevel = Logger.level;
			}
			
		});
		
		newWindowButton.addActionListener(new ActionListener(){
			@Override
			public void actionPerformed(ActionEvent e) {
				ExternalLogDisplay.requestAny();
			}
		});
		
		//utilityPanel.setLayout(new GridLayout(UtilityManager.utilities.length, 1));
		utilityHolder.setLayout(new BoxLayout(utilityHolder, BoxLayout.Y_AXIS));
		for (UtilityParent up : UtilityManager.utilities) {
			utilityHolder.add(new UtilityBox(up));
			utilityHolder.add(Box.createVerticalStrut(5));
		}
		utilityHolder.add(Box.createVerticalGlue());
		
		disposeAllButton.setVisible(false);
		na1.setVisible(false);
		na2.setVisible(false);
		na3.setVisible(false);
		na4.setVisible(false);
		
		KeyBind.subRight = this.subRight;
	}
	private void dropStatsBoxActionPerformed(java.awt.event.ActionEvent evt) {
		SessionMaster.dropSTATS = dropStatsBox.isSelected();
	}                                                                             

	private void newWindowButtonActionPerformed(java.awt.event.ActionEvent evt) {  
		//TODO
	} 
	
	private void disposeAllButtonActionPerformed(java.awt.event.ActionEvent evt) {   
		//TODO
	}

	private class UtilityBox extends JPanel {
		private UtilityParent util;
		public UtilityBox(UtilityParent up) {
			initComponents();
			
			util = up;
			char memn = up.preferredMemnonic();
			if (memn > 0) {
				this.memLabel.setText("" + memn);
			} else {
				this.memLabel.setText("n/a");
			}
			
			this.purposeLabel.setText(up.purpose());
			this.showButton.setText(up.getClass().getSimpleName());
			this.showButton.addActionListener(new ActionListener(){
				@Override
				public void actionPerformed(ActionEvent e) {
					util.getDisplay().setVisible(true);
				}
			});
			this.setPreferredSize(new Dimension(Integer.MAX_VALUE, 59));
			this.setMaximumSize(this.getPreferredSize());
	    }

		private void initComponents() {
	        purposeLabel = new javax.swing.JLabel();
	        showButton = new javax.swing.JButton();
	        memLabel = new javax.swing.JLabel();

	        setBorder(javax.swing.BorderFactory.createLineBorder(new java.awt.Color(0, 0, 0)));

	        purposeLabel.setText("jLabel1");

	        showButton.setText("jButton1");

	        memLabel.setFont(new java.awt.Font("Lucida Grande", 3, 13)); // NOI18N
	        memLabel.setText("jLabel2");

	        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
	        this.setLayout(layout);
	        layout.setHorizontalGroup(
	            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
	            .addGroup(layout.createSequentialGroup()
	                .addContainerGap()
	                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
	                    .addGroup(layout.createSequentialGroup()
	                        .addComponent(purposeLabel)
	                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 246, Short.MAX_VALUE)
	                        .addComponent(memLabel))
	                    .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
	                        .addGap(0, 0, Short.MAX_VALUE)
	                        .addComponent(showButton)))
	                .addContainerGap())
	        );
	        layout.setVerticalGroup(
	            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
	            .addGroup(layout.createSequentialGroup()
	                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
	                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
	                    .addComponent(purposeLabel)
	                    .addComponent(memLabel))
	                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
	                .addComponent(showButton))
	        );
	    }// </editor-fold>                           


	    // Variables declaration - do not modify                     
	    private javax.swing.JLabel memLabel;
	    private javax.swing.JLabel purposeLabel;
	    private javax.swing.JButton showButton;
	    // End of variables declaration          
	}
	
	private void initComponents() {
        subRight = new javax.swing.JTabbedPane();
        utilityHolder = new javax.swing.JPanel();
        optionHolder = new javax.swing.JPanel();
        loggingLabel = new javax.swing.JLabel();
        levelBox = new javax.swing.JComboBox<>();
        dropStatsBox = new javax.swing.JCheckBox();
        disposeAllButton = new javax.swing.JButton();
        newWindowButton = new javax.swing.JButton();
        na1 = new javax.swing.JCheckBox();
        na2 = new javax.swing.JCheckBox();
        na3 = new javax.swing.JButton();
        na4 = new javax.swing.JButton();

        javax.swing.GroupLayout utilityHolderLayout = new javax.swing.GroupLayout(utilityHolder);
        utilityHolder.setLayout(utilityHolderLayout);
        utilityHolderLayout.setHorizontalGroup(
            utilityHolderLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 235, Short.MAX_VALUE)
        );
        utilityHolderLayout.setVerticalGroup(
            utilityHolderLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 473, Short.MAX_VALUE)
        );

        subRight.addTab("utilities", utilityHolder);

        loggingLabel.setText("logging level");

        dropStatsBox.setText("drop STATS");
        dropStatsBox.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                dropStatsBoxActionPerformed(evt);
            }
        });

        disposeAllButton.setText("dispose");
        disposeAllButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                disposeAllButtonActionPerformed(evt);
            }
        });

        newWindowButton.setText("window");
        newWindowButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                newWindowButtonActionPerformed(evt);
            }
        });

        na1.setText("n/a");
        na1.setEnabled(false);

        na2.setText("n/a");
        na2.setEnabled(false);

        na3.setText("n/a");
        na3.setEnabled(false);

        na4.setText("n/a");
        na4.setEnabled(false);

        javax.swing.GroupLayout optionHolderLayout = new javax.swing.GroupLayout(optionHolder);
        optionHolder.setLayout(optionHolderLayout);
        optionHolderLayout.setHorizontalGroup(
            optionHolderLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(optionHolderLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(optionHolderLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(optionHolderLayout.createSequentialGroup()
                        .addComponent(newWindowButton)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                        .addComponent(disposeAllButton))
                    .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, optionHolderLayout.createSequentialGroup()
                        .addComponent(na4)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                        .addComponent(na3))
                    .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, optionHolderLayout.createSequentialGroup()
                        .addGap(0, 0, Short.MAX_VALUE)
                        .addGroup(optionHolderLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(dropStatsBox, javax.swing.GroupLayout.Alignment.TRAILING)
                            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, optionHolderLayout.createSequentialGroup()
                                .addComponent(loggingLabel, javax.swing.GroupLayout.PREFERRED_SIZE, 92, javax.swing.GroupLayout.PREFERRED_SIZE)
                                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                                .addComponent(levelBox, javax.swing.GroupLayout.PREFERRED_SIZE, 103, javax.swing.GroupLayout.PREFERRED_SIZE))
                            .addComponent(na1, javax.swing.GroupLayout.Alignment.TRAILING)
                            .addComponent(na2, javax.swing.GroupLayout.Alignment.TRAILING))))
                .addContainerGap())
        );
        optionHolderLayout.setVerticalGroup(
            optionHolderLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(optionHolderLayout.createSequentialGroup()
                .addGroup(optionHolderLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                    .addComponent(levelBox, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(loggingLabel, javax.swing.GroupLayout.PREFERRED_SIZE, 24, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(dropStatsBox)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(na1)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(na2)
                .addGap(42, 42, 42)
                .addGroup(optionHolderLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(disposeAllButton)
                    .addComponent(newWindowButton))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(optionHolderLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(na3)
                    .addComponent(na4))
                .addGap(0, 253, Short.MAX_VALUE))
        );

        subRight.addTab("options", optionHolder);

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(subRight)
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(subRight)
        );
    }// </editor-fold>                                                                   


    // Variables declaration - do not modify                     
    private javax.swing.JButton disposeAllButton;
    private javax.swing.JCheckBox dropStatsBox;
    private javax.swing.JTabbedPane subRight;
    private javax.swing.JComboBox<Logger.LogLevel> levelBox;
    private javax.swing.JLabel loggingLabel;
    private javax.swing.JCheckBox na1;
    private javax.swing.JCheckBox na2;
    private javax.swing.JButton na3;
    private javax.swing.JButton na4;
    private javax.swing.JButton newWindowButton;
    private javax.swing.JPanel optionHolder;
    private javax.swing.JPanel utilityHolder;
    // End of variables declaration
}
