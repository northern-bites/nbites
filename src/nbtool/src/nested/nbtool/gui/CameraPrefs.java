package nbtool.gui;

import java.awt.event.*;
import java.awt.*;

import javax.swing.*;
import javax.swing.border.Border;
import javax.swing.event.DocumentEvent;
import javax.swing.text.Document;
import javax.swing.text.Element;

import java.io.IOException;

import nbtool.data.*;
import nbtool.gui.utilitypanes.*;

public class CameraPrefs extends JPanel {
	private static final long serialVersionUID = 1L;
	
	static final int blockHeight = 500;
	
	private CameraPrefsField prefsField;
	
	String[] listOfParams = new String[] {
		"H_FLIP [0/1]",
		"V_FLIP [0/1]",
		"Auto-Exposure [0/1]",
		"Brightness [0..255]",
		"Contrast [16..64]",
		"Saturation [0..255]",
		"Hue [-22..22]",
		"Sharpness [0..255]",
		"Gamma [0..1000]",
		"Auto-Whitebalance [0/1]",
		"Exposure [1..1000]",
		"Gain [0..255]",
		"Wh-Balance [2700..6500]",
		"Fade-To-Black [0/1]"
	};
	//all params except for BackLight-Compensation.
	JTextField[] fields = new JTextField[] {
		new JTextField(4),
		new JTextField(4),
		new JTextField(4),
		new JTextField(4),
		new JTextField(4),
		new JTextField(4),
		new JTextField(4),
		new JTextField(4),
		new JTextField(4),
		new JTextField(4),
		new JTextField(4),
		new JTextField(4),
		new JTextField(4),
		new JTextField(4),
	};
	
	protected CameraPrefs(String whichCamera) {
		super();
		setLayout(null);
		setPreferredSize(new Dimension(250,blockHeight));
		addComponentListener(new ComponentAdapter() {
			public void componentResized(ComponentEvent e) {
				useSize(e.getComponent().getSize());
			}
		});
		
		prefsField = new CameraPrefsField(whichCamera);
		add(prefsField);
	}
	
	protected void useSize(Dimension size) {
		prefsField.setBounds(0,0,270,blockHeight);
	}
	
	JButton newButton;
	JPanel optionsPanel;
	
	public Integer[] getParameterValues() {
		Integer [] paramValues = new Integer[fields.length];
		for(int i=0; i<fields.length; i++) {
			paramValues[i] = Integer.parseInt(fields[i].getText());
		}
		return paramValues;
	}
	
	//backlight compensation parameter not set here
	public void setInitialParameterValues(String whichCamera) {
		//Searles Camera Parameters
		if(whichCamera.equals("Top Camera")) {
			fields[0] = new JTextField("1",4);
			fields[1] = new JTextField("1",4);
			fields[2] = new JTextField("0",4);
			fields[3] = new JTextField("250",4);
			fields[4] = new JTextField("43",4);
			fields[5] = new JTextField("144",4);
			fields[6] = new JTextField("0",4);
			fields[7] = new JTextField("4",4);
			fields[8] = new JTextField("220",4);
			fields[9] = new JTextField("0",4);
			fields[10] = new JTextField("250",4);
			fields[11] = new JTextField("53",4);
			fields[12] = new JTextField("3300",4);
			fields[13] = new JTextField("0",4);
		} else {
			fields[0] = new JTextField("0",4);
			fields[1] = new JTextField("0",4);
			fields[2] = new JTextField("0",4);
			fields[3] = new JTextField("200",4);
			fields[4] = new JTextField("56",4);
			fields[5] = new JTextField("122",4);
			fields[6] = new JTextField("0",4);
			fields[7] = new JTextField("3",4);
			fields[8] = new JTextField("220",4);
			fields[9] = new JTextField("0",4);
			fields[10] = new JTextField("250",4);
			fields[11] = new JTextField("62",4);
			fields[12] = new JTextField("3200",4);
			fields[13] = new JTextField("0",4);
		}
	}
	
	private class CameraPrefsField extends JPanel implements ActionListener, DocumentEvent {
		
		protected JComponent getTwoColumnLayout(JLabel[] labels, JComponent[] fields) {
			if(labels.length != fields.length) {
				String s = "Inconsistent # of labels and fields";
				throw new IllegalArgumentException(s);
			}
			
			JComponent panel = new JPanel();
			GroupLayout layout = new GroupLayout(panel);
			panel.setLayout(layout);
			layout.setAutoCreateGaps(true);
			GroupLayout.SequentialGroup hGroup = layout.createSequentialGroup();
			GroupLayout.Group yLabelGroup = layout.createParallelGroup(GroupLayout.Alignment.TRAILING);
			hGroup.addGroup(yLabelGroup);
			
			GroupLayout.Group yFieldGroup = layout.createParallelGroup();
			hGroup.addGroup(yFieldGroup);
			layout.setHorizontalGroup(hGroup);
			
			GroupLayout.SequentialGroup vGroup = layout.createSequentialGroup();
			layout.setVerticalGroup(vGroup);
			
			int p = GroupLayout.PREFERRED_SIZE;
			
			for(JLabel label : labels) {
				yLabelGroup.addComponent(label);
			}
			for(Component field : fields) {
				yFieldGroup.addComponent(field,p,p,p);
				for(int i=0; i<labels.length; i++) {
					vGroup.addGroup(layout.createParallelGroup().
							addComponent(labels[i]).
							addComponent(fields[i],p,p,p));
				}
			}
			return panel;
		}
		
		protected JComponent getTwoColumnLayout(String[] labelStrings, JComponent[] fields) {
			JLabel[] labels = new JLabel[labelStrings.length];
			for(int i=0; i<labels.length; i++) {
				labels[i] = new JLabel(labelStrings[i]);
			}
			return getTwoColumnLayout(labels,fields);
		}
		
		protected CameraPrefsField(String whichCamera) {
			super();
			setInitialParameterValues(whichCamera);
			JComponent labelsAndFields = getTwoColumnLayout(listOfParams,fields);
			JComponent optionsPanel = new JPanel(new BorderLayout(5,5));
			optionsPanel.add(labelsAndFields,BorderLayout.CENTER);
			optionsPanel.setBorder(BorderFactory.createTitledBorder(whichCamera));
			optionsPanel.setPreferredSize(new Dimension(250,blockHeight));
			optionsPanel.setMinimumSize(new Dimension(250,blockHeight));
			add(optionsPanel);
		}
		
		private void prefUseSize(Dimension size) {
			optionsPanel.setBounds(0, 0, 250, blockHeight);
		}

		@Override
		public void actionPerformed(ActionEvent e) {
			// TODO Auto-generated method stub
			
		}

		@Override
		public int getOffset() {
			// TODO Auto-generated method stub
			return 0;
		}

		@Override
		public int getLength() {
			// TODO Auto-generated method stub
			return 0;
		}

		@Override
		public Document getDocument() {
			// TODO Auto-generated method stub
			return null;
		}

		@Override
		public EventType getType() {
			// TODO Auto-generated method stub
			return null;
		}

		@Override
		public ElementChange getChange(Element elem) {
			// TODO Auto-generated method stub
			return null;
		}
	}
	
}
