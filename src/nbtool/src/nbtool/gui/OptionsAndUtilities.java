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
		
		resetButton.setEnabled(false);
		
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
		
		//utilityPanel.setLayout(new GridLayout(UtilityManager.utilities.length, 1));
		utilityPanel.setLayout(new BoxLayout(utilityPanel, BoxLayout.Y_AXIS));
		for (UtilityParent up : UtilityManager.utilities) {
			utilityPanel.add(new UtilityBox(up));
			utilityPanel.add(Box.createVerticalStrut(5));
		}
		utilityPanel.add(Box.createVerticalGlue());
	}
	
	/*
	private class Utils extends JPanel implements ActionListener {
		protected Utils() {
			super();
			
			setLayout(null);
			addComponentListener(new ComponentAdapter() {
				public void componentResized(ComponentEvent e) {
					utilUseSize(e.getComponent().getSize());
				}
			});
			
			Border b = BorderFactory.createLineBorder(Color.BLACK);
			setBorder(BorderFactory.createTitledBorder(b, "Utilities"));
			
			for (int i = 0; i < ubuttons.length; ++i) {
				ubuttons[i] = new JButton("" + UtilityManager.utilities[i].getSimpleName());
				ubuttons[i].addActionListener(this);
				ubuttons[i].setName("" + i);
				add(ubuttons[i]);
			}
			
		}
		
		
		
		private JButton[] ubuttons = new JButton[UtilityManager.utilities.length];

		private void utilUseSize(Dimension size) {
			Insets ins = this.getInsets();
			int y = ins.top;
			int mw = size.width - ins.left - ins.right;
			for (int i = 0; i < ubuttons.length; ++i) {
				int height = ubuttons[i].getPreferredSize().height;
				ubuttons[i].setBounds(ins.left, y, mw, height);
				y += height;
			}
		}
		
		public void actionPerformed(ActionEvent e) {
			if (e.getSource() instanceof JButton) {
				JButton b = (JButton) e.getSource();
				int bindex = Integer.parseInt(b.getName());
				
				UtilityParent inst = UtilityManager.instanceOf(UtilityManager.utilities[bindex]);
				inst.setVisible(true);
			}
		}
	} */
	
	private class UtilityBox extends JPanel {
		private UtilityParent util;
		public UtilityBox(UtilityParent up) {
			initComponents();
			
			util = up;
			this.memLabel.setText("n/a");
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
        optionsPanel = new javax.swing.JPanel();
        resetButton = new javax.swing.JButton();
        levelBox = new javax.swing.JComboBox<>();
        jLabel1 = new javax.swing.JLabel();
        utilityPanel = new javax.swing.JPanel();

        optionsPanel.setBorder(javax.swing.BorderFactory.createTitledBorder(javax.swing.BorderFactory.createBevelBorder(javax.swing.border.BevelBorder.RAISED), "options"));

        resetButton.setText("reset non-bounds prefs");

        jLabel1.setText("logging level:");

        javax.swing.GroupLayout optionsPanelLayout = new javax.swing.GroupLayout(optionsPanel);
        optionsPanel.setLayout(optionsPanelLayout);
        optionsPanelLayout.setHorizontalGroup(
            optionsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(optionsPanelLayout.createSequentialGroup()
                .addContainerGap(32, Short.MAX_VALUE)
                .addGroup(optionsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                    .addComponent(resetButton, javax.swing.GroupLayout.Alignment.TRAILING)
                    .addGroup(optionsPanelLayout.createSequentialGroup()
                        .addComponent(jLabel1)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(levelBox, 0, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)))
                .addContainerGap())
        );
        optionsPanelLayout.setVerticalGroup(
            optionsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(optionsPanelLayout.createSequentialGroup()
                .addComponent(resetButton)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(optionsPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(levelBox, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(jLabel1)))
        );

        utilityPanel.setBorder(javax.swing.BorderFactory.createTitledBorder(javax.swing.BorderFactory.createBevelBorder(javax.swing.border.BevelBorder.RAISED), "utilities"));

        javax.swing.GroupLayout utilityPanelLayout = new javax.swing.GroupLayout(utilityPanel);
        utilityPanel.setLayout(utilityPanelLayout);
        utilityPanelLayout.setHorizontalGroup(
            utilityPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 0, Short.MAX_VALUE)
        );
        utilityPanelLayout.setVerticalGroup(
            utilityPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 391, Short.MAX_VALUE)
        );

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(utilityPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(optionsPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                .addContainerGap())
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(optionsPanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(utilityPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addContainerGap())
        );
    }// </editor-fold>                        


    // Variables declaration - do not modify                     
    private javax.swing.JLabel jLabel1;
    private javax.swing.JComboBox<Logger.LogLevel> levelBox;
    private javax.swing.JPanel optionsPanel;
    private javax.swing.JButton resetButton;
    private javax.swing.JPanel utilityPanel;
    // End of variables declaration 
}
