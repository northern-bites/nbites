package nbtool.gui;

import java.awt.event.*;
import java.awt.*;

import javax.swing.*;
import javax.swing.border.Border;
import javax.swing.event.DocumentEvent;
import javax.swing.text.Document;
import javax.swing.text.Element;

import nbtool.data.*;
import nbtool.util.*;
import nbtool.gui.utilitypanes.*;


import java.io.File;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.FileReader;
import java.io.BufferedWriter;
import java.io.BufferedReader;
import java.io.IOException;

import java.util.Scanner;

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
	
	protected CameraPrefs(String whichCamera, int naoVersion) {
		super();
		setLayout(null);
		setPreferredSize(new Dimension(250,blockHeight));
		addComponentListener(new ComponentAdapter() {
			public void componentResized(ComponentEvent e) {
				useSize(e.getComponent().getSize());
			}
		});
		
		prefsField = new CameraPrefsField(whichCamera, naoVersion);
		add(prefsField);
	}
	
	protected void useSize(Dimension size) {
		prefsField.setBounds(0,0,270,blockHeight);
	}
	
	JPanel optionsPanel;

	SExpr top;
	SExpr bot;
	
	public Integer[] getParameterValues() {
		Integer [] paramValues = new Integer[fields.length];
		for(int i=0; i<fields.length; i++) {
			paramValues[i] = Integer.parseInt(fields[i].getText());
		}
		return paramValues;
	}

	public void refresh(String whichCamera, int naoVersion) {
		readExistingParamsFromFile(naoVersion);
		setInitialParameterValues(whichCamera);
		repaint();
	}

	public void readExistingParamsFromFile(int naoVersion) {

		String topFileName, botFileName;
		String topFileContents = ""; 
		String botFileContents = "";

		if(naoVersion == 4) {
			topFileName = "../../../src/man/config/V4topCameraParams.txt";
			botFileName = "../../../src/man/config/V4bottomCameraParams.txt";
		} else {
			topFileName = "../../../src/man/config/V5topCameraParams.txt";
			botFileName = "../../../src/man/config/V5bottomCameraParams.txt";
		}

		try {
			topFileContents = readFile(topFileName);
		} catch (IOException e) {
			System.err.println("Error Reading Top Parameter File");
		}

		try {
			botFileContents = readFile(botFileName);
		} catch (IOException e) {
			System.err.println("Error Reading Bottom Parameter File");
		}

		top = SExpr.deserializeFrom(topFileContents);
		bot = SExpr.deserializeFrom(botFileContents);
	}

	private String readFile(String fileName) throws IOException {
		String content = new Scanner(new File(fileName)).useDelimiter("\\Z").next();
		return content;
	}
	
	//backlight compensation parameter not set here
	public void setInitialParameterValues(String whichCamera) {
		if(whichCamera.equals("Top Camera")) {
			fields[0].setText(top.find("hflip").get(1).value());
			fields[1].setText(top.find("vflip").get(1).value());
			fields[2].setText(top.find("autoexposure").get(1).value());
			fields[3].setText(top.find("brightness").get(1).value()); 
			fields[4].setText(top.find("contrast").get(1).value());
			fields[5].setText(top.find("saturation").get(1).value());
			fields[6].setText(top.find("hue").get(1).value());
			fields[7].setText(top.find("sharpness").get(1).value());
			fields[8].setText(top.find("gamma").get(1).value());
			fields[9].setText(top.find("auto_whitebalance").get(1).value());
			fields[10].setText(top.find("exposure").get(1).value());
			fields[11].setText(top.find("gain").get(1).value());
			fields[12].setText(top.find("white_balance").get(1).value());
			fields[13].setText(top.find("fade_to_black").get(1).value());
		} else {
			fields[0].setText(bot.find("hflip").get(1).value());
			fields[1].setText(bot.find("vflip").get(1).value());
			fields[2].setText(bot.find("autoexposure").get(1).value());
			fields[3].setText(bot.find("brightness").get(1).value()); 
			fields[4].setText(bot.find("contrast").get(1).value());
			fields[5].setText(bot.find("saturation").get(1).value());
			fields[6].setText(bot.find("hue").get(1).value());
			fields[7].setText(bot.find("sharpness").get(1).value());
			fields[8].setText(bot.find("gamma").get(1).value());
			fields[9].setText(bot.find("auto_whitebalance").get(1).value());
			fields[10].setText(bot.find("exposure").get(1).value());
			fields[11].setText(bot.find("gain").get(1).value());
			fields[12].setText(bot.find("white_balance").get(1).value());
			fields[13].setText(bot.find("fade_to_black").get(1).value());
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
		
		protected CameraPrefsField(String whichCamera, int naoVersion) {
			super();
			readExistingParamsFromFile(naoVersion);
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
