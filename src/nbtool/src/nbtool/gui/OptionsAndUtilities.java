package nbtool.gui;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.io.IOException;
import java.util.Arrays;

import javax.swing.BorderFactory;
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
	
	private Utils utils;
	private Options opts;
	
	protected OptionsAndUtilities() {
		super();
		setLayout(null);
		addComponentListener(new ComponentAdapter() {
			public void componentResized(ComponentEvent e) {
				useSize(e.getComponent().getSize());
			}
		});
		
		utils = new Utils();
		opts = new Options();
		
		add(utils); add(opts);
	}

	protected void useSize(Dimension size) {
		opts.setBounds(0, 0, size.width, Options.REQ_HEIGHT);
		utils.setBounds(0, Options.REQ_HEIGHT + 20, size.width, size.height - Options.REQ_HEIGHT - 20);
	}
	
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
	}
	
	private class Options extends JPanel implements ActionListener {
		protected static final int REQ_HEIGHT = 80;
		protected Options() {
			
			super();
			
			setLayout(null);
			addComponentListener(new ComponentAdapter() {
				public void componentResized(ComponentEvent e) {
					prefUseSize(e.getComponent().getSize());
				}
			});
			
			Border b = BorderFactory.createLineBorder(Color.BLACK);
			setBorder(BorderFactory.createTitledBorder(b, "Options"));
			
			resetPrefB = new JButton("reset preferences (!bounds) (must restart)");
			resetPrefB.addActionListener(this);
			add(resetPrefB);
			
			LogLevel[] levels = Logger.LogLevel.values();
			String [] model = new String[levels.length];
			for (int i = 0; i < levels.length; ++i)
				model[i] = levels[i].toString();
			logLevel = new JComboBox<String>(model);
			logLevel.setEditable(false);
			logLevel.setSelectedIndex(Arrays.asList(levels).indexOf(Logger.level));
			logLevel.addActionListener(this);
					
			add(logLevel);
		}
		
		private void prefUseSize(Dimension size) {
			Insets ins = this.getInsets();
			int y = ins.top;
			int mw = size.width - ins.left - ins.right;
			
			int height = 0;
			
			height = resetPrefB.getPreferredSize().height;
			resetPrefB.setBounds(ins.left, y, mw, height);
			y += height;
			
			height = logLevel.getPreferredSize().height;
			logLevel.setBounds(ins.left, y, mw, height);
			y += height;
		}
		
		private JButton resetPrefB;
		private JComboBox<String> logLevel;
		
		public void actionPerformed(ActionEvent e) {
			
			if (e.getSource() == resetPrefB){
				Logger.log(Logger.WARN, "can't reset preferences right now.");
			} else {
				int sel = logLevel.getSelectedIndex();
				Logger.level = Logger.LogLevel.values()[sel];
				Prefs.logLevel = Logger.level;
			} 
		}
	}
}
