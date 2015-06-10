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
import messages.CameraParamsOuterClass;
import messages.CameraParamsOuterClass.CameraParams;

import java.io.File;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.BufferedWriter;
import java.io.IOException;


public class StreamingPanel extends JPanel implements ActionListener {
	
	static final long serialVersionUID = 1L;

	public StreamingPanel() {
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
		
		topCameraPrefs = new CameraPrefs("Top Camera");
		bottomCameraPrefs = new CameraPrefs("Bottom Camera");
		
		canvas.add(topCameraPrefs);
		canvas.add(bottomCameraPrefs);
		
		startStreaming = new JButton("stream");
		startStreaming.addActionListener(this);
		startStreaming.setPreferredSize(new Dimension(80,25));
		canvas.add(startStreaming);

		saveParams = new JButton("save");
		saveParams.addActionListener(this);
		saveParams.setPreferredSize(new Dimension(60,25));
		canvas.add(saveParams);
		
		sp = new JScrollPane();
		sp.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED);
		sp.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
		sp.getVerticalScrollBar().setUnitIncrement(16);
		sp.setViewportView(canvas);

		add(sp);
		
	}
	
	private void useSize(Dimension s) {
		sp.setBounds(0, 0, s.width, s.height);
		Dimension d1, d2, d3, d4;
		int y=0;
		int x = 0;
		d1 = topCameraPrefs.getPreferredSize();
		topCameraPrefs.setBounds(x,y,d1.width,d1.height);
		y += d1.height+3;
		
		d2 = bottomCameraPrefs.getPreferredSize();
		bottomCameraPrefs.setBounds(x,y,d2.width,d2.height);
		y += d2.height+3;
		
		d3 = startStreaming.getPreferredSize();
		startStreaming.setBounds(10,y,d3.width,d3.height);
		y += d3.height+3;

		d4 = saveParams.getPreferredSize();
		saveParams.setBounds(90,y-3-d3.height,d4.width,d4.height);
		y += d4.height+3;
		
		canvas.setPreferredSize(new Dimension(250,y));
	}
	
	private JScrollPane sp;
	private JPanel canvas;
	
	private JButton startStreaming;
	private JButton saveParams;
	
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
		} else if(e.getSource() == saveParams) {
			trySave(topCameraParams, bottomCameraParams);
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

	private void trySave(Integer[] topCameraPar, Integer[] bottomCameraPar) {
		SExpr top;
		SExpr bot;

		top = SExpr.list(
			SExpr.newKeyValue("whichcamera","TOP"),
			SExpr.newKeyValue("hflip",topCameraPar[0]),
			SExpr.newKeyValue("vflip",topCameraPar[1]),
			SExpr.newKeyValue("autoexposure",topCameraPar[2]),
			SExpr.newKeyValue("brightness",topCameraPar[3]),
			SExpr.newKeyValue("contrast",topCameraPar[4]),
			SExpr.newKeyValue("saturation",topCameraPar[5]),
			SExpr.newKeyValue("hue",topCameraPar[6]),
			SExpr.newKeyValue("sharpness",topCameraPar[7]),
			SExpr.newKeyValue("gamma",topCameraPar[8]),
			SExpr.newKeyValue("auto_whitebalance",topCameraPar[9]),
			SExpr.newKeyValue("exposure",topCameraPar[10]),
			SExpr.newKeyValue("gain",topCameraPar[11]),
			SExpr.newKeyValue("white_balance",topCameraPar[12]),
			SExpr.newKeyValue("fade_to_black",topCameraPar[13])
			);

		bot = SExpr.list(
			SExpr.newKeyValue("whichcamera","TOP"),
			SExpr.newKeyValue("hflip",bottomCameraPar[0]),
			SExpr.newKeyValue("vflip",bottomCameraPar[1]),
			SExpr.newKeyValue("autoexposure",bottomCameraPar[2]),
			SExpr.newKeyValue("brightness",bottomCameraPar[3]),
			SExpr.newKeyValue("contrast",bottomCameraPar[4]),
			SExpr.newKeyValue("saturation",bottomCameraPar[5]),
			SExpr.newKeyValue("hue",bottomCameraPar[6]),
			SExpr.newKeyValue("sharpness",bottomCameraPar[7]),
			SExpr.newKeyValue("gamma",bottomCameraPar[8]),
			SExpr.newKeyValue("auto_whitebalance",bottomCameraPar[9]),
			SExpr.newKeyValue("exposure",bottomCameraPar[10]),
			SExpr.newKeyValue("gain",bottomCameraPar[11]),
			SExpr.newKeyValue("white_balance",bottomCameraPar[12]),
			SExpr.newKeyValue("fade_to_black",bottomCameraPar[13])
			);
			
			String topContent = top.serialize();
			String botContent = bot.serialize();
			File topFile = new File("../../src/man/config/topCameraParams.txt");
			File botFile = new File("../../src/man/config/bottomCameraParams.txt");

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
