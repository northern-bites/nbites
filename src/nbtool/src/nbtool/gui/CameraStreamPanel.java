package nbtool.gui;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;

import nbtool.data.*;
import nbtool.util.*;
import nbtool.io.ControlIO;
import nbtool.io.ControlIO.ControlInstance;
import nbtool.util.NBConstants.*;
import nbtool.util.Logger;
import messages.CameraParams;

import java.io.File;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.BufferedWriter;
import java.io.IOException;


public class CameraStreamPanel extends JPanel implements ActionListener {
	
	static final long serialVersionUID = 1L;

	public CameraStreamPanel() {
		/*
		 GUI is done below. 
		 TODO: transfer to actual layout manager
		 */
		super();
		//setLayout(new BoxLayout(this,BoxLayout.Y_AXIS));
		setLayout(null);
		addComponentListener(new ComponentAdapter() {
			public void componentResized(ComponentEvent e) {
				useSize(e.getComponent().getSize());
			}
		});
		
		canvas = new JPanel();
		//canvas.setLayout(new BoxLayout(canvas,BoxLayout.Y_AXIS));
		canvas.setLayout(null);

		naoVersion = new JButton("V5 Camera Parameters");
		naoVersion.addActionListener(this);
		naoVersion.setPreferredSize(new Dimension(180,25));
		canvas.add(naoVersion);
		
		topCameraPrefs = new CameraPrefs("Top Camera",5);
		bottomCameraPrefs = new CameraPrefs("Bottom Camera",5);
		
		canvas.add(topCameraPrefs);
		canvas.add(bottomCameraPrefs);
		
		startStreaming = new JButton("stream");
		startStreaming.addActionListener(this);
		startStreaming.setPreferredSize(new Dimension(80,25));
		canvas.add(startStreaming);

		saveParamsV4 = new JButton("saveV4");
		saveParamsV4.addActionListener(this);
		saveParamsV4.setPreferredSize(new Dimension(80,25));
		canvas.add(saveParamsV4);

		saveParamsV5 = new JButton("saveV5");
		saveParamsV5.addActionListener(this);
		saveParamsV5.setPreferredSize(new Dimension(80,25));
		canvas.add(saveParamsV5);
		
		sp = new JScrollPane();
		sp.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED);
		sp.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
		sp.getVerticalScrollBar().setUnitIncrement(16);
		sp.setViewportView(canvas);

		add(sp);
		
	}
	
	private void useSize(Dimension s) {
		sp.setBounds(0, 0, s.width, s.height);
		Dimension d0, d1, d2, d3, d4, d5;
		int y=0;
		int x = 0;

		d0 = naoVersion.getPreferredSize();
		naoVersion.setBounds(x,y,d0.width,d0.height);
		y += d0.height+3;

		d1 = topCameraPrefs.getPreferredSize();
		topCameraPrefs.setBounds(x,y,d1.width,d1.height);
		y += d1.height+3;
		
		d2 = bottomCameraPrefs.getPreferredSize();
		bottomCameraPrefs.setBounds(x,y,d2.width,d2.height);
		y += d2.height+3;
		
		d3 = startStreaming.getPreferredSize();
		startStreaming.setBounds(10,y,d3.width,d3.height);
		y += d3.height+3;

		d4 = saveParamsV4.getPreferredSize();
		saveParamsV4.setBounds(90,y-3-d3.height,d4.width,d4.height);
		y += d4.height+3;

		d5 = saveParamsV5.getPreferredSize();
		saveParamsV5.setBounds(170,y-6-d3.height-d4.height,d4.width,d5.height);
		
		canvas.setPreferredSize(new Dimension(250,y));
	}
	
	private JScrollPane sp;
	private JPanel canvas;
	
	private JButton naoVersion;
	private JButton startStreaming;
	private JButton saveParamsV4;
	private JButton saveParamsV5;
	
	private CameraPrefs topCameraPrefs;
	private CameraPrefs bottomCameraPrefs;
	
	public Integer[] topCameraParams;
	public Integer[] bottomCameraParams;

	@Override
	public void actionPerformed(ActionEvent e) {
		
		topCameraParams = topCameraPrefs.getParameterValues();
		bottomCameraParams = bottomCameraPrefs.getParameterValues();

		if(e.getSource() == startStreaming) {
			tryStream(topCameraParams, bottomCameraParams);
		} else if(e.getSource() == saveParamsV4) {
			trySave(topCameraParams, bottomCameraParams,4);
		} else if(e.getSource() == saveParamsV5) {
			trySave(topCameraParams,bottomCameraParams,5);
		} else if(e.getSource() == naoVersion) {
			updateParamsForCorrectNaoVersion();
		}
	}
	
	private void tryStream(Integer[] topCameraParams, Integer[] bottomCameraParams) {
		CameraParams topCamera = CameraParams.newBuilder()
				.setWhichCamera("TOP")
				.setHFLIP(topCameraParams[0])
				.setVFLIP(topCameraParams[1])
				.setAutoExposure(topCameraParams[2])
				.setBrightness(topCameraParams[3])
				.setContrast(topCameraParams[4])
				.setSaturation(topCameraParams[5])
				.setHue(topCameraParams[6])
				.setSharpness(topCameraParams[7])
				.setGamma(topCameraParams[8])
				.setAutoWhiteBalance(topCameraParams[9])
				.setExposure(topCameraParams[10])
				.setGain(topCameraParams[11])
				.setWhiteBalance(topCameraParams[12])
				.setFadeToBlack(topCameraParams[13])
				.build();
		
		CameraParams bottomCamera = CameraParams.newBuilder()
				.setWhichCamera("BOTTOM")
				.setHFLIP(bottomCameraParams[0])
				.setVFLIP(bottomCameraParams[1])
				.setAutoExposure(bottomCameraParams[2])
				.setBrightness(bottomCameraParams[3])
				.setContrast(bottomCameraParams[4])
				.setSaturation(bottomCameraParams[5])
				.setHue(bottomCameraParams[6])
				.setSharpness(bottomCameraParams[7])
				.setGamma(bottomCameraParams[8])
				.setAutoWhiteBalance(bottomCameraParams[9])
				.setExposure(bottomCameraParams[10])
				.setGain(bottomCameraParams[11])
				.setWhiteBalance(bottomCameraParams[12])
				.setFadeToBlack(bottomCameraParams[13])
				.build();
		
		ControlInstance first = ControlIO.getByIndex(0);
		if (first == null) {
			Logger.log(Logger.WARN, "CameraParams clicked when no ControlIO instance available");
			return;
		}
		first.tryAddCmnd(ControlIO.createCmndSetCameraParams(topCamera));
		first.tryAddCmnd(ControlIO.createCmndSetCameraParams(bottomCamera));
	}

	private void trySave(Integer[] topCameraPar, Integer[] bottomCameraPar, int naoVersion) {
		SExpr top;
		SExpr bot;

		top = buildSExprFromArray(topCameraPar);
		bot = buildSExprFromArray(bottomCameraPar);
			
		String topContent = top.serialize();
		String botContent = bot.serialize();

		File topFile, botFile;
		if(naoVersion == 4) {
			topFile = new File("../../../src/man/config/V4topCameraParams.txt");
			botFile = new File("../../../src/man/config/V4bottomCameraParams.txt");
		} else {
			topFile = new File("../../../src/man/config/V5topCameraParams.txt");
			botFile = new File("../../../src/man/config/V5bottomCameraParams.txt");
		}
		
		try {
			BufferedWriter topOut = new BufferedWriter(new FileWriter(topFile));
			topOut.write(topContent);
			topOut.close();

			BufferedWriter botOut = new BufferedWriter(new FileWriter(botFile));
			botOut.write(botContent);
			botOut.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	private void updateParamsForCorrectNaoVersion() {
		if(naoVersion.getText().equals("V4 Camera Parameters")) {
			naoVersion.setText("V5 Camera Parameters");
			topCameraPrefs.refresh("Top Camera",5);
			bottomCameraPrefs.refresh("Bottom Camera",5);
		} else {
			naoVersion.setText("V4 Camera Parameters");
			topCameraPrefs.refresh("Top Camera",4);
			bottomCameraPrefs.refresh("Bottom Camera",4);
		}
	}

	private SExpr buildSExprFromArray(Integer[] Params) {
		SExpr s;
		s = SExpr.list(
			SExpr.newKeyValue("hflip",Params[0]),
			SExpr.newKeyValue("vflip",Params[1]),
			SExpr.newKeyValue("autoexposure",Params[2]),
			SExpr.newKeyValue("brightness",Params[3]),
			SExpr.newKeyValue("contrast",Params[4]),
			SExpr.newKeyValue("saturation",Params[5]),
			SExpr.newKeyValue("hue",Params[6]),
			SExpr.newKeyValue("sharpness",Params[7]),
			SExpr.newKeyValue("gamma",Params[8]),
			SExpr.newKeyValue("auto_whitebalance",Params[9]),
			SExpr.newKeyValue("exposure",Params[10]),
			SExpr.newKeyValue("gain",Params[11]),
			SExpr.newKeyValue("white_balance",Params[12]),
			SExpr.newKeyValue("fade_to_black",Params[13])
			);
		return s;
	}
	
	private void useStatus(STATUS s) {
		switch (s) {
		case IDLE:
			startStreaming.setText("stream");
			startStreaming.setEnabled(true);
			break;
		case RUNNING:
			startStreaming.setText("streaming");
			startStreaming.setEnabled(false);
			break;
		default:
			break;
		}
	}
	
}
