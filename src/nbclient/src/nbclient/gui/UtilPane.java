package nbclient.gui;

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
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.border.Border;

import nbclient.gui.utilitypanes.UtilityManager;
import nbclient.gui.utilitypanes.UtilityParent;
import nbclient.util.P;
import nbclient.util.U;


public class UtilPane extends JPanel{
	private Utils utils;
	private Prefs prefs;
	
	protected UtilPane() {
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
		protected static final int REQ_HEIGHT = 90;
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
			
			maxMemoryUsage = new JTextField("300000");
			maxMemoryUsage.addActionListener(this);
			JLabel lbl = new JLabel("max memory: ");
			mmuPanel = U.fieldWithlabel(lbl, maxMemoryUsage);
			
			add(mmuPanel);
			
		}
		
		private void prefUseSize(Dimension size) {
			Insets ins = this.getInsets();
			int y = ins.top;
			int mw = size.width - ins.left - ins.right;
			
			int height = mmuPanel.getPreferredSize().height;
			mmuPanel.setBounds(ins.left, y, mw, height);
			y += height;
			
			height = copyMappingB.getPreferredSize().height;
			copyMappingB.setBounds(ins.left, y, mw, height);
			y += height;
		}
		
		private JButton copyMappingB;
		private JTextField maxMemoryUsage;
		private JPanel mmuPanel;
		
		public void actionPerformed(ActionEvent e) {
			if (e.getSource() == copyMappingB) {
				try {
					P.copyOrReplaceMapping();
				} catch (IOException e1) {
					e1.printStackTrace();
				}
			} else {
				U.w("" + e.getSource());
			}
		}
	}
}
