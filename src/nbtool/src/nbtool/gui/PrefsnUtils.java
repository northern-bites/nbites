package nbtool.gui;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.io.IOException;

import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.border.Border;

import nbtool.data.SessionMaster;
import nbtool.gui.utilitypanes.UtilityManager;
import nbtool.gui.utilitypanes.UtilityParent;
import nbtool.util.N;
import nbtool.util.N.EVENT;
import nbtool.util.P;
import nbtool.util.U;


public class PrefsnUtils extends JPanel{
	private static final long serialVersionUID = 1L;
	
	private Utils utils;
	private Prefs prefs;
	
	protected PrefsnUtils() {
		super();
		setLayout(null);
		addComponentListener(new ComponentAdapter() {
			public void componentResized(ComponentEvent e) {
				useSize(e.getComponent().getSize());
			}
		});
		
		utils = new Utils();
		prefs = new Prefs();
		
		add(utils); add(prefs);
	}

	protected void useSize(Dimension size) {
		prefs.setBounds(0, 0, size.width, Prefs.REQ_HEIGHT);
		utils.setBounds(0, Prefs.REQ_HEIGHT + 20, size.width, size.height - Prefs.REQ_HEIGHT - 20);
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
	
	private class Prefs extends JPanel implements ActionListener {
		protected static final int REQ_HEIGHT = 170;
		protected Prefs() {
			
			super();
			
			setLayout(null);
			addComponentListener(new ComponentAdapter() {
				public void componentResized(ComponentEvent e) {
					prefUseSize(e.getComponent().getSize());
				}
			});
			
			Border b = BorderFactory.createLineBorder(Color.BLACK);
			setBorder(BorderFactory.createTitledBorder(b, "Preferences"));
			
			copyMappingB = new JButton("recopy view mapping (must restart)");
			copyMappingB.addActionListener(this);
			add(copyMappingB);
			
			copyExceptB = new JButton("recopy class excepts (must restart)");
			copyExceptB.addActionListener(this);
			add(copyExceptB);
			
			resetPrefB = new JButton("reset preferences (!bounds) (must restart)");
			resetPrefB.addActionListener(this);
			add(resetPrefB);
					
			verbosity = new JCheckBox("verbose tool");
			verbosity.setSelected(P.getVerbose());
			verbosity.addActionListener(this);
			add(verbosity);
		}
		
		private void prefUseSize(Dimension size) {
			Insets ins = this.getInsets();
			int y = ins.top;
			int mw = size.width - ins.left - ins.right;
			
			int height = 0;
			
			height = copyMappingB.getPreferredSize().height;
			copyMappingB.setBounds(ins.left, y, mw, height);
			y += height;
			
			height = copyExceptB.getPreferredSize().height;
			copyExceptB.setBounds(ins.left, y, mw, height);
			y += height;
			
			height = resetPrefB.getPreferredSize().height;
			resetPrefB.setBounds(ins.left, y, mw, height);
			y += height;
			
			height = verbosity.getPreferredSize().height;
			verbosity.setBounds(ins.left, y, mw, height);
			y += height;
		}
		
		private JButton copyMappingB;
		private JButton copyExceptB;
		private JButton resetPrefB;
				
		private JCheckBox verbosity;
		
		public void actionPerformed(ActionEvent e) {
			if (e.getSource() == copyMappingB) {
				try {
					P.copyOrReplaceMapping();
				} catch (IOException e1) {
					e1.printStackTrace();
				}
			} else if (e.getSource() == copyExceptB){
				try {
					P.copyOrReplaceExceptions();
				} catch (IOException e1) {
					e1.printStackTrace();
				}
			} else if (e.getSource() == resetPrefB){
				U.w("Prefs: reseting preferences.");
				P.resetNonFilePreferences();
			} 
			else {
				
			}
		}
	}
}
